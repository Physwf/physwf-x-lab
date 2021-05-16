#include "Sampling.hlsl"
#include "microfacet.hlsl"

struct VSInput
{
    float3 Position : POSITION;
};

struct VSOutput
{
    float3 Position : POSITION;
};

struct GSOutput
{
    float3 WorldPosition : TEXCOORD0;
    float4 SVPosition : SV_Position;
    uint RenderTargetArrayIndex : SV_RenderTargetArrayIndex;
};

VSOutput VSMain(VSInput Input) 
{
    VSOutput Output = (VSOutput)0;
    Output.Position = Input.Position;
    return Output;
}

cbuffer View 
{
    float4x4 FaceTransform[6];
    float4x4 Projection;
};

[maxvertexcount(18)]
void GSMain(
    triangle VSOutput Input[3],
    inout TriangleStream<GSOutput> TriStream)
{
    [unroll]
    for(int CubeFaceIndex=0;CubeFaceIndex<6;++CubeFaceIndex)
    {
        GSOutput Output = (GSOutput)0;
        Output.RenderTargetArrayIndex = CubeFaceIndex;
        [unroll]
        for(int VertexIndex=0;VertexIndex<3;++VertexIndex)
        {
            Output.WorldPosition = mul(FaceTransform[CubeFaceIndex],float4(Input[VertexIndex].Position,1.0f)).xyz;
            Output.SVPosition = mul(float4(Input[VertexIndex].Position,1.0f),Projection);
            TriStream.Append(Output);
        }
        TriStream.RestartStrip();
    }
     
}
cbuffer cbRoughness:register(b2)
{
    float fRoughness ;
}
TextureCube<float4> EnvironmentMap;
SamplerState EnvironmentMapSampler;

float3 PrefilterEnvironmentMap(float Roughness, float3 R)
{
    float3 N = R;
    float3 V = R;
    const uint SmapleCount = 2048;
    float3 Li = 0;
    float TotalWeight = 0;
    [unrool]
    for(uint i=0;i<SmapleCount;++i)
    {
        float2 Xi = Hammersley(i,SmapleCount);
        float3 H = ImportantSamplingGGX(Xi,Roughness,N);
        float3 L = 2*dot(V,H)*H - V;

        float NoL = saturate(dot(N,L));

        if(NoL > 0)
        {
            Li += EnvironmentMap.SampleLevel(EnvironmentMapSampler,L,0).rgb * NoL;
            TotalWeight += NoL;
        }
    }
    return Li/TotalWeight;
}

float Gussian(float Sigma,float Miu, float X)
{
    float Delta = X - Miu;
    return (1.0f / (Sigma * sqrt(2.0f * PI))) * exp(-(Delta*Delta)/(2.f * Sigma * Sigma));
}

float3 PrefilterEnvironmentMap_Gussian(float Roughness, float3 R)
{
    const uint SmapleCount = 1024;
    float3 Li = 0;
    float TotalWeight = 0;
    float ThetaMax = PI / 2.0f * Roughness;
    float CosThetaMax = cos(ThetaMax);
    [unrool]
    for(uint i=0;i<SmapleCount;++i)
    {
        float2 Xi = Hammersley(i,SmapleCount);
        float3 H = UniformSampleCone(Xi,CosThetaMax);
        float3x3 TangentSpace;

        TangentSpace[2] = R;
        CoordinateSystem(TangentSpace[2],TangentSpace[0],TangentSpace[1]);
        float3 L = mul(H,TangentSpace);
        float Weight = Gussian(0.5f,0.5f,Xi.x);
        
        Li += EnvironmentMap.SampleLevel(EnvironmentMapSampler,L,0).rgb * Weight;
        TotalWeight += Weight;
    }
    return Li/TotalWeight;
}

float4 PSMain(GSOutput Input) : SV_Target
{
    float3 WorldPosition = normalize(Input.WorldPosition);
    float3 N = normalize(WorldPosition);
    float3 PrefilteredColor = PrefilterEnvironmentMap_Gussian(fRoughness,N);
    return float4(PrefilteredColor,1.f);
}


