//SceneTexturesStruct
#ifndef __UniformBuffer_SceneTexturesStruct_Definition__
#define __UniformBuffer_SceneTexturesStruct_Definition__

//FSceneTexturesUniformParameters

Texture2D<float4> SceneTexturesStruct_SceneColorTexture : register(t0);
SamplerState SceneTexturesStruct_SceneColorTextureSampler : register(s0);

Texture2D<float> SceneTexturesStruct_SceneDepthTexture : register(t1);
SamplerState SceneTexturesStruct_SceneDepthTextureSampler: register(s1);
Texture2D<float> SceneTexturesStruct_SceneDepthTextureNonMS: register(t2);

Texture2D<float4> SceneTexturesStruct_GBufferATexture: register(t3);
Texture2D<float4> SceneTexturesStruct_GBufferBTexture: register(t4);
Texture2D<float4> SceneTexturesStruct_GBufferCTexture: register(t5);
Texture2D<float4> SceneTexturesStruct_GBufferDTexture: register(t6);
Texture2D<float4> SceneTexturesStruct_GBufferETexture: register(t7);
Texture2D<float4> SceneTexturesStruct_GBufferVelocityTexture: register(t8);
Texture2D<float4> SceneTexturesStruct_GBufferATextureNonMS: register(t9);
Texture2D<float4> SceneTexturesStruct_GBufferBTextureNonMS: register(t10);
Texture2D<float4> SceneTexturesStruct_GBufferCTextureNonMS: register(t11);
Texture2D<float4> SceneTexturesStruct_GBufferDTextureNonMS: register(t12);
Texture2D<float4> SceneTexturesStruct_GBufferETextureNonMS: register(t13);
Texture2D<float4> SceneTexturesStruct_GBufferVelocityTextureNonMS : register(t14);
SamplerState SceneTexturesStruct_GBufferATextureSampler : register(s3);
SamplerState SceneTexturesStruct_GBufferBTextureSampler : register(s4);
SamplerState SceneTexturesStruct_GBufferCTextureSampler : register(s5);
SamplerState SceneTexturesStruct_GBufferDTextureSampler : register(s6);
SamplerState SceneTexturesStruct_GBufferETextureSampler : register(s7);
SamplerState SceneTexturesStruct_GBufferVelocityTextureSampler : register(s8);
Texture2D<float4> SceneTexturesStruct_ScreenSpaceAOTexture;
SamplerState SceneTexturesStruct_ScreenSpaceAOTextureSampler;
Texture2D<float> SceneTexturesStruct_CustomDepthTextureNonMS;
Texture2D SceneTexturesStruct_CustomDepthTexture;
SamplerState SceneTexturesStruct_CustomDepthTextureSampler;
Texture2D<uint2> SceneTexturesStruct_CustomStencilTexture;
Texture2D<uint2> SceneTexturesStruct_SceneStencilTexture;
Texture2D SceneTexturesStruct_EyeAdaptation;
Texture2D SceneTexturesStruct_SceneColorCopyTexture;
SamplerState SceneTexturesStruct_SceneColorCopyTextureSampler;

static const struct
{
    Texture2D SceneColorTexture;
	SamplerState SceneColorTextureSampler;
	Texture2D SceneDepthTexture;
	SamplerState SceneDepthTextureSampler;
	Texture2D<float> SceneDepthTextureNonMS;
	Texture2D GBufferATexture;
	Texture2D GBufferBTexture;
	Texture2D GBufferCTexture;
	Texture2D GBufferDTexture;
	Texture2D GBufferETexture;
	Texture2D GBufferVelocityTexture;
	Texture2D<float4> GBufferATextureNonMS;
	Texture2D<float4> GBufferBTextureNonMS;
	Texture2D<float4> GBufferCTextureNonMS;
	Texture2D<float4> GBufferDTextureNonMS;
	Texture2D<float4> GBufferETextureNonMS;
	Texture2D<float4> GBufferVelocityTextureNonMS;
	SamplerState GBufferATextureSampler;
	SamplerState GBufferBTextureSampler;
	SamplerState GBufferCTextureSampler;
	SamplerState GBufferDTextureSampler;
	SamplerState GBufferETextureSampler;
	SamplerState GBufferVelocityTextureSampler;
	Texture2D ScreenSpaceAOTexture;
	SamplerState ScreenSpaceAOTextureSampler;
	Texture2D<float> CustomDepthTextureNonMS;
	Texture2D CustomDepthTexture;
	SamplerState CustomDepthTextureSampler;
	Texture2D<uint2> CustomStencilTexture;
    Texture2D<uint2> SceneStencilTexture; 
	Texture2D EyeAdaptation;
	Texture2D SceneColorCopyTexture;
	SamplerState SceneColorCopyTextureSampler;

} SceneTexturesStruct = 
{ 
    SceneTexturesStruct_SceneColorTexture,
    SceneTexturesStruct_SceneColorTextureSampler,
    SceneTexturesStruct_SceneDepthTexture,
    SceneTexturesStruct_SceneDepthTextureSampler,
    SceneTexturesStruct_SceneDepthTextureNonMS,
    SceneTexturesStruct_GBufferATexture,
    SceneTexturesStruct_GBufferBTexture,
    SceneTexturesStruct_GBufferCTexture,
    SceneTexturesStruct_GBufferDTexture,
    SceneTexturesStruct_GBufferETexture,
    SceneTexturesStruct_GBufferVelocityTexture,
    SceneTexturesStruct_GBufferATextureNonMS,
    SceneTexturesStruct_GBufferBTextureNonMS,
    SceneTexturesStruct_GBufferCTextureNonMS,
    SceneTexturesStruct_GBufferDTextureNonMS,
    SceneTexturesStruct_GBufferETextureNonMS,
    SceneTexturesStruct_GBufferVelocityTextureNonMS,
    SceneTexturesStruct_GBufferATextureSampler,
    SceneTexturesStruct_GBufferBTextureSampler,
    SceneTexturesStruct_GBufferCTextureSampler,
    SceneTexturesStruct_GBufferDTextureSampler,
    SceneTexturesStruct_GBufferETextureSampler,
    SceneTexturesStruct_GBufferVelocityTextureSampler,
    SceneTexturesStruct_ScreenSpaceAOTexture,
    SceneTexturesStruct_ScreenSpaceAOTextureSampler,
    SceneTexturesStruct_CustomDepthTextureNonMS,
    SceneTexturesStruct_CustomDepthTexture,
    SceneTexturesStruct_CustomDepthTextureSampler  ,
    SceneTexturesStruct_CustomStencilTexture  ,
    SceneTexturesStruct_SceneStencilTexture ,
    SceneTexturesStruct_EyeAdaptation,
    SceneTexturesStruct_SceneColorCopyTexture,
    SceneTexturesStruct_SceneColorCopyTextureSampler

};

#endif