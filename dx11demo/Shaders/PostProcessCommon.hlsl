#ifndef __POST_PROCESS_COMMON__
#define __POST_PROCESS_COMMON__

#include "SceneTexturesCommon.hlsl"

/// e.g.
//   PostprocessInput0.SampleLevel(PostprocessInput0Sampler, float2 UV, 0)
//   Texture2DSample(PostprocessInput0, PostprocessInput0Sampler, float2 UV)
//   PostprocessInput0.Load(Texel.xyz)
//   PostprocessInput0.GetDimensions(uint, out uint width, out uint height, out uint levels)
//   PostprocessInput0.GetDimensions(uint, out float width, out float height, out float levels)
//   PostprocessInput0.GetDimensions(out uint width, out uint height)
//   PostprocessInput0.GetDimensions(out float width, out float height)
Texture2D PostprocessInput0;
SamplerState PostprocessInput0Sampler;
Texture2D PostprocessInput1;
SamplerState PostprocessInput1Sampler;
Texture2D PostprocessInput2;
SamplerState PostprocessInput2Sampler;
Texture2D PostprocessInput3;
SamplerState PostprocessInput3Sampler;
Texture2D PostprocessInput4;
SamplerState PostprocessInput4Sampler;
Texture2D PostprocessInput5;
SamplerState PostprocessInput5Sampler;
Texture2D PostprocessInput6;
SamplerState PostprocessInput6Sampler;
Texture2D PostprocessInput7;
SamplerState PostprocessInput7Sampler;
Texture2D PostprocessInput8;
SamplerState PostprocessInput8Sampler;
Texture2D PostprocessInput9;
SamplerState PostprocessInput9Sampler;
Texture2D PostprocessInput10;
SamplerState PostprocessInput10Sampler;

// width, height, 1/width, 1/height
float4 PostprocessInput0Size;
float4 PostprocessInput1Size;
float4 PostprocessInput2Size;
float4 PostprocessInput3Size;
float4 PostprocessInput4Size;
float4 PostprocessInput5Size;
float4 PostprocessInput6Size;
float4 PostprocessInput7Size;
float4 PostprocessInput8Size;
float4 PostprocessInput9Size;
float4 PostprocessInput10Size;

// xy = min valid BufferUV for a bilinear sampler in PostprocessInput%d, zw = max valid BufferUV for a bilinear sampler.
float4 PostprocessInput0MinMax;
float4 PostprocessInput1MinMax;
float4 PostprocessInput2MinMax;
float4 PostprocessInput3MinMax;
float4 PostprocessInput4MinMax;
float4 PostprocessInput5MinMax;
float4 PostprocessInput6MinMax;
float4 PostprocessInput7MinMax;
float4 PostprocessInput8MinMax;
float4 PostprocessInput9MinMax;
float4 PostprocessInput10MinMax;



#endif // __POST_PROCESS_COMMON__