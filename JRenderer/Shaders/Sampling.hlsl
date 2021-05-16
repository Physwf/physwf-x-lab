#include "Common.hlsl"

void CoordinateSystem(const float3 InZ,out float3 OutX, out float3 OutY )
{
    if(InZ.x == 0.f && InZ.y == 0.f)
    {
        OutX = float3(1.f,0.f,0.f);
    }
    else if(abs(InZ.x) > abs(InZ.y))
    {
        OutX = float3(-InZ.z, 0, InZ.x) / sqrt(InZ.x*InZ.x + InZ.z * InZ.z);
    }
    else 
    {
        OutX = float3( 0,InZ.z, -InZ.y) / sqrt(InZ.y*InZ.y + InZ.z * InZ.z);
    }
    OutY = cross(InZ,OutX);
}

float3 UniformSamplingHalfSphere(float2 Xi, float3 N)
{
    float Phi = 2 * PI * Xi.x;
    float Theta = 0.5 * PI  * Xi.y;

    float3 L;
    L.x = sin(Theta)*cos(Phi);
    L.y = sin(Theta)*sin(Phi);
    L.z = cos(Theta);

    float3 UpVector = (abs(N.z) < 0.999f) ? float3(0,0,1) : float3(1,0,0);
    float3 TangentX = normalize(cross(UpVector,N));
    float3 TangentY = cross(N,TangentX);

    return L.x * TangentX + L.y * TangentY + L.z * N;
}

float3 UniformSampleCone(float2 Xi,float CosThetaMax)
{
    float CosTheta = (1.0f - Xi.x) + Xi.x * CosThetaMax;
    float SinTheta = sqrt(1.f - CosTheta * CosTheta);
    float Phi = Xi.y * 2 * PI;
    return float3(cos(Phi)*SinTheta,sin(Phi) * SinTheta,CosTheta);
}

float RadicalInverse2(uint n)
{
    n = (n << 16) | (n >> 16);
    n = ((n & 0x00ff00ff) << 8) | ((n & 0xff00ff00) >> 8);
    n = ((n & 0x0f0f0f0f) << 4) | ((n & 0xf0f0f0f0) >> 4);
    n = ((n & 0x33333333) << 2) | ((n & 0xcccccccc) >> 2);
    n = ((n & 0x55555555) << 1) | ((n & 0xaaaaaaaa) >> 1);
    //return n* 0x1p-64;
    return float(n) * 2.3283064365386963e-10f;
}

float2 Hammersley(int i,uint N)
{
    float2 Xi;
    Xi.x = float(i)/N;
    Xi.y = RadicalInverse2(i);
    return Xi;
}