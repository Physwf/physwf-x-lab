
//Same as GeneratedUnfiromBuffers.ush in UE4


//View
#ifndef __UniformBuffer_View_Definition__
#define __UniformBuffer_View_Definition__

cbuffer View : register(b0)
{
    float4x4 View_TranslatedWorldToClip;
    float4x4 View_WorldToClip;
    float4x4 View_TranslatedWorldToView;
    float4x4 View_ViewToTranslatedWorld;
    float4x4 View_TranslatedWorldToCameraView;
    float4x4 View_CameraViewToTranslatedWorld;
    float4x4 View_ViewToClip;
    float4x4 View_ViewToClipNoAA;
    float4x4 View_ClipToView;
    float4x4 View_ClipToTranslatedWorld;
    float4x4 View_SVPositionToTranslatedWorld;
    float4x4 View_ScreenToWorld;
    float4x4 View_ScreenToTranslatedWorld;
    // half3 View_ViewForward;
    // half3 View_ViewUp;
    // half3 View_ViewRight;
    // half3 View_HMDViewNoRollUp;
    // half3 View_HMDViewNoRollRight;
    float4 View_InvDeviceZToWorldZTransform;
    // half4 View_ScreenPositionScaleBias;
    // float4 View_WorldCameraOrigin;
    // float4 View_TranslatedWorldCameraOrigin;
    // float4 View_WorldViewOrigin;


    float3 View_PreViewTranslation;
    //float View_Padding;
    float4 View_ViewRectMin;
    float4 View_ViewSizeAndInvSize;
};
static const struct
{
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
    // half3 ViewForward;
    // half3 ViewUp;
    // half3 ViewRight;
    // half3 HMDViewNoRollUp;
    // half3 HMDViewNoRollRight;
    float4 InvDeviceZToWorldZTransform;
    // half4 ScreenPositionScaleBias;
    // float4 WorldCameraOrigin;
    // float4 TranslatedWorldCameraOrigin;
    // float4 WorldViewOrigin;

    float3 PreViewTranslation;

    float4 ViewRectMin;
    float4 ViewSizeAndInvSize;
} View = 
{
    View_TranslatedWorldToClip,
    View_WorldToClip,
    View_TranslatedWorldToView,
    View_ViewToTranslatedWorld,
    View_TranslatedWorldToCameraView,
    View_CameraViewToTranslatedWorld,
    View_ViewToClip,
    View_ViewToClipNoAA,
    View_ClipToView,
    View_ClipToTranslatedWorld,
    View_SVPositionToTranslatedWorld,
    View_ScreenToWorld,
    View_ScreenToTranslatedWorld,
    // View_ViewForward,
    // View_ViewUp,
    // View_ViewRight,
    // View_HMDViewNoRollUp,
    // View_HMDViewNoRollRight,
    View_InvDeviceZToWorldZTransform,
    // View_ScreenPositionScaleBias,
    // View_WorldCameraOrigin,
    // View_TranslatedWorldCameraOrigin,
    // View_WorldViewOrigin,

    View_PreViewTranslation,

    View_ViewRectMin,
    View_ViewSizeAndInvSize
};
#endif

//Primitive
#ifndef __UniformBuffer_Primitive_Definition__
#define __UniformBuffer_Primitive_Definition__
cbuffer	Primitive : register(b1)
{
    float4x4 Primitive_LocalToWorld;
	float4x4 Primitive_WorldToLocal;
	// float4 Primitive_ObjectWorldPositionAndRadius;
	// float3 Primitive_ObjectBounds;
	// half Primitive_LocalToWorldDeterminantSign;
	// float3 Primitive_ActorWorldPosition;
	// half Primitive_DecalReceiverMask;
	float Primitive_PerObjectGBufferData;
	// half Primitive_UseSingleSampleShadowFromStationaryLights;
	// half Primitive_UseVolumetricLightmapShadowFromStationaryLights;
	// half Primitive_UseEditorDepthTest;
	// half4 Primitive_ObjectOrientation;
	// half4 Primitive_NonUniformScale;
	float4 Primitive_InvNonUniformScale;
	// float3 Primitive_LocalObjectBoundsMin;
	// float PrePadding_Primitive_252;
	// float3 Primitive_LocalObjectBoundsMax;
	// uint Primitive_LightingChannelMask;
	// float Primitive_LpvBiasMultiplier;
};
static const struct
{
    float4x4 LocalToWorld;
	float4x4 WorldToLocal;
    float PerObjectGBufferData;
    float4 InvNonUniformScale;
} Primitive = { Primitive_LocalToWorld, Primitive_WorldToLocal, Primitive_PerObjectGBufferData, Primitive_InvNonUniformScale };
#endif

//DrawRectangleParameters
#ifndef __UniformBuffer_DrawRectangleParameters_Definition__
#define __UniformBuffer_DrawRectangleParameters_Definition__

cbuffer DrawRectangleParameters : register(b2)
{
    float4 DrawRectangleParameters_PosScaleBias;
    float4 DrawRectangleParameters_UVScaleBias;
    float4 DrawRectangleParameters_InvTargetSizeAndTextureSize;
};
static const struct
{
    float4 PosScaleBias;
    float4 UVScaleBias;
    float4 InvTargetSizeAndTextureSize;
} DrawRectangleParameters = { DrawRectangleParameters_PosScaleBias, DrawRectangleParameters_UVScaleBias, DrawRectangleParameters_InvTargetSizeAndTextureSize};
#endif

//DeferredLightUniform
#ifndef __UniformBuffer_DeferredLightUniform_Definition__
#define __UniformBuffer_DeferredLightUniform_Definition__

cbuffer DeferredLightUniform : register(b3)
{
    float3 DeferredLightUniform_LightPosition;
    float DeferredLightUniform_LightInvRadius;
    float3 DeferredLightUniform_LightColor;
    float DeferredLightUniform_LightFalloffExponent;
    float3 DeferredLightUniform_NormalizedLightDirection;
    float3 DeferredLightUniform_NormalizedLightTangent;
    float2 DeferredLightUniform_SpotAngles;
    float DeferredLightUniform_SpecularScale;
    float DeferredLightUniform_SourceRadius;
    float DeferredLightUniform_SoftSourceRadius;
    float DeferredLightUniform_SourceLength;
    float DeferredLightUniform_ContactShadowLength;
    float2 DeferredLightUniform_DistanceFadeMAD;
    float4 DeferredLightUniform_ShadowMapChannelMask;
    uint DeferredLightUniform_ShadowedBits;
    uint DeferredLightUniform_LightingChannelMask;
    float DeferredLightUniform_VolumetricScatteringIntensity;
};
Texture2D DeferredLightUniform_VSourceTexture;
static const struct
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
    Texture2D VSourceTexture;
} DeferredLightUniform =
{
    DeferredLightUniform_LightPosition,
    DeferredLightUniform_LightInvRadius,
    DeferredLightUniform_LightColor,
    DeferredLightUniform_LightFalloffExponent,
    DeferredLightUniform_NormalizedLightDirection,
    DeferredLightUniform_NormalizedLightTangent,
    DeferredLightUniform_SpotAngles,
    DeferredLightUniform_SpecularScale,
    DeferredLightUniform_SourceRadius,
    DeferredLightUniform_SoftSourceRadius,
    DeferredLightUniform_SourceLength,
    DeferredLightUniform_ContactShadowLength,
    DeferredLightUniform_DistanceFadeMAD,
    DeferredLightUniform_ShadowMapChannelMask,
    DeferredLightUniform_ShadowedBits,
    DeferredLightUniform_LightingChannelMask,
    DeferredLightUniform_VolumetricScatteringIntensity,
    DeferredLightUniform_VSourceTexture
};
#endif

//SceneTexturesStruct
#ifndef __UniformBuffer_SceneTexturesStruct_Definition__
#define __UniformBuffer_SceneTexturesStruct_Definition__

Texture2D<float4> SceneTexturesStruct_SceneColorTexture;
SamplerState SceneTexturesStruct_SceneColorTextureSampler;
Texture2D<float> SceneTexturesStruct_SceneDepthTexture;
SamplerState SceneTexturesStruct_SceneDepthTextureSampler;

Texture2D<float4> SceneTexturesStruct_GBufferATexture;
Texture2D<float4> SceneTexturesStruct_GBufferBTexture;
Texture2D<float4> SceneTexturesStruct_GBufferCTexture;
Texture2D<float4> SceneTexturesStruct_GBufferDTexture;
Texture2D<float4> SceneTexturesStruct_GBufferETexture;
Texture2D<float4> SceneTexturesStruct_GBufferVelocityTexture;
SamplerState SceneTexturesStruct_GBufferATextureSampler;
SamplerState SceneTexturesStruct_GBufferBTextureSampler;
SamplerState SceneTexturesStruct_GBufferCTextureSampler;
SamplerState SceneTexturesStruct_GBufferDTextureSampler;
SamplerState SceneTexturesStruct_GBufferETextureSampler;
SamplerState SceneTexturesStruct_GBufferVelocityTextureSampler;

Texture2D<float4> SceneTexturesStruct_ScreenSpaceAOTexture;
SamplerState SceneTexturesStruct_ScreenSpaceAOTextureSampler;

static const struct
{
    Texture2D<float4> SceneColorTexture;
    SamplerState SceneColorTextureSampler;
    Texture2D<float> SceneDepthTexture;
    SamplerState SceneDepthTextureSampler;

    Texture2D<float4> GBufferATexture;
    Texture2D<float4> GBufferBTexture;
    Texture2D<float4> GBufferCTexture;
    Texture2D<float4> GBufferDTexture;
    Texture2D<float4> GBufferETexture;
    Texture2D<float4> GBufferVelocityTexture;
    SamplerState GBufferATextureSampler;
    SamplerState GBufferBTextureSampler;
    SamplerState GBufferCTextureSampler;
    SamplerState GBufferDTextureSampler;
    SamplerState GBufferETextureSampler;
    SamplerState GBufferVelocityTextureSampler;

    Texture2D<float4> ScreenSpaceAOTexture;
    SamplerState ScreenSpaceAOTextureSampler;
} SceneTexturesStruct = 
{
    SceneTexturesStruct_SceneColorTexture,
    SceneTexturesStruct_SceneColorTextureSampler,
    SceneTexturesStruct_SceneDepthTexture,
    SceneTexturesStruct_SceneDepthTextureSampler,

    SceneTexturesStruct_GBufferATexture,
    SceneTexturesStruct_GBufferBTexture,
    SceneTexturesStruct_GBufferCTexture,
    SceneTexturesStruct_GBufferDTexture,
    SceneTexturesStruct_GBufferETexture,
    SceneTexturesStruct_GBufferVelocityTexture,
    SceneTexturesStruct_GBufferATextureSampler,
    SceneTexturesStruct_GBufferBTextureSampler,
    SceneTexturesStruct_GBufferCTextureSampler,
    SceneTexturesStruct_GBufferDTextureSampler,
    SceneTexturesStruct_GBufferETextureSampler,
    SceneTexturesStruct_GBufferVelocityTextureSampler,

    SceneTexturesStruct_ScreenSpaceAOTexture,
    SceneTexturesStruct_ScreenSpaceAOTextureSampler
};
#endif
