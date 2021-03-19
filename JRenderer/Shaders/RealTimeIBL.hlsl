#define PI 3.14

struct VSInput
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
};

struct VSOutput
{
    float4 SvPosition : SV_Position;

    float3 Position : POSITION;
    float3 Normal : NORMAL;
};

VSOutput VSMain(VSInput Input) 
{
    VSOutput Output = (VSOutput)0;
    Output.Position = Input.Position;
    return Output;
}

cbuffer View
{
    float3 ViewOrigin;
}

cbuffer Material
{
    float3 BaseColor;
    float3 SpecularColor;
    float  fRoughness;
}

TextureCube EnvironmentMap;
SamplerState EnvironmentMapSampler;

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

float G_Smith(float Roughness,float NoV,float NoL)
{
    float k = (Roughness+1)*(Roughness+1) / 8.f;
    float G1V = NoV / (NoV *(1-k) + k);
    float G1L = NoL / (NoL *(1-k) + k);
    return G1V * G1V;
}

float3 DiffuseIBL(float3 V, float3 N)
{
    float3 DiffuseLight = 0;
    const uint SmapleCount = 1024;
    for(uint i=0;i<SmapleCount;++i)
    {
        float2 Xi = Hammersley(i,SmapleCount);
        float3 L = UniformSamplingHalfSphere(Xi,N);
        float3 Li = EnvironmentMap.SampleLevel(EnvironmentMapSampler,L,0).rgb;
        float NoL = saturate(dot(N,L));
        DiffuseLight += NoL* Li;
    }
    return (DiffuseLight / SmapleCount) * BaseColor / PI;
}

float3 SpecularIBL(float3 V, float3 N)
{
    float3 SpecularLight = 0;
    const uint SmapleCount = 1024;
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
    float3 V = normalize(ViewOrigin - Input.Position);
    float3 N = normalize(Input.Normal);
    float3 Color = DiffuseIBL(V,N) + SpecularIBL(V,N);
    return float4(Color,1.0);
}