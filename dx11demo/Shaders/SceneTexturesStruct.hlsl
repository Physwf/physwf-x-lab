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