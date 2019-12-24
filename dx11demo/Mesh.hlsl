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

cbuffer CAMERA_CBUFFER : register(b0)
{
	float4x4 View;
	float4x4 Proj;
};
 
cbuffer	ACTOR_CBUFFER : register(b1)
{
	float4x4 World;
};

struct AmbientLight
{
	float3 Color;
};

struct DirectionalLight
{
	float3 Direction;
	float3 Color;
};

struct PointLight
{
	float4 PoistionAndRadias;
	float4 ColorAndFalloffExponent;
};

cbuffer LIGHT_CBUFFER : register(b0)
{
	AmbientLight AmLight;
	DirectionalLight DirLight;
	PointLight PointLights[8];
};


VertexOut VS_Main(VertexIn vin)
{
    VertexOut vout;

    float4 WorldPostion = mul(World,float4(vin.Position,1.0));
	float4x4 VP = mul(Proj, View);
    vout.PosH = mul(VP,WorldPostion);
    vout.Color = float4(0.1,0.2,0.4,1);
    
    return vout;
}

float4 PS_Main(VertexOut pin) : SV_Target
{
    return pin.Color;
}
