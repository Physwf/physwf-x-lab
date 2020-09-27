
struct VertexFactoryInput
{
    float3 Position 	        : ATTRIBUTE0;
	float3 TangetX 		        : ATTRIBUTE1;
	float3 TangetZ 		        : ATTRIBUTE2;
	float4 Color 		        : ATTRIBUTE3;
    float2 TexCoords[1]         : ATTRIBUTE4;
    float2	LightMapCoordinate  : ATTRIBUTE5;
};

struct PostionOnlyVertexFactoryInput
{
    float3 Position 	        : ATTRIBUTE0;
};

cbuffer View : register(b0)
{
	float4x4 WorldToView;
	float4x4 ViewToProj;
    float4x4 TranslatedWorldToClip;
    float4x4 WorldToClip;
    float4x4 TranslatedWorldToView;
    float4x4 ViewToTranslatedWorld;
    float4x4 TranslatedWorldToCameraView;
    float4x4 CameraViewToTranslatedWorld;
    float4x4 ViewToClip;
    float4x4 ViewToClipNoAA;
    float4x4 ClipToView;
    float4x4 ClipToTranslatedWorld;
    float4x4 SVPositionToTranslatedWorld;
    float4x4 ScreenToWorld;
    float4x4 ScreenToTranslatedWorld;
    half3 ViewForward;
    half3 ViewUp;
    half3 ViewRight;
    // half3 HMDViewNoRollUp;
    // half3 HMDViewNoRollRight;
    float4 InvDeviceZToWorldZTransform;
    half4 ScreenPositionScaleBias;
    float4 WorldCameraOrigin;
    float4 TranslatedWorldCameraOrigin;
    float4 WorldViewOrigin;
};

cbuffer	Primitive : register(b1)
{
	float4x4 LocalToWorld;
	float4x4 WorldToLocal;
	// float4 ObjectWorldPositionAndRadius;
    // float3 ObjectBounds;
    // half LocalToWorldDeterminantSign;
    // float3 ActorWorldPosition;
    // float DecalReceiverMask;
    // float PerObjectGBufferData;
    // float UseSingleSampleShadowFromStationaryLights;
    // float UseVolumetricLightmapShadowFromStationaryLights;
    // float UseEditorDepthTest;
    // float4 ObjectOrientation;
    // float4 NonUniformScale;
    // float4 InvNonUniformScale;
    // float3 LocalObjectBoundsMin;
    // float3 LocalObjectBoundsMax;
    // uint LightingChannelMask;
    // float LpvBiasMultiplier;
};

cbuffer DrawRectangleParameters : register(b2)
{
    float4 PosScaleBias;
    float4 UVScaleBias;
    float4 InvTargetSizeAndTextureSize;
};

cbuffer DeferredLightUniform : register(b3)
{
    float3 LightPosition;
    float LightInvRadius;
    float3 LightColor;
    float LightFalloffExponent;
    float3 NormalizedLightDirection;
    float3 NormalizedLightTangent;
    float2 SpotAngles;
    float SpecularScale;
    float SourceRadius;
    float SoftSourceRadius;
    float SourceLength;
    float ContactShadowLength;
    float2 DistanceFadeMAD;
    float4 ShadowMapChannelMask;
    uint ShadowedBits;
    uint LightingChannelMask;
    float VolumetricScatteringIntensity;
    float VolumetricScatteringIntensity;
    Texture2D SourceTexture;
};



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

float ConvertFromDeviceZ(float DeviceZ)
{
    return DeviceZ * View.InvDeviceZToWorldZTransform[0] + View.InvDeviceZToWorldZTransform[1] + 1.0f / (DeviceZ * View.InvDeviceZToWorldZTransform[2] - View.InvDeviceZToWorldZTransform[3]);
}

float4 CalSceneDepth(float2 ScreenUV)
{
    float DeviceZ = SceneDepthTextureSampler.Sample(SceneDepthTexture,ScreenUV);
    return ConvertFromDeviceZ(DeviceZ);
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
    LightData.LightPositionAndInvRadius = float4(DeferredLightUniform.LightPositionAndInvRadius,DeferredLightUniform.LightInvRadius);
    LightData.LightColorAndFalloffExponent = float4(DeferredLightUniform.LightColorAndFalloffExponent,DeferredLightUniform.LightFalloffExponent);
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

float4 GetPerPixelLightAttenuation(float2 UV)
{
    return float2(0,0);
	return Square(Texture2DSampleLevel(LightAttenuationTexture, LightAttenuationTextureSampler, UV, 0));
}

float4 GetDynamicLighting(float3 WorldPosition, float3 CameraVector, FGBufferData GBuffer, float AmbientOcclusion, uint ShadingModelID, FDeferredLightData LightData, float4 LightAttenuation, float Dither, uint2 SVPos)
{
    return float4();
}