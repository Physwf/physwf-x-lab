#ifndef __DEFERRED_LIGHTING_COMMON__
#define __DEFERRED_LIGHTING_COMMON__

#include "DeferredShadingCommon.hlsl"
#include "DynamicLightingCommon.hlsl"
//#include "IESLightProfilesCommon.ush"
#include "CapsuleLightIntegrate.hlsl"
//#include "RectLightIntegrate.ush"

struct DeferredLightData
{
    float4 LightPositionAndInvRadius;
    float4 LightColorAndFalloffExponent;
    float3 LightDirection;
    float3 LightTangent;
    float SoftSourceRadius;
    float4 SpotAnglesAndSourceRadius;
    float SpecularScale;
    float ContactShadowLength;
    float2 DistanceFadeMAD;
    float4 ShadowMapChannelMask;
    bool ContactShadowLengthInWS;
    bool bInverseSquared;
    bool bRadialLight;
    bool bSpotLight;
	bool bRectLight;
    uint ShadowedBits;
};

#define REFERENCE_QUALITY	0

/** Returns 0 for positions closer than the fade near distance from the camera, and 1 for positions further than the fade far distance. */
float DistanceFromCameraFade(float SceneDepth, DeferredLightData LightData, float3 WorldPosition, float3 CameraPosition)
{
	// depth (non radial) based fading over distance
	float Fade = saturate(SceneDepth * LightData.DistanceFadeMAD.x + LightData.DistanceFadeMAD.y);
	return Fade * Fade;
}

void GetShadowTerms(GBufferData GBuffer, DeferredLightData LightData, float3 WorldPosition, float3 L, float4 LightAttenuation,float Dither, inout ShadowTerms Shadow)
{
    float ContactShadowLength = 0.0f;
    const float ContactShadowLengthScreenScale = View.ClipToView[1][1] * GBuffer.Depth;

    [branch]
    if(LightData.ShadowedBits)
    {
        float UsesStaticShadowMap = dot(LightData.ShadowMapChannelMask,float4(1,1,1,1));
        float StaticShadowing = lerp(1,dot(GBuffer.PrecomputedShadowFactors,LightData.ShadowMapChannelMask),UsesStaticShadowMap);

        if(LightData.bRadialLight)
        {
            Shadow.SurfaceShadow = LightAttenuation.z * StaticShadowing;
            Shadow.TransmissionShadow = LightAttenuation.w * StaticShadowing;
            Shadow.TransmissionThickness = LightAttenuation.w;
        }
        else
        {
            float DynamicShadowFraction = DistanceFromCameraFade(GBuffer.Depth, LightData, WorldPosition, View.WorldCameraOrigin);
            
            Shadow.SurfaceShadow = lerp(LightAttenuation.x, StaticShadowing, DynamicShadowFraction);
			// Fade between SSS dynamic shadowing and static shadowing based on distance
			Shadow.TransmissionShadow = min(lerp(LightAttenuation.y, StaticShadowing, DynamicShadowFraction), LightAttenuation.w);

			Shadow.SurfaceShadow *= LightAttenuation.z;
			Shadow.TransmissionShadow *= LightAttenuation.z;

			// Need this min or backscattering will leak when in shadow which cast by non perobject shadow(Only for directional light)
			Shadow.TransmissionThickness = min(LightAttenuation.y, LightAttenuation.w);
        }
    }

}

float GetLocalLightAttenuation(float3 WorldPosition, DeferredLightData LightData, inout float3 ToLight, inout float3 L)
{
    //该点的光的方向及距离
    ToLight = LightData.LightPositionAndInvRadius.xyz - WorldPosition;

    float DistanceSqr = dot(ToLight,ToLight);
    L = ToLight * rsqrt(DistanceSqr);//返回平方根倒数

    float LightMask;
    if(LightData.bInverseSquared)
    {
        //clamp(1-(d^2/r^2)^2)
        LightMask = Square(saturate(1-Square(DistanceSqr * Square(LightData.LightPositionAndInvRadius.w))));
    }
    else
    {
        LightMask = RadialAttenuation(ToLight*LightData.LightPositionAndInvRadius.w,LightData.LightColorAndFalloffExponent.w);
    }

    if(LightData.bSpotLight)
    {
        LightMask *= SpotAttenuation(L,-LightData.LightDirection,LightData.SpotAnglesAndSourceRadius.xy);
    }

    if(LightData.bRectLight)
    {
        LightMask = dot(LightData.LightDirection,L) < 0 ? 0 : LightMask;
    }

    return LightMask;
}

