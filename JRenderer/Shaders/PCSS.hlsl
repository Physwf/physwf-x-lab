
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
};

VSOutput VSMain(VSInput Input)
{
    VSOutput Output = (VSOutput)0;
    float3 WorldPosition = mul(Input.Position,LocalToWorld) + Translation;
    Output.SVPosition = mul(float4(WorldPosition,1.0f),WorldToClip);
    return Output;
}