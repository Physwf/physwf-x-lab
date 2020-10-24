
cbuffer View : register(b0)
{
    float4x4 WorldToClip;
};

void VS_Main(
    float2 InPosition:ATTRIBUTE0,
    out float4 OutPosition:SV_Position
)
{
    OutPosition = mul(float4(InPosition.x,InPosition.y,0.5f,1),WorldToClip);
}

void PS_Main(float4 InputPosition:SV_Position, out float4 OutColor : SV_Target)
{
    OutColor = float4(0,0,1,1);
}