CapsuleLight GetCapsule(float3 ToLight, DeferredLightData LightData)
{
    CapsuleLight Capsule;
    Capsule.Length = LightData.SpotAnglesAndSourceRadius.w;
    Capsule.Radius = LightData.SpotAnglesAndSourceRadius.z;
    Capsule.SoftRadius = LightData.SoftSourceRadius;
    Capsule.DistBiasSqr = 1;
    Capsule.LightPos[0] = ToLight - 0.5 * Capsule.Length * LightData.LightTangent;
    Capsule.LightPos[1] = ToLight - 0.5 * Capsule.Length * LightData.LightTangent;
    return Capsule;
}

float4 GetDynamicLighting(float3 WorldPosition, float3 CameraVector, GBufferData GBuffer, float AmbientOcclusion, uint ShadingModelID, DeferredLightData LightData, float4 LightAttenuation, float Dither, uint2 SVPos)
{
    LightAccumulator LA = (LightAccumulator)0;

    float3 V = -CameraVector;
    float3 N = GBuffer.WorldNormal;
    float3 L = LightData.LightDirection;
    float3 ToLight = L;

    float LightMask = 1;
    if(LightData.bRadialLight)
    {
        LightMask = GetLocalLightAttenuation(WorldPosition,LightData,ToLight,L);
    }

    LA.EstimatedCost += 0.3f;

    [branch]
    if(LightMask > 0)
    {
        ShadowTerms Shadow;
        Shadow.SurfaceShadow = AmbientOcclusion;
        Shadow.TransmissionShadow = 1;
        Shadow.TransmissionThickness = 1;
        GetShadowTerms(GBuffer,LightData,WorldPosition,L,LightAttenuation,Dither,Shadow);

        LA.EstimatedCost += 0.3f;

        [branch]
        if(Shadow.SurfaceShadow + Shadow.TransmissionShadow > 0)
        {
            const bool bNeedsSeparateSubsurfaceLightAccumulation = UseSubsurfaceProfile(GBuffer.ShadingModelID);
            float3 LightColor = LightData.LightColorAndFalloffExponent.rgb;

        #if NON_DIRECTIONAL_DIRECT_LIGHTING
            // float Lighting;

            // if(LightData.bRectLight)
            // {
                
            // }
            // else
            // {

            // }

        #else
            DirectLighting Lighting;

            if(LightData.bRectLight)
            {
                Lighting.Diffuse = 0.6f;
            }
            else
            {
                CapsuleLight Capsule = GetCapsule(ToLight,LightData);

                #if REFERENCE_QUALITY
                    Lighting = IntegrateBxDF(GBuffer,N,V,Capsule,Shadow,SVPos);
                #else
                    Lighting = IntegrateBxDF(GBuffer,N,V,Capsule,Shadow,LightData.bInverseSquared);
                #endif
            }

            Lighting.Specular *= LightData.SpecularScale;
            
            LightAccumulator_Add(LA, Lighting.Diffuse + Lighting.Specular, Lighting.Diffuse,       LightColor*LightMask*Shadow.SurfaceShadow,          bNeedsSeparateSubsurfaceLightAccumulation);
            LightAccumulator_Add(LA, Lighting.Transmission,                Lighting.Transmission,  LightColor * LightMask * Shadow.TransmissionShadow, bNeedsSeparateSubsurfaceLightAccumulation);

            LA.EstimatedCost += 0.4f;	
        #endif

        }

    }
    
    return LightAccumulator_GetResult(LA);
}

#endif