
#include "Common.hlsl"

#include "ShadowProjectionCommon.hlsl"

#include "DeferredShadingCommon.hlsl"
#include "DynamicLightingCommon.hlsl"

float ShadowFadeFraction;
float ShadowSharpen;
float4 LightPositionAndInvRadius;

float4x4 ScreenToShadowMatrix;
	// .x:DepthBias, y: MaxSubjectZ - MinSubjectZ
float2 ProjectionDepthBiasParameters;

[earlydepthstencil]
void Main(in float4 SVPos : SV_Position, out float4 OutColor : SV_Target0)
{
    float2 ScreenUV = float2(SVPos.xy * View.BufferSizeAndInvSize.zw);
    float SceneW = CalcSceneDepth(ScreenUV);
    float4 ScreenPosition = float4(((ScreenUV.xy - View.ScreenPositionScaleBias.wz ) / View.ScreenPositionScaleBias.xy) * SceneW, SceneW, 1);
    float4 ShadowPosition = mul(ScreenPosition, ScreenToShadowMatrix);
    float3 WorldPosition = mul(ScreenPosition, View.ScreenToWorld).xyz;

    float ShadowZ = ShadowPosition.z;
    ShadowPosition.xyz /= ShadowPosition.w;

    float LightSpacePixelDepthForOpaque = min(ShadowZ,0.99999f);
    //float LightSpacePixelDepthForSSS = ShadowZ;

    float Shadow = 1;
    float SSSTransmission = 1;

    Shadow = LightSpacePixelDepthForOpaque < Texture2DSampleLevel(ShadowDepthTexture, ShadowDepthTextureSampler, ShadowPosition.xy, 0).r;

    Shadow = saturate( (Shadow - 0.5) * ShadowSharpen + 0.5 );

    float FadedShadow = lerp(1.0f, Square(Shadow), ShadowFadeFraction);

    float FadedSSSShadow = lerp(1.0f, Square(SSSTransmission), ShadowFadeFraction);

    OutColor = EncodeLightAttenuation(half4(FadedShadow, FadedSSSShadow, FadedShadow, FadedSSSShadow));
}