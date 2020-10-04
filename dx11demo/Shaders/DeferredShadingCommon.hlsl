//#include "LightAccumulator.ush"
#include "SceneTexturesCommon.hlsl"

#define SHADINGMODELID_UNLIT				0
#define SHADINGMODELID_DEFAULT_LIT			1
#define SHADINGMODELID_SUBSURFACE			2
#define SHADINGMODELID_PREINTEGRATED_SKIN	3
#define SHADINGMODELID_CLEAR_COAT			4
#define SHADINGMODELID_SUBSURFACE_PROFILE	5
#define SHADINGMODELID_TWOSIDED_FOLIAGE		6
#define SHADINGMODELID_HAIR					7
#define SHADINGMODELID_CLOTH				8
#define SHADINGMODELID_EYE					9
#define SHADINGMODELID_NUM					10
#define SHADINGMODELID_MASK					0xF		

float EncodeShadingModelIdAndSelectiveOutputMask(uint ShadingModelId, uint SelectiveOutputMask)
{
	uint Value = (ShadingModelId & SHADINGMODELID_MASK) | SelectiveOutputMask;
	return (float)Value / (float)0xFF;
}

struct GBufferData
{
    float3 WorldNormal;
    float3 DiffuseColor;
    float3 SpecularColor;
    float3 BaseColor;
    float Metallic;
    float Specular;
    float4 CustomData;
    float IndirectIrradiance;
    float4 PrecomputedShadowFactors;
    float Roughness;
    float GBufferAO;
    uint ShadingModelID;
    uint SelectiveOutputMask;
    float PerObjectGBufferData;
    float CustomDepth;
    uint CustomStencil;
    float Depth;
    float4 Velocity;
    float3 StoredBaseColor;
    float StoredSpecular;
    float StoredMetallic;
};

float3 EncodeNormal( float3 N )
{
	return N * 0.5 + 0.5;
	//return Pack1212To888( UnitVectorToOctahedron( N ) * 0.5 + 0.5 );
}

float3 DecodeNormal( float3 N )
{
	return N * 2 - 1;
	//return OctahedronToUnitVector( Pack888To1212( N ) * 2 - 1 );
}

float3 EncodeBaseColor(float3 BaseColor)
{
	// we use sRGB on the render target to give more precision to the darks
	return BaseColor;
}

GBufferData DecodeGBufferData(
    float4 InGBufferA,
	float4 InGBufferB,
	float4 InGBufferC,
	float4 InGBufferD,
	float4 InGBufferE,
	float4 InGBufferVelocity,
	float CustomNativeDepth,
	uint CustomStencil,
	float SceneDepth,
	bool bGetNormalizedNormal,
	bool bChecker)
{
    GBufferData GBuffer;
    GBuffer.WorldNormal = DecodeNormal(InGBufferA.xyz);
    if(bGetNormalizedNormal)
    {
        GBuffer.WorldNormal = normalize(GBuffer.WorldNormal);
    }
    GBuffer.PerObjectGBufferData = InGBufferA.a;
    GBuffer.Metallic = InGBufferB.r;
    GBuffer.Specular = InGBufferB.g;
    GBuffer.Roughness = InGBufferB.b;

    //GBuffer.ShadingModelID = DecodeShadingModelId(InGBufferB.a);
	//GBuffer.SelectiveOutputMask = DecodeSelectiveOutputMask(InGBufferB.a);

    GBuffer.BaseColor = InGBufferC.rgb;
    GBuffer.GBufferAO = InGBufferC.a;

    GBuffer.CustomData = InGBufferD;


    GBuffer.StoredBaseColor = GBuffer.BaseColor;
	GBuffer.StoredMetallic = GBuffer.Metallic;
	GBuffer.StoredSpecular = GBuffer.Specular;

    return GBuffer;
}

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

float4 GetDynamicLighting(float3 WorldPosition, float3 CameraVector, GBufferData GBuffer, float AmbientOcclusion, uint ShadingModelID, DeferredLightData LightData, float4 LightAttenuation, float Dither, uint2 SVPos)
{
    return float4(0,0,0,0);
}

/** Populates OutGBufferA, B and C */
void EncodeGBuffer(
	GBufferData GBuffer,
	out float4 OutGBufferA,
	out float4 OutGBufferB,
	out float4 OutGBufferC,
	out float4 OutGBufferD,
	out float4 OutGBufferE,
	out float4 OutGBufferVelocity,
	float QuantizationBias = 0		// -0.5 to 0.5 random float. Used to bias quantization.
	)
{
    OutGBufferA.rgb = EncodeNormal( GBuffer.WorldNormal );
    OutGBufferA.a = GBuffer.PerObjectGBufferData;

    OutGBufferB.r = GBuffer.Metallic;
    OutGBufferB.g = GBuffer.Specular;
    OutGBufferB.b = GBuffer.Roughness;
    OutGBufferB.a = EncodeShadingModelIdAndSelectiveOutputMask(GBuffer.ShadingModelID, GBuffer.SelectiveOutputMask);

    OutGBufferC.rgb = EncodeBaseColor( GBuffer.BaseColor );

    // #if ALLOW_STATIC_LIGHTING
	// 	// No space for AO. Multiply IndirectIrradiance by AO instead of storing.
	// 	OutGBufferC.a = EncodeIndirectIrradiance(GBuffer.IndirectIrradiance * GBuffer.GBufferAO) + QuantizationBias * (1.0 / 255.0);
    // #else
	    OutGBufferC.a = GBuffer.GBufferAO;
    // #endif

    OutGBufferD = GBuffer.CustomData;
    OutGBufferE = GBuffer.PrecomputedShadowFactors;

    OutGBufferVelocity = 0;
}