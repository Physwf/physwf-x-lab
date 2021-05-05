#include "Sampling.hlsl"
#include "microfacet.hlsl"

struct VSInput
{
    float3 Position : POSITION;
};

struct VSOutput
{
    float3 WorldPosition : TEXCOORD0;
    float4 SVPosition : SV_Position;
};

cbuffer View
{
    float4x4 WorldToProj;
};

VSOutput VSMain(VSInput Input)
{
    VSOutput Output = (VSOutput)0;
    Output.WorldPosition = Input.Position;
    Output.SVPosition = mul(WorldToProj,float4(Input.Position,1));
    return Output;
}

float fRoughness;
TextureCube EnvironmentMap;
SamplerState EnvironmentMapSampler;

float4 PSMain(VSOutput Input) : SV_Target
{
    float3 WorldPosition = normalize(Input.WorldPosition);
    float3 N = normalize(WorldPosition);
    float3 V = N;
    const uint SmapleCount = 1024;
    float3 Li = 0;
    float TotalWeight = 0;
    [unrool]
    for(uint i=0;i<SmapleCount;++i)
    {
        float2 Xi = Hammersley(i,SmapleCount);
        float3 H = ImportantSamplingGGX(Xi,fRoughness,N);
        float3 L = 2*dot(V,H)*H - V;

        float NoL = saturate(dot(N,L));

        if(NoL > 0)
        {
            Li += EnvironmentMap.SampleLevel(EnvironmentMapSampler,L,0).rgb * NoL;
            TotalWeight += NoL;
        }
    }
    return Li / TotalWeight;
}


