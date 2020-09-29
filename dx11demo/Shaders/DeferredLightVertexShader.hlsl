#include "Common.ush"
#include "DeferredShadingCommond.hlsl"

struct VS_Input
{
    float2 Postion : ATTRIBUTE0;
    float2 UV      : ATTRIBUTE1;
};

struct VS_Output
{
    float3 TexCoord     : TEXCOORD0;
    float3 ScreenVector : TEXCOORD1;
    float4 Postion      : SV_Position;
};

void DrawRectangle(in float4 InPosition, in float2 InTexCoord,out float4 OutPosition, out float2 OutTexCoord)
{
    OutPosition = InPosition;
    OutPosition.xy = -1.0f + 2.f * (DrawRectangleParameters.zw + (InPosition.xy * DrawRectangleParameters.xy)) * DrawRectangleParameters.InvTargetSizeAndTextureSize.xy;
    OutPosition.xy *= float2(1,-1);
    OutTexCoord.xy = (DrawRectangleParameters.UVScaleBias.zw + (InTexCoord.xy * DrawRectangleParameters.xy) * DrawRectangleParameters.InvTargetSizeAndTextureSize.xy;
}

VS_Output VS_Main(VS_Input Input) : SV_Position
{
    VS_Output Out;
    DrawRectangle(float4(Input.Position,0,1), Input.UV, Out.Position, Out.TexCoord);
    Out.ScreenVector = mul(float4(OutPosition,1,0),View.ScreenToTranslatedWorld).xyz;
    return Out;
}

GBufferData GetGBufferData(float2 UV, bool bGetNormalizedNormal = true)
{
    float4 GBufferA = GBufferATextureSampler.Sample(GBufferATexture,UV);
	float4 GBufferB = GBufferBTextureSampler.Sample(GBufferBTexture,UV);
	float4 GBufferC = GBufferCTextureSampler.Sample(GBufferCTexture,UV);
	float4 GBufferD = GBufferDTextureSampler.Sample(GBufferDTexture,UV);

	float CustomNativeDepth = CustomDepthTextureSampler.Sample(CustomDepthTexture,UV).r;
    uint CustomStencil = 0;
	float4 GBufferE = GBufferETextureSampler.Sample(GBufferETexture,UV);
	float4 GBufferVelocity = GBufferVelocityTextureSampler.Sample(GBufferVelocityTexture,UV);

    float SceneDepth = CalcSceneDepth(UV);
    bool bChecker = false;

    return DecodeGBufferData(GBufferA,GBufferB,GBufferC,GBufferD,GBufferE,GBufferVelocity,CustomNativeDepth,CustomNativeDepth,SceneDepth,bGetNormalizedNormal,bChecker);
}

struct ScreenSpaceData
{
    GBufferData GBuffer;
    float AmbientOcclusion;
};

ScreenSpaceData GetScreenSpaceData(float2 UV, bool bGetNormalizedNormal = true)
{
    FScreenSpaceData Out;
    Out.GBuffer = GetGBufferData(UV, bGetNormalizedNormal);

    //float4 ScreenSpaceAO = Texture2DSampleLevel(SceneTexturesStruct.ScreenSpaceAOTexture, SceneTexturesStruct.ScreenSpaceAOTextureSampler, UV, 0);

	//Out.AmbientOcclusion = ScreenSpaceAO.r;

    return Out;
}

void PS_Main(VS_Output Input, out float4 OutColor : SV_Target0) 
{
    FScreenSpaceData ScreenSpaceData = GetScreenSpaceData(Input.TexCoord);

    float SceneDepth = CalcSceneDepth(Input.TexCoord);
    float3 WorldPosition = Input.ScreenVector * ScreenDepth + View.WorldCameraOrigin;

    DeferredLightData LightData = SetupLightDataForStandardDeferred();

    //float Dither = InterleavedGradientNoise( SVPos.xy, View.StateFrameIndexMod8 );
    float Dither = 0;

    OutColor = GetDynamicLighting(WorldPosition,CameraVector,ScreenSpaceData.GBuffer,ScreenSpaceData.AmbientOcclusion,ScreenSpaceData.GBuffer.ShadingModelID,LightData, GetPerPixelLightAttenuation(ScreenUV), Dither, uint2( Input.Position.xy ));    
}
