
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

/*
    The pixel shader can optionally take the position, but it doesn't have to. The (x,y) are in pixel coordinates. 
    The vertex output ('clip space') is converted to pixels by using the viewport state that was provided in D3D11_VIEWPORT.
    https://stackoverflow.com/questions/46527515/directx-11-pixel-shader-what-is-sv-position
*/
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
    float SSSTransmission = 0.5f;

    float ShadowDepth = Texture2DSampleLevel(ShadowDepthTexture, ShadowDepthTextureSampler, ShadowPosition.xy, 0).r;
    Shadow = LightSpacePixelDepthForOpaque > ShadowDepth;

    Shadow = saturate( (Shadow - 0.5) * ShadowSharpen + 0.5 );

    float FadedShadow = lerp(1.0f, Square(Shadow), ShadowFadeFraction);

    float FadedSSSShadow = lerp(1.0f, Square(SSSTransmission), ShadowFadeFraction);

    OutColor = EncodeLightAttenuation(half4(FadedShadow, FadedSSSShadow, FadedShadow, FadedSSSShadow));
}