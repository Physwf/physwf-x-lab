#include "Common.hlsl"
#include "DeferredShadingCommon.hlsl"


void VS_Main(
    float2 InPostion : ATTRIBUTE0,
    float2 InUV      : ATTRIBUTE1,
    out float2 OutTexCoord : TEXCOORD0,
    out float3 OutScreenVector : TEXCOORD1,
    out float4 OutPosition : SV_POSITION
)
{
    DrawRectangle(float4(InPostion,0,1), InUV, OutPosition, OutTexCoord);
    OutScreenVector = mul(float4(OutPosition.xy,1,0),View.ScreenToTranslatedWorld).xyz;
}

