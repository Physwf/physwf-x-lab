#include "Common.hlsl"
#include "DeferredShadingCommon.hlsl"
#include "DeferredLightingCommon.hlsl"

DeferredLightData SetupLightDataForStandardDeferred()
{
    DeferredLightData LightData;
    LightData.LightPositionAndInvRadius = float4(DeferredLightUniform.LightPosition,DeferredLightUniform.LightInvRadius);
    LightData.LightColorAndFalloffExponent = float4(DeferredLightUniform.LightColor,DeferredLightUniform.LightFalloffExponent);
    LightData.LightDirection = DeferredLightUniform.NormalizedLightDirection;
    LightData.LightTangent = DeferredLightUniform.NormalizedLightTangent;
    LightData.SpotAnglesAndSourceRadius = float4(DeferredLightUniform.SpotAngles,DeferredLightUniform.SourceRadius,DeferredLightUniform.SourceLength);
    LightData.SoftSourceRadius = DeferredLightUniform.SoftSourceRadius;
    LightData.SpecularScale = DeferredLightUniform.SpecularScale;
    LightData.ContactShadowLength = abs(DeferredLightUniform.ContactShadowLength);
    LightData.ContactShadowLengthInWS = DeferredLightUniform.ContactShadowLength < 0.0f;
    LightData.DistanceFadeMAD = DeferredLightUniform.DistanceFadeMAD;
    LightData.ShadowMapChannelMask = DeferredLightUniform.ShadowMapChannelMask;
    LightData.ShadowedBits = DeferredLightUniform.ShadowedBits;

    // LightData.bInverseSquared = INVERSE_SQUARED_FALLOFF;
    // LightData.bRadialLight = LIGHT_SOURCE_SHAPE > 0;
    // LightData.bSpotLight = LIGHT_SOURCE_SHAPE > 0;
    // LightData.bRectLight = LIGHT_SOURCE_SHAPE == 2;

    return LightData;
}


void PS_Main(
    float2 ScreenUV		: TEXCOORD0,
	float3 ScreenVector	: TEXCOORD1,
    float4 SVPos		: SV_POSITION,
    out float4 OutColor : SV_Target) 
{
    //ScreenSpaceData ScreenSpaceData = GetScreenSpaceData(ScreenUV);

   // float SceneDepth = CalcSceneDepth(ScreenUV);
   // float3 WorldPosition = ScreenVector * SceneDepth + View.WorldCameraOrigin;

   // DeferredLightData LightData = SetupLightDataForStandardDeferred();

    //float Dither = InterleavedGradientNoise( SVPos.xy, View.StateFrameIndexMod8 );
   // float Dither = 0;

    // OutColor = GetDynamicLighting(
    //     WorldPosition,
    //     CameraVector,
    //     ScreenSpaceData.GBuffer,
    //     ScreenSpaceData.AmbientOcclusion,
    //     ScreenSpaceData.GBuffer.ShadingModelID,
    //     LightData, 
    //     GetPerPixelLightAttenuation(ScreenUV),
    //     Dither, 
    //     uint2( Position.xy )
    // );    
    OutColor = float4(0.6,0.4,0.7,1.0f);
}
