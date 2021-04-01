cbuffer Primitive
{
    float4x4 LocalToWorld;
}

cbuffer View
{
    float4x4 WorldToClip;
}

struct VSInput
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
}

struct VSOutput
{
    float4 SVPosition : SV_Position;
    float3 Normal : TEXCOORD;
}

VSOutput VSMain(VSInput Input)
{
    VSOutput Output = (VSOutput)0;


    return Output;
}

struct Light
{
    float3 LightPosition;
    
}

Texture<float> ShadowDepth;
SamplerState ShadowDepthSampler;
Texture<float4> DiffuseMap;
SamplerState DiffuseMapSampler;

void PSMain()
{

}