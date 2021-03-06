
//Same as GeneratedUnfiromBuffers.ush in UE4

#include "View.hlsl"
#include "Primitive.hlsl"
#include "DrawRectangleParameters.hlsl"
#include "DeferredLightUniform.hlsl"
#include "SceneTexturesStruct.hlsl"

/*
BasePassRendering.h
BEGIN_UNIFORM_BUFFER_STRUCT(FSharedBasePassUniformParameters,)
	UNIFORM_MEMBER_STRUCT(FForwardLightData, Forward)
	UNIFORM_MEMBER_STRUCT(FForwardLightData, ForwardISR)
	UNIFORM_MEMBER_STRUCT(FReflectionUniformParameters, Reflection)
	UNIFORM_MEMBER_STRUCT(FFogUniformParameters, Fog)
	UNIFORM_MEMBER_TEXTURE(Texture2D, SSProfilesTexture)
END_UNIFORM_BUFFER_STRUCT(FSharedBasePassUniformParameters)

BEGIN_UNIFORM_BUFFER_STRUCT(FOpaqueBasePassUniformParameters,)
	UNIFORM_MEMBER_STRUCT(FSharedBasePassUniformParameters, Shared)
	// Forward shading 
	UNIFORM_MEMBER_TEXTURE(Texture2D, ForwardScreenSpaceShadowMaskTexture)
	UNIFORM_MEMBER_SAMPLER(SamplerState, ForwardScreenSpaceShadowMaskTextureSampler)
	UNIFORM_MEMBER_TEXTURE(Texture2D, IndirectOcclusionTexture)
	UNIFORM_MEMBER_SAMPLER(SamplerState, IndirectOcclusionTextureSampler)
	UNIFORM_MEMBER_TEXTURE(Texture2D, ResolvedSceneDepthTexture)
	// DBuffer decals
	UNIFORM_MEMBER_TEXTURE(Texture2D, DBufferATexture)
	UNIFORM_MEMBER_SAMPLER(SamplerState, DBufferATextureSampler)
	UNIFORM_MEMBER_TEXTURE(Texture2D, DBufferBTexture)
	UNIFORM_MEMBER_SAMPLER(SamplerState, DBufferBTextureSampler)
	UNIFORM_MEMBER_TEXTURE(Texture2D, DBufferCTexture)
	UNIFORM_MEMBER_SAMPLER(SamplerState, DBufferCTextureSampler)
	UNIFORM_MEMBER_TEXTURE(Texture2D<uint>, DBufferRenderMask)
	// Misc
	UNIFORM_MEMBER_TEXTURE(Texture2D, EyeAdaptation)
END_UNIFORM_BUFFER_STRUCT(FOpaqueBasePassUniformParameters)

BEGIN_UNIFORM_BUFFER_STRUCT(FTranslucentBasePassUniformParameters,)
	UNIFORM_MEMBER_STRUCT(FSharedBasePassUniformParameters, Shared)
	UNIFORM_MEMBER_STRUCT(FSceneTexturesUniformParameters, SceneTextures)
	// Material SSR
	UNIFORM_MEMBER(FVector4, HZBUvFactorAndInvFactor)
	UNIFORM_MEMBER(FVector4, PrevScreenPositionScaleBias)
	UNIFORM_MEMBER(float, PrevSceneColorPreExposureInv)
	UNIFORM_MEMBER_TEXTURE(Texture2D, HZBTexture)
	UNIFORM_MEMBER_SAMPLER(SamplerState, HZBSampler)
	UNIFORM_MEMBER_TEXTURE(Texture2D, PrevSceneColor)
	UNIFORM_MEMBER_SAMPLER(SamplerState, PrevSceneColorSampler)
	// Translucency Lighting Volume
	UNIFORM_MEMBER_TEXTURE(Texture3D, TranslucencyLightingVolumeAmbientInner)
	UNIFORM_MEMBER_SAMPLER(SamplerState, TranslucencyLightingVolumeAmbientInnerSampler)
	UNIFORM_MEMBER_TEXTURE(Texture3D, TranslucencyLightingVolumeAmbientOuter)
	UNIFORM_MEMBER_SAMPLER(SamplerState, TranslucencyLightingVolumeAmbientOuterSampler)
	UNIFORM_MEMBER_TEXTURE(Texture3D, TranslucencyLightingVolumeDirectionalInner)
	UNIFORM_MEMBER_SAMPLER(SamplerState, TranslucencyLightingVolumeDirectionalInnerSampler)
	UNIFORM_MEMBER_TEXTURE(Texture3D, TranslucencyLightingVolumeDirectionalOuter)
	UNIFORM_MEMBER_SAMPLER(SamplerState, TranslucencyLightingVolumeDirectionalOuterSampler)
END_UNIFORM_BUFFER_STRUCT(FTranslucentBasePassUniformParameters)
*/