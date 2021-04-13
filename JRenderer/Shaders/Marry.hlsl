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
};

struct VSOutput
{
    float4 SVPosition : SV_Position;
    float3 WorldNormal : TEXCOORD;
};

VSOutput VSMain(VSInput Input)
{
    VSOutput Output = (VSOutput)0;
    float3 WorldPosition = mul(Input.Position,LocalToWorld);
    Output.SVPosition = mul(float4(WorldPosition,1.0f),WorldToClip);
    Output.WorldNormal = mul(Input.Normal,LocalToWorld);

    return Output;
}

struct Light
{
    float4 LightPositionAndRadius;
    float4 LightDirectionAndNeerPlane;
};

Texture2D<float> ShadowDepth;
SamplerState ShadowDepthSampler;
Texture2D<float4> DiffuseMap;
SamplerState DiffuseMapSampler;

void PSMain(VSOutput Input,out float4 OutColor)
{
    float3 WorldPosition;
}