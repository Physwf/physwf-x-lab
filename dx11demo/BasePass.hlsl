#include "DeferredShadingCommon.hlsl"

struct VertexOut
{
    float4 PosH                 : SV_Position;
    float4 Color                : COLOR0;
    float2 TexCoords            : TEXCOORD0;
    float2 LightMapCoordinate   : TEXCOORD1;
};

struct VertexIntermediates
{
    half3x3 TangentToLocal;
	half3x3 TangentToWorld;
	half TangentToWorldSign;

    half4 Color;
};

half3x3 CalcTangentToLocal(VertexFactoryInput Input,out float TangentSign)
{
    half3x3 Result;

    half3 TangentInputX = Input.TangentX;
    half4 TangentInputZ = Input.TangentZ;

    //half3 TangentX = TangentBias(TangentInputX);
	//half4 TangentZ = TangentBias(TangentInputZ);

    half3 TangentX = TangentInputX;
	half4 TangentZ = TangentInputZ;

    TangentSign = TangentZ.w;

    half3 TangentY = cross(TangentZ.xyz,TangentX) * TangentZ.w;

    Result[0] = cross(TangentY,Tangent.xyz) * Tangent.w;
    Result[1] = TangentY;
    Result[2] = TangentZ.xyz;

    return Result;
}

half3x3 CalcTangentToWorldNoScale(in half3x3 TangentToLocal)
{
    return mul(TangentToWorld,LocalToWorld);
}

half3x3 CalcTangentToWorld(VertexIntermediates Intermediates, half3x3 TangentToLocal)
{
    half3x3 TangentToWorld = CalcTangentToWorldNoScale(TangentToLocal);
    return TangentToWorld;
}

VertexIntermediates GetVertexFactoryIntermediates(VertexFactoryInput Input)
{
    VertexIntermediates Intermediates;

    Intermediates.Color = Input.Color;

    float TangentSign;
    Intermediates.TangentToLocal = CalcTangentToLocal(Input,TangentSign);
    Intermediates.TangentToWorld = CalcTangentToWorld(Input,Intermediates.TangentToLocal);

    return Intermediates;
};

struct PS_Output
{
    float4 OutTarget0 : SV_Target0;
    float4 OutTarget1 : SV_Target0;
    float4 OutTarget2 : SV_Target0;
    float4 OutTarget3 : SV_Target0;
    float4 OutTarget4 : SV_Target0;
    float4 OutTarget5 : SV_Target0;
    float4 OutTarget6 : SV_Target0;
    float4 OutTarget7 : SV_Target0;
    uint Coverage : SV_Coverage;
    float Depth : SV_Depth;
}


VertexOut VS_Main(VertexFactoryInput Input)
{
    VertexOut Out;

    VertexIntermediates VFIntermediates = GetVertexFactoryIntermediates(Input);

    float4 WorldPostion = mul(Primitive.LocalToWorld,float4(Input.Position,1.0));
    float4 ViewPostion = mul(View.WorldToView,WorldPostion);
    Out.PosH = mul(View.ViewToProj,ViewPostion);

    Out.Color = Input.Color;
    Out.TexCoords = Input.TexCoords;

    return Out;
}

Texture2D<float4> BaseColor;
Texture2D<float4> Normal;
Texture2D<float> Gloss;
Texture2D<float> Specular;
SamplerState BaseColorSampler;
SamplerState NormalSampler;
SamplerState GlossSampler;
SamplerState SpecularSampler;


PS_Output PS_Main(VertexOut Input)
{
    GBufferData GBuffer = (GBufferData)0;
    GBuffer.WorldNormal = NormalSampler.Sampler(Normal,Input.TexCoords);
    GBuffer.BaseColor = BaseColorSampler.Sample(BaseColor,Input.TexCoords);
    GBuffer.Metallic = 0.5;
    GBuffer.Specular = SpecularSampler.Sample(Specular,Input.TexCoords);
    GBuffer.Roughness = 0.5;
    //GBuffer.GBufferAO = 0.5;
    //GBuffer.PerObjectGBufferData = Primitive.PerObjectGBufferData;
    GBuffer.Depth = Input.PosH.w;
    //GBuffer.PrecomputedShadowFactors = GetPrecomputedShadowMasks(Interpolants, MaterialParameters.AbsoluteWorldPosition, VolumetricLightmapBrickTextureUVs);
    //GBuffer.Velocity = 0;


    GBuffer.DiffuseColor = BaseColor - BaseColor * Metallic;

    PS_Output Out = (PS_Output)0;
    
    float Opacity = 0;
    float3 Color = GBuffer.DiffuseColor + GBuffer.SpecularColor * 0.45;
    Out.OutTarget0 = float4(Color,Opacity);
    Out.OutTarget1.rgb = GBuffer.WorldNormal;
    Out.OutTarget1.a = GBuffer.PerObjectGBufferData;
    Out.OutTarget2.r = GBuffer.Metallic;
    Out.OutTarget2.g = GBuffer.Specular;
    Out.OutTarget2.b = GBuffer.Roughness;
    //Out.OutTarget2.a = EncodeShadingModelIdAndSelectiveOutputMask(GBuffer.ShadingModelID, GBuffer.SelectiveOutputMask);;
    Out.OutTarget3.rgb = float4(Color,GBuffer.GBufferAO);
    Out.OutTarget3.a = GBuffer.GBufferAO;
    Out.OutTarget4 = GBuffer.CustomData;
    Out.OutTarget5 = GBuffer.PrecomputedShadowFactors;
    //Out.OutTarget6 = float4(Color,);
    //Out.OutTarget7 = float4(Color,);
    Out.Depth = 1 / pin.Position.w;
    return Out;
}