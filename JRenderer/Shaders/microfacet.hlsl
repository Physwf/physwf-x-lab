#include "Common.hlsl"

float3 ImportantSamplingGGX(float2 Xi,float Roughness,float3 N)
{
    float a = Roughness*Roughness;

    float Phi = 2 * PI * Xi.x;
    float CosTheta = sqrt((1-Xi.y)/((a*a - 1) * Xi.y +1));
    float SinTheta = sqrt(1-CosTheta * CosTheta);

    float3 H;
    H.x = SinTheta * cos(Phi);
    H.y = SinTheta * sin(Phi);
    H.z = CosTheta;

    float3 UpVector = (abs(N.z) < 0.999f) ? float3(0,0,1) : float3(1,0,0);
    float3 TangentX = normalize(cross(UpVector,N));
    float3 TangentY = cross(N,TangentX);

    return H.x * TangentX + H.y * TangentY + H.z * N;
}

float G_Smith(float Roughness,float NoV,float NoL)
{
    float k = (Roughness+1)*(Roughness+1) / 8.f;
    float G1V = NoV / (NoV *(1-k) + k);
    float G1L = NoL / (NoL *(1-k) + k);
    return G1V * G1V;
}