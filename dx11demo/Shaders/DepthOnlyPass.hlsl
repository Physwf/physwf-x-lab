
#include "Common.hlsl"
#include "Material.hlsl"
#include "LocalVertexFactory.hlsl"


void VS_Main(PostionOnlyVertexFactoryInput Input, out float4 Position : SV_POSITION)
{
    ResolvedView = ResolveView();

    float4 WorldPostion =   mul(Primitive.LocalToWorld,             float4(Input.Position, 1.f));
    float4 ViewPostion =    mul(ResolvedView.TranslatedWorldToView, WorldPostion);
    Position =              mul(ResolvedView.ViewToClip,            ViewPostion);

    // float4 WorldPostion =   mul(float4(Input.Position, 1.f),    Primitive.LocalToWorld);
    // float4 ViewPostion =    mul(WorldPostion,                   ResolvedView.TranslatedWorldToView);
    // Position =              mul(ViewPostion,                    ResolvedView.ViewToClip);

    // float4 WorldPostion =   mul(float4(Input.Position, 1.f),    Primitive.LocalToWorld);
    // float4 ViewPostion =    mul(WorldPostion,                   View.TranslatedWorldToView);
    // Position =              mul(ViewPostion,                    View.ViewToClip);

    //float4x4 l2w = {1.0f,0,0,0, 0.0f,1.0f,0,0, 0.0f,0,1.0f,0, 0.0f,0,0,1.0f};
    //Position =              mul(Primitive.LocalToWorld,            float4(Input.Position, 1.f));
    //Position =              mul(View.TranslatedWorldToView,            float4(Input.Position, 1.f));

    //Position = float4(Input.Position, 1.f);
}

void PS_Main(float4 Position : SV_POSITION, out float4 Color : SV_Target/*, out float Depth : SV_Depth*/) 
{
    Color = float4(1.f,0.f,0.f,1.f);
    //Depth = 0;
}


