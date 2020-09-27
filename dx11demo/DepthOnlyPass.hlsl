struct VertexIn
{
    float3 Position 	: ATTRIBUTE0;
	float3 TangetX 		: ATTRIBUTE1;
	float3 TangetY 		: ATTRIBUTE2;
	float3 TangetZ 		: ATTRIBUTE3;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
};

cbuffer ViewUniform : register(b0)
{
	float4x4 View;
	float4x4 Proj;
};
 
cbuffer	PrimitiveUniform : register(b1)
{
	float4x4 World;
};


VertexOut VS_Main(VertexIn vin)
{
    VertexOut vout;

    float4 WorldPostion = mul(World,float4(vin.Position,1.0));
    float4 ViewPostion = mul(View,WorldPostion);
    vout.PosH = mul(Proj,ViewPostion);
    //vout.PosH = mul(VP,WorldPostion);
    //vout.PosH = mul(VP,float4(vin.Position,1.0));
    return vout;
}

float4 PS_Main(VertexOut pin) : SV_Target
{
    return float4(1.f,0.f,0.f,1.f);
}
