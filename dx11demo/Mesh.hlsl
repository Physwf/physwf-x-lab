struct VertexIn
{
    float3 Position 	: POSITION;
	float2 TexCoord1	: TEXCOORD1;
	float2 TexCoord2	: TEXCOORD2;
    float3 Normal 		: NORMAL;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
    float4 Color : COLOR;
};

float4x4 View;
float4x4 Proj; 

VertexOut VS_Main(VertexIn vin)
{
    VertexOut vout;

    vout.PosH = Proj * View * float4(vin.Position,1.0);
    vout.Color = vin.Color;

    return vout;
}

float4 PS_Main(VertexOut pin) : SV_Target
{
    return pin.Color;
}
