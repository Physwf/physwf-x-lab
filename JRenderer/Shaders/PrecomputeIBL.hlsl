#include "Sampling.hlsl"
#include "microfacet.hlsl"

struct VSInput
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
};

struct VSOutput
{
    float4 SvPosition : SV_Position;
    float3 WorldPosition : POSITION;
    float3 Normal : NORMAL;
};

cbuffer Primitive : register(b0)
{
    float4x4 LocalToWorld;
    float LocalToWorldDeterminantSign;
    float4 padding[11];
};

cbuffer View  : register(b1)
{
    float4 ViewOrigin;
    float4x4 WorldToClip;
	float4x4 ClipToWorld;
	float4x4 SvPositionToWorld;
	float4 ViewSizeAndInvSize;
	float4 ViewRectMin;
    float4 padding3;
};

VSOutput VSMain(VSInput Input) 
{
    VSOutput Output = (VSOutput)0;
    Output.WorldPosition = mul(LocalToWorld,float4(Input.Position,1.f));
    Output.SvPosition = mul(WorldToClip,float4(Output.WorldPosition,1.f));
    Output.Normal = Input.Normal;
    return Output;
}

cbuffer Material : register(b2)
{
    float4 Albeto;
    float4 BaseColor;
    float  fRoughness;
    float4 padding05[13];
}

TextureCube PrefilterEnvironmentMap;
SamplerState PrefilterEnvironmentSampler;
Texture2D<float2> IntegratedBRDFMap;
SamplerState IntegratedBRDFMapSampler;

float3 PrefilterEnvMap(float fRoughness,float3 R)
{
    float3 Result = PrefilterEnvironmentMap.SampleLevel(PrefilterEnvironmentSampler,R,fRoughness);
    return Result;
}

float2 IntegratedIBL(float Roughness,float NoV)
{
    return IntegratedBRDFMap.SampleLevel(IntegratedBRDFMapSampler,float2(Roughness,NoV),0);
}

float4 PSMain(VSOutput Input) :SV_Target
{
    float3 V = normalize(ViewOrigin.xyz - Input.WorldPosition);
    float3 N = normalize(Input.Normal);
    float NoV = saturate(dot(V,N));
    float3 R = 2 * dot(V,N) * N - V;
    float3 PrefilterdLi = PrefilterEnvMap(fRoughness,R);
    float2 IntegratedBRDF = IntegratedIBL(fRoughness,NoV);

    float3 Color = PrefilterdLi * (BaseColor.xyz*IntegratedBRDF.x + IntegratedBRDF.y);
    return float4(Color,1.f);
}