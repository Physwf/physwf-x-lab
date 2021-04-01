
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
    float4 Position : POSITION;
}

struct VSOutput
{
    float4 SVPosition : SV_Position;
}

VSOutput VSMain(VSInput Input)
{
    VSOutput Output = (VSOutput)0;
    float4 WorldPosition = mul(Input.Position,LocalToWorld);
    Output.SVPosition = mul(WorldPosition,WorldToClip);
}