cbuffer Primitive
{
    float3x3 LocalToWorld;
    float3 Translation;
}

cbuffer View
{
    float4 ViewOrigin;
	float4x4 WorldToClip;
	float4x4 ClipToWorld;
	float4x4 SvPositionToWorld;
	float4 ViewSizeAndInvSize;
	float4 ViewRectMin;
}

struct VSInput
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float4 Tangent :TANGENT;
    float2 UV : TEXCOORD;
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