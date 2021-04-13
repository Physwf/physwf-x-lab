cbuffer Primitive
{
    float3x3 LocalToWorld;
    float3 Translation;
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
    float3 WorldNormal : TEXCOORD;
}

VSOutput VSMain(VSInput Input)
{
    VSOutput Output = (VSOutput)0;
    float3 WorldPosition = mul(Input.Position,LocalToWorld);
    Output.SV_Position = mul(float4(WorldPosition,1.0f),WorldToClip);
    Output.WorldNormal = mul(Normal,LocalToWorld);

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

void PSMain(VSOutput Input,out float4 OutColor)
{

}