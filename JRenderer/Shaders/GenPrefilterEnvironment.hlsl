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

float4 PSMain(GSOutput Input) : SV_Target
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
    return float4(Li/TotalWeight,1.f);
}


