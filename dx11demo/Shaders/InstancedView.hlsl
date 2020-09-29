#ifndef __UniformBuffer_InstancedView_Definition__
#define __UniformBuffer_InstancedView_Definition__
cbuffer InstancedView
{
	float4x4 InstancedView_TranslatedWorldToClip;
	float4x4 InstancedView_WorldToClip;
	float4x4 InstancedView_TranslatedWorldToView;
	float4x4 InstancedView_ViewToTranslatedWorld;
	float4x4 InstancedView_TranslatedWorldToCameraView;
	float4x4 InstancedView_CameraViewToTranslatedWorld;
	float4x4 InstancedView_ViewToClip;
	float4x4 InstancedView_ViewToClipNoAA;
	float4x4 InstancedView_ClipToView;
	float4x4 InstancedView_ClipToTranslatedWorld;
	float4x4 InstancedView_SVPositionToTranslatedWorld;
	float4x4 InstancedView_ScreenToWorld;
	float4x4 InstancedView_ScreenToTranslatedWorld;
	half3 InstancedView_ViewForward;
	half PrePadding_InstancedView_844;
	half3 InstancedView_ViewUp;
	half PrePadding_InstancedView_860;
	half3 InstancedView_ViewRight;
	half PrePadding_InstancedView_876;
	half3 InstancedView_HMDViewNoRollUp;
	half PrePadding_InstancedView_892;
	half3 InstancedView_HMDViewNoRollRight;
	half PrePadding_InstancedView_908;
	float4 InstancedView_InvDeviceZToWorldZTransform;
	half4 InstancedView_ScreenPositionScaleBias;
	float3 InstancedView_WorldCameraOrigin;
	float PrePadding_InstancedView_956;
	float3 InstancedView_TranslatedWorldCameraOrigin;
	float PrePadding_InstancedView_972;
	float3 InstancedView_WorldViewOrigin;
	float PrePadding_InstancedView_988;
	float3 InstancedView_PreViewTranslation;
	float PrePadding_InstancedView_1004;
	float4x4 InstancedView_PrevProjection;
	float4x4 InstancedView_PrevViewProj;
	float4x4 InstancedView_PrevViewRotationProj;
	float4x4 InstancedView_PrevViewToClip;
	float4x4 InstancedView_PrevClipToView;
	float4x4 InstancedView_PrevTranslatedWorldToClip;
	float4x4 InstancedView_PrevTranslatedWorldToView;
	float4x4 InstancedView_PrevViewToTranslatedWorld;
	float4x4 InstancedView_PrevTranslatedWorldToCameraView;
	float4x4 InstancedView_PrevCameraViewToTranslatedWorld;
	float3 InstancedView_PrevWorldCameraOrigin;
	float PrePadding_InstancedView_1660;
	float3 InstancedView_PrevWorldViewOrigin;
	float PrePadding_InstancedView_1676;
	float3 InstancedView_PrevPreViewTranslation;
	float PrePadding_InstancedView_1692;
	float4x4 InstancedView_PrevInvViewProj;
	float4x4 InstancedView_PrevScreenToTranslatedWorld;
	float4x4 InstancedView_ClipToPrevClip;
	float4 InstancedView_TemporalAAJitter;
	float4 InstancedView_GlobalClippingPlane;
	float2 InstancedView_FieldOfViewWideAngles;
	float2 InstancedView_PrevFieldOfViewWideAngles;
	half4 InstancedView_ViewRectMin;
	float4 InstancedView_ViewSizeAndInvSize;
	float4 InstancedView_BufferSizeAndInvSize;
	float4 InstancedView_BufferBilinearUVMinMax;
	int InstancedView_NumSceneColorMSAASamples;
	half InstancedView_PreExposure;
	half InstancedView_OneOverPreExposure;
	half PrePadding_InstancedView_2012;
	half4 InstancedView_DiffuseOverrideParameter;
	half4 InstancedView_SpecularOverrideParameter;
	half4 InstancedView_NormalOverrideParameter;
	half2 InstancedView_RoughnessOverrideParameter;
	float InstancedView_PrevFrameGameTime;
	float InstancedView_PrevFrameRealTime;
	half InstancedView_OutOfBoundsMask;
	half PrePadding_InstancedView_2084;
	half PrePadding_InstancedView_2088;
	half PrePadding_InstancedView_2092;
	float3 InstancedView_WorldCameraMovementSinceLastFrame;
	float InstancedView_CullingSign;
	half InstancedView_NearPlane;
	float InstancedView_AdaptiveTessellationFactor;
	float InstancedView_GameTime;
	float InstancedView_RealTime;
	float InstancedView_MaterialTextureMipBias;
	float InstancedView_MaterialTextureDerivativeMultiply;
	uint InstancedView_Random;
	uint InstancedView_FrameNumber;
	uint InstancedView_StateFrameIndexMod8;
	half InstancedView_CameraCut;
	half InstancedView_UnlitViewmodeMask;
	half PrePadding_InstancedView_2156;
	half4 InstancedView_DirectionalLightColor;
	half3 InstancedView_DirectionalLightDirection;
	half PrePadding_InstancedView_2188;
	float4 InstancedView_TranslucencyLightingVolumeMin[2];
	float4 InstancedView_TranslucencyLightingVolumeInvSize[2];
	float4 InstancedView_TemporalAAParams;
	float4 InstancedView_CircleDOFParams;
	float InstancedView_DepthOfFieldSensorWidth;
	float InstancedView_DepthOfFieldFocalDistance;
	float InstancedView_DepthOfFieldScale;
	float InstancedView_DepthOfFieldFocalLength;
	float InstancedView_DepthOfFieldFocalRegion;
	float InstancedView_DepthOfFieldNearTransitionRegion;
	float InstancedView_DepthOfFieldFarTransitionRegion;
	float InstancedView_MotionBlurNormalizedToPixel;
	float InstancedView_bSubsurfacePostprocessEnabled;
	float InstancedView_GeneralPurposeTweak;
	half InstancedView_DemosaicVposOffset;
	half PrePadding_InstancedView_2332;
	float3 InstancedView_IndirectLightingColorScale;
	half InstancedView_HDR32bppEncodingMode;
	float3 InstancedView_AtmosphericFogSunDirection;
	half InstancedView_AtmosphericFogSunPower;
	half InstancedView_AtmosphericFogPower;
	half InstancedView_AtmosphericFogDensityScale;
	half InstancedView_AtmosphericFogDensityOffset;
	half InstancedView_AtmosphericFogGroundOffset;
	half InstancedView_AtmosphericFogDistanceScale;
	half InstancedView_AtmosphericFogAltitudeScale;
	half InstancedView_AtmosphericFogHeightScaleRayleigh;
	half InstancedView_AtmosphericFogStartDistance;
	half InstancedView_AtmosphericFogDistanceOffset;
	half InstancedView_AtmosphericFogSunDiscScale;
	uint InstancedView_AtmosphericFogRenderMask;
	uint InstancedView_AtmosphericFogInscatterAltitudeSampleNum;
	float4 InstancedView_AtmosphericFogSunColor;
	float3 InstancedView_NormalCurvatureToRoughnessScaleBias;
	float InstancedView_RenderingReflectionCaptureMask;
	float4 InstancedView_AmbientCubemapTint;
	float InstancedView_AmbientCubemapIntensity;
	float InstancedView_SkyLightParameters;
	float PrePadding_InstancedView_2472;
	float PrePadding_InstancedView_2476;
	float4 InstancedView_SkyLightColor;
	float4 InstancedView_SkyIrradianceEnvironmentMap[7];
	float InstancedView_MobilePreviewMode;
	float InstancedView_HMDEyePaddingOffset;
	half InstancedView_ReflectionCubemapMaxMip;
	float InstancedView_ShowDecalsMask;
	uint InstancedView_DistanceFieldAOSpecularOcclusionMode;
	float InstancedView_IndirectCapsuleSelfShadowingIntensity;
	float PrePadding_InstancedView_2632;
	float PrePadding_InstancedView_2636;
	float3 InstancedView_ReflectionEnvironmentRoughnessMixingScaleBiasAndLargestWeight;
	int InstancedView_StereoPassIndex;
	float4 InstancedView_GlobalVolumeCenterAndExtent[4];
	float4 InstancedView_GlobalVolumeWorldToUVAddAndMul[4];
	float InstancedView_GlobalVolumeDimension;
	float InstancedView_GlobalVolumeTexelSize;
	float InstancedView_MaxGlobalDistance;
	float InstancedView_bCheckerboardSubsurfaceProfileRendering;
	float3 InstancedView_VolumetricFogInvGridSize;
	float PrePadding_InstancedView_2812;
	float3 InstancedView_VolumetricFogGridZParams;
	float PrePadding_InstancedView_2828;
	float2 InstancedView_VolumetricFogSVPosToVolumeUV;
	float InstancedView_VolumetricFogMaxDistance;
	float PrePadding_InstancedView_2844;
	float3 InstancedView_VolumetricLightmapWorldToUVScale;
	float PrePadding_InstancedView_2860;
	float3 InstancedView_VolumetricLightmapWorldToUVAdd;
	float PrePadding_InstancedView_2876;
	float3 InstancedView_VolumetricLightmapIndirectionTextureSize;
	float InstancedView_VolumetricLightmapBrickSize;
	float3 InstancedView_VolumetricLightmapBrickTexelSize;
	float InstancedView_StereoIPD;
}
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
	half3 ViewForward;
	half3 ViewUp;
	half3 ViewRight;
	half3 HMDViewNoRollUp;
	half3 HMDViewNoRollRight;
	float4 InvDeviceZToWorldZTransform;
	half4 ScreenPositionScaleBias;
	float3 WorldCameraOrigin;
	float3 TranslatedWorldCameraOrigin;
	float3 WorldViewOrigin;
	float3 PreViewTranslation;
	float4x4 PrevProjection;
	float4x4 PrevViewProj;
	float4x4 PrevViewRotationProj;
	float4x4 PrevViewToClip;
	float4x4 PrevClipToView;
	float4x4 PrevTranslatedWorldToClip;
	float4x4 PrevTranslatedWorldToView;
	float4x4 PrevViewToTranslatedWorld;
	float4x4 PrevTranslatedWorldToCameraView;
	float4x4 PrevCameraViewToTranslatedWorld;
	float3 PrevWorldCameraOrigin;
	float3 PrevWorldViewOrigin;
	float3 PrevPreViewTranslation;
	float4x4 PrevInvViewProj;
	float4x4 PrevScreenToTranslatedWorld;
	float4x4 ClipToPrevClip;
	float4 TemporalAAJitter;
	float4 GlobalClippingPlane;
	float2 FieldOfViewWideAngles;
	float2 PrevFieldOfViewWideAngles;
	half4 ViewRectMin;
	float4 ViewSizeAndInvSize;
	float4 BufferSizeAndInvSize;
	float4 BufferBilinearUVMinMax;
	int NumSceneColorMSAASamples;
	half PreExposure;
	half OneOverPreExposure;
	half4 DiffuseOverrideParameter;
	half4 SpecularOverrideParameter;
	half4 NormalOverrideParameter;
	half2 RoughnessOverrideParameter;
	float PrevFrameGameTime;
	float PrevFrameRealTime;
	half OutOfBoundsMask;
	float3 WorldCameraMovementSinceLastFrame;
	float CullingSign;
	half NearPlane;
	float AdaptiveTessellationFactor;
	float GameTime;
	float RealTime;
	float MaterialTextureMipBias;
	float MaterialTextureDerivativeMultiply;
	uint Random;
	uint FrameNumber;
	uint StateFrameIndexMod8;
	half CameraCut;
	half UnlitViewmodeMask;
	half4 DirectionalLightColor;
	half3 DirectionalLightDirection;
	float4 TranslucencyLightingVolumeMin[2];
	float4 TranslucencyLightingVolumeInvSize[2];
	float4 TemporalAAParams;
	float4 CircleDOFParams;
	float DepthOfFieldSensorWidth;
	float DepthOfFieldFocalDistance;
	float DepthOfFieldScale;
	float DepthOfFieldFocalLength;
	float DepthOfFieldFocalRegion;
	float DepthOfFieldNearTransitionRegion;
	float DepthOfFieldFarTransitionRegion;
	float MotionBlurNormalizedToPixel;
	float bSubsurfacePostprocessEnabled;
	float GeneralPurposeTweak;
	half DemosaicVposOffset;
	float3 IndirectLightingColorScale;
	half HDR32bppEncodingMode;
	float3 AtmosphericFogSunDirection;
	half AtmosphericFogSunPower;
	half AtmosphericFogPower;
	half AtmosphericFogDensityScale;
	half AtmosphericFogDensityOffset;
	half AtmosphericFogGroundOffset;
	half AtmosphericFogDistanceScale;
	half AtmosphericFogAltitudeScale;
	half AtmosphericFogHeightScaleRayleigh;
	half AtmosphericFogStartDistance;
	half AtmosphericFogDistanceOffset;
	half AtmosphericFogSunDiscScale;
	uint AtmosphericFogRenderMask;
	uint AtmosphericFogInscatterAltitudeSampleNum;
	float4 AtmosphericFogSunColor;
	float3 NormalCurvatureToRoughnessScaleBias;
	float RenderingReflectionCaptureMask;
	float4 AmbientCubemapTint;
	float AmbientCubemapIntensity;
	float SkyLightParameters;
	float4 SkyLightColor;
	float4 SkyIrradianceEnvironmentMap[7];
	float MobilePreviewMode;
	float HMDEyePaddingOffset;
	half ReflectionCubemapMaxMip;
	float ShowDecalsMask;
	uint DistanceFieldAOSpecularOcclusionMode;
	float IndirectCapsuleSelfShadowingIntensity;
	float3 ReflectionEnvironmentRoughnessMixingScaleBiasAndLargestWeight;
	int StereoPassIndex;
	float4 GlobalVolumeCenterAndExtent[4];
	float4 GlobalVolumeWorldToUVAddAndMul[4];
	float GlobalVolumeDimension;
	float GlobalVolumeTexelSize;
	float MaxGlobalDistance;
	float bCheckerboardSubsurfaceProfileRendering;
	float3 VolumetricFogInvGridSize;
	float3 VolumetricFogGridZParams;
	float2 VolumetricFogSVPosToVolumeUV;
	float VolumetricFogMaxDistance;
	float3 VolumetricLightmapWorldToUVScale;
	float3 VolumetricLightmapWorldToUVAdd;
	float3 VolumetricLightmapIndirectionTextureSize;
	float VolumetricLightmapBrickSize;
	float3 VolumetricLightmapBrickTexelSize;
	float StereoIPD;
} InstancedView = { InstancedView_TranslatedWorldToClip,InstancedView_WorldToClip,InstancedView_TranslatedWorldToView,InstancedView_ViewToTranslatedWorld,InstancedView_TranslatedWorldToCameraView,InstancedView_CameraViewToTranslatedWorld,InstancedView_ViewToClip,InstancedView_ViewToClipNoAA,InstancedView_ClipToView,InstancedView_ClipToTranslatedWorld,InstancedView_SVPositionToTranslatedWorld,InstancedView_ScreenToWorld,InstancedView_ScreenToTranslatedWorld,InstancedView_ViewForward,InstancedView_ViewUp,InstancedView_ViewRight,InstancedView_HMDViewNoRollUp,InstancedView_HMDViewNoRollRight,InstancedView_InvDeviceZToWorldZTransform,InstancedView_ScreenPositionScaleBias,InstancedView_WorldCameraOrigin,InstancedView_TranslatedWorldCameraOrigin,InstancedView_WorldViewOrigin,InstancedView_PreViewTranslation,InstancedView_PrevProjection,InstancedView_PrevViewProj,InstancedView_PrevViewRotationProj,InstancedView_PrevViewToClip,InstancedView_PrevClipToView,InstancedView_PrevTranslatedWorldToClip,InstancedView_PrevTranslatedWorldToView,InstancedView_PrevViewToTranslatedWorld,InstancedView_PrevTranslatedWorldToCameraView,InstancedView_PrevCameraViewToTranslatedWorld,InstancedView_PrevWorldCameraOrigin,InstancedView_PrevWorldViewOrigin,InstancedView_PrevPreViewTranslation,InstancedView_PrevInvViewProj,InstancedView_PrevScreenToTranslatedWorld,InstancedView_ClipToPrevClip,InstancedView_TemporalAAJitter,InstancedView_GlobalClippingPlane,InstancedView_FieldOfViewWideAngles,InstancedView_PrevFieldOfViewWideAngles,InstancedView_ViewRectMin,InstancedView_ViewSizeAndInvSize,InstancedView_BufferSizeAndInvSize,InstancedView_BufferBilinearUVMinMax,InstancedView_NumSceneColorMSAASamples,InstancedView_PreExposure,InstancedView_OneOverPreExposure,InstancedView_DiffuseOverrideParameter,InstancedView_SpecularOverrideParameter,InstancedView_NormalOverrideParameter,InstancedView_RoughnessOverrideParameter,InstancedView_PrevFrameGameTime,InstancedView_PrevFrameRealTime,InstancedView_OutOfBoundsMask,InstancedView_WorldCameraMovementSinceLastFrame,InstancedView_CullingSign,InstancedView_NearPlane,InstancedView_AdaptiveTessellationFactor,InstancedView_GameTime,InstancedView_RealTime,InstancedView_MaterialTextureMipBias,InstancedView_MaterialTextureDerivativeMultiply,InstancedView_Random,InstancedView_FrameNumber,InstancedView_StateFrameIndexMod8,InstancedView_CameraCut,InstancedView_UnlitViewmodeMask,InstancedView_DirectionalLightColor,InstancedView_DirectionalLightDirection,InstancedView_TranslucencyLightingVolumeMin,InstancedView_TranslucencyLightingVolumeInvSize,InstancedView_TemporalAAParams,InstancedView_CircleDOFParams,InstancedView_DepthOfFieldSensorWidth,InstancedView_DepthOfFieldFocalDistance,InstancedView_DepthOfFieldScale,InstancedView_DepthOfFieldFocalLength,InstancedView_DepthOfFieldFocalRegion,InstancedView_DepthOfFieldNearTransitionRegion,InstancedView_DepthOfFieldFarTransitionRegion,InstancedView_MotionBlurNormalizedToPixel,InstancedView_bSubsurfacePostprocessEnabled,InstancedView_GeneralPurposeTweak,InstancedView_DemosaicVposOffset,InstancedView_IndirectLightingColorScale,InstancedView_HDR32bppEncodingMode,InstancedView_AtmosphericFogSunDirection,InstancedView_AtmosphericFogSunPower,InstancedView_AtmosphericFogPower,InstancedView_AtmosphericFogDensityScale,InstancedView_AtmosphericFogDensityOffset,InstancedView_AtmosphericFogGroundOffset,InstancedView_AtmosphericFogDistanceScale,InstancedView_AtmosphericFogAltitudeScale,InstancedView_AtmosphericFogHeightScaleRayleigh,InstancedView_AtmosphericFogStartDistance,InstancedView_AtmosphericFogDistanceOffset,InstancedView_AtmosphericFogSunDiscScale,InstancedView_AtmosphericFogRenderMask,InstancedView_AtmosphericFogInscatterAltitudeSampleNum,InstancedView_AtmosphericFogSunColor,InstancedView_NormalCurvatureToRoughnessScaleBias,InstancedView_RenderingReflectionCaptureMask,InstancedView_AmbientCubemapTint,InstancedView_AmbientCubemapIntensity,InstancedView_SkyLightParameters,InstancedView_SkyLightColor,InstancedView_SkyIrradianceEnvironmentMap,InstancedView_MobilePreviewMode,InstancedView_HMDEyePaddingOffset,InstancedView_ReflectionCubemapMaxMip,InstancedView_ShowDecalsMask,InstancedView_DistanceFieldAOSpecularOcclusionMode,InstancedView_IndirectCapsuleSelfShadowingIntensity,InstancedView_ReflectionEnvironmentRoughnessMixingScaleBiasAndLargestWeight,InstancedView_StereoPassIndex,InstancedView_GlobalVolumeCenterAndExtent,InstancedView_GlobalVolumeWorldToUVAddAndMul,InstancedView_GlobalVolumeDimension,InstancedView_GlobalVolumeTexelSize,InstancedView_MaxGlobalDistance,InstancedView_bCheckerboardSubsurfaceProfileRendering,InstancedView_VolumetricFogInvGridSize,InstancedView_VolumetricFogGridZParams,InstancedView_VolumetricFogSVPosToVolumeUV,InstancedView_VolumetricFogMaxDistance,InstancedView_VolumetricLightmapWorldToUVScale,InstancedView_VolumetricLightmapWorldToUVAdd,InstancedView_VolumetricLightmapIndirectionTextureSize,InstancedView_VolumetricLightmapBrickSize,InstancedView_VolumetricLightmapBrickTexelSize,InstancedView_StereoIPD};
#endif