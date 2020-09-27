#include "DeferredShadingCommon.hlsl"

struct VertexOut
{
    float4 PosH : SV_POSITION;
};

VertexOut VS_Main(VertexFactoryInput Input)
{
    VertexOut vout;

    float4 WorldPostion = mul(LocalToWorld,float4(Input.Position,1.0));
    float4 ViewPostion = mul(WorldToView,WorldPostion);
    vout.PosH = mul(ViewToProj,ViewPostion);
    return vout;
}

void PS_Main(VertexOut Input,out float4 Color : SV_Target, out float Depth : SV_Depth) 
{
    Color = float4(1.f,0.f,0.f,1.f);
    Depth = 1.0f / Input.Position.w;
}
