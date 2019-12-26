struct VertexIn
{
    float3 Position 	: POSITION;
	float3 Normal 		: NORMAL;
	float2 TexCoord0	: TEXCOORD;
	float2 TexCoord1	: TEXCOORD;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
    float4 Color : COLOR;
};

cbuffer ViewUniform : register(b0)
{
	float4x4 WorldToView;
	float4x4 ViewToClip;
	float4x4 WorldToClip;
};
 
cbuffer	PrimitiveUniform : register(b1)
{
	float4x4 LocalToWorld;
};

cbuffer	DirectionalLightUniform: register(b2)
{
	float4 DirectionalLightColor;
	float4 DirectionalLightDirectionAndShadowTransition;
};


VertexOut VS_Main(VertexIn vin)
{
    VertexOut vout;

    float4 WorldPostion = mul(LocalToWorld,float4(vin.Position,1.0));
	float4x4 VP = mul(ViewToClip, WorldToView);
    vout.PosH = mul(VP,WorldPostion);
    vout.Color = float4(0.1,0.2,0.4,1);
    
    return vout;
}

float4 PS_Main(VertexOut pin) : SV_Target
{
    return pin.Color;
}
