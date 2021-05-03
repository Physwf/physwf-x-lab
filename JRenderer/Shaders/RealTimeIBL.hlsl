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
    float4 BaseColor;
    float4 SpecularColor;
    float  fRoughness;
    float4 padding05[13];
}

TextureCube EnvironmentMap;
SamplerState EnvironmentMapSampler;

float3 DiffuseIBL(float3 V, float3 N)
{
    float3 DiffuseLight = 0;
    const uint SmapleCount = 1024;
    [unrool]
    for(uint i=0;i<SmapleCount;++i)
    {
        float2 Xi = Hammersley(i,SmapleCount);
        float3 L = UniformSamplingHalfSphere(Xi,N);
        float3 Li = EnvironmentMap.SampleLevel(EnvironmentMapSampler,L,0).rgb;
        float NoL = saturate(dot(N,L));
        DiffuseLight += NoL* Li;
    }
    return (DiffuseLight / SmapleCount) * BaseColor.rgb / PI;
}

float3 SpecularIBL(float3 V, float3 N)
{
    float3 SpecularLight = 0;
    const uint SmapleCount = 1024;
    [unrool]
    for(uint i=0;i<SmapleCount;++i)
    {
        float2 Xi = Hammersley(i,SmapleCount);
        float3 H = ImportantSamplingGGX(Xi,fRoughness,N);
        float3 L = 2*dot(V,H)*H - V;

        float NoH = saturate(dot(N,H));
        float NoV = saturate(dot(N,V));
        float VoH = saturate(dot(V,H));
        float NoL = saturate(dot(N,L));

        if(NoL > 0)
        {
            float3 Li = EnvironmentMap.SampleLevel(EnvironmentMapSampler,L,0).rgb;

            float F0 = SpecularColor;
            float F = F0 + (1-F0) * pow((1-VoH),5);

            float G = G_Smith(fRoughness,NoV,NoH);
            SpecularLight += Li * F * G * VoH / (NoH*NoV);
        }
    }

    return SpecularLight / SmapleCount;
}

float4 PSMain(VSOutput Input) :SV_Target
{
    float3 V = normalize(ViewOrigin.xyz - Input.WorldPosition);
    float3 N = normalize(Input.Normal);
    float3 Color = DiffuseIBL(V,N) + SpecularIBL(V,N);
    return float4(Color,1.0);
}