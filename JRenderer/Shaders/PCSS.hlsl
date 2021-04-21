cbuffer Primitive
{
    float4x4 LocalToWorld;
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
    float4 WorldPosition = mul(LocalToWorld,float4(Input.Position,1.0f));
    Output.SVPosition = mul(WorldToClip,WorldPosition);
    return Output;
}

float4 PSMain(VSOutput Input) :SV_Target
{
    return 0;
}