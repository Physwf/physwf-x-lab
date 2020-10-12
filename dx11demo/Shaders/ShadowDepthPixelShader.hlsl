#include "Common.hlsl"
#include "Material.hlsl"
#include "LocalVertexFactory.hlsl"
#include "ShadowDepthCommon.hlsl"

void PS_Main(/*ShadowDepthVSToPS Inputs,*/ in float4 SvPosition : SV_Position, out float Depth : SV_Depth)
{
    //ResolvedView = ResolveView();
    Depth = SvPosition.z;
}