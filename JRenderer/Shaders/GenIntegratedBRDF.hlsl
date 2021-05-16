#include "microfacet.hlsl"
#include "Sampling.hlsl"

struct VSInput
{
    float2 Position : POSITION;
};

struct VSOutput
{
    float4 SVPosition : SV_Position;
    float2 UV : TEXCOORD;
};

VSOutput VSMain(VSInput Input)
{
    VSOutput Output = (VSOutput)0;
    //Output.UV = float2(Input.Position.x + 1.0f / 2.0f,  -Input.Position.y + 1.0f / 2.0f);
    Output.UV = (Input.Position + float2(1.f,1.f))/2.f;
    Output.UV.y = 1.f - Output.UV.y;
    Output.SVPosition = float4(Input.Position,0,1.0f);
    return Output;
}

float2 IntegrateBRDF(float Roughness,float NoV)
{
    float3 V;
    V.x = sqrt(1.0f - NoV * NoV);
    V.y = 0;
    V.z = NoV;

    float3 N = float3(0,0,1.f);

    float A = 0;
    float B = 0;

    const int SampleCount = 1024;
    for(int i=0;i<SampleCount;++i)
    {
        float2 Xi = Hammersley(i,SampleCount);
        float3 H = ImportantSamplingGGX(Xi,Roughness,N);
        float3 L = 2*dot(V,H)*H - V;

        float NoL = saturate(L.z);
        float NoH = saturate(H.z);
        float VoH = saturate(dot(V,H));

        if(NoL > 0)
        {
            float G = G_Smith(Roughness,NoV,NoL);

            float G_Vis = G * VoH / (NoH * NoV);
            float Fc = pow(1-VoH,5);
            A += (1-Fc) * G_Vis;
            B += Fc * G_Vis;
        }
    }
    return float2(A,B)/SampleCount;
}

float2 PSMain(VSOutput Input) : SV_Target
{
    return IntegrateBRDF(Input.UV.x,Input.UV.y);
}