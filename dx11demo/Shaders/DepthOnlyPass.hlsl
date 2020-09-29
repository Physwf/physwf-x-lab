
#include "Common.hlsl"
#include "Material.hlsl"
#include "LocalVertexFactory.hlsl"

struct VertexOut
{
    float4 Position : SV_POSITION;
};


VertexOut VS_Main(VertexFactoryInput Input)
{
    VertexOut vout;

    ResolvedView = ResolveView();

    float4 WorldPostion = mul(Primitive.LocalToWorld,Input.Position);
    float4 ViewPostion = mul(ResolvedView.TranslatedWorldToView,WorldPostion);
    vout.Position = mul(ResolvedView.ViewToClip,ViewPostion);
    return vout;
}

void PS_Main(VertexOut Input,out float4 Color : SV_Target, out float Depth : SV_Depth) 
{
    Color = float4(1.f,0.f,0.f,1.f);
    Depth = 1.0f / Input.Position.w;
}


