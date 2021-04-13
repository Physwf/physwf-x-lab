

#define GROUP_SIZE 256

Texture2D<float4> Input;
RWBuffer<uint> Histogram;

cbuffer HistogramCB
{
    uint InputWidth;
    uint InputHeight;
    float MinLogL;
    float LogRange;
    float InverseLogRange;
    float DeltaTime;
    float tau;
};

uint rbg2bin(float3 rgb)
{
    const float3 lum_multiplyer = float3(0.2126f,0.7152f,0.0722f);
    float L = dot(lum_multiplyer,rgb);
    if(L < 0.005f) return 0;

    float LogL = (log2(L) - MinLogL) * InverseLogRange;
    LogL = saturate(LogL);
    return (uint)(LogL * 254.f + 1.0f);
}

groupshared uint LocalSharedHistogram[GROUP_SIZE];

[numthreads(16,16,1)]
void HistogramCSMain(uint GroupIndex : SV_GroupIndex, uint3 ThreadID : SV_DispatchThreadID)
{
    LocalSharedHistogram[GroupIndex] = 0;

    GroupMemoryBarrierWithGroupSync();
    if(ThreadID.x < InputWidth && ThreadID.y < InputHeight)
    {
        float3 LinearColor = Input.Load(int3(ThreadID.xy,0)).rgb;
        uint BinIndex = rbg2bin(LinearColor);
        InterlockedAdd(LocalSharedHistogram[BinIndex],1);
    }

    GroupMemoryBarrierWithGroupSync();

    InterlockedAdd(Histogram[GroupIndex],LocalSharedHistogram[GroupIndex]);
}

Buffer<uint> HistogramROnly;
RWBuffer<float> LuminanceAverage;

[numthreads(1,1,1)]
void CSMain(uint GroupIndex : SV_GroupIndex)
{
    uint CountForThisBin = HistogramROnly[GroupIndex] * GroupIndex;
    LocalSharedHistogram[GroupIndex] = CountForThisBin;

    GroupMemoryBarrierWithGroupSync();

    for(uint i = GROUP_SIZE >> 1; i >0 ;i >>= 1)
    {
        if(GroupIndex < i)
        {
            InterlockedAdd(LocalSharedHistogram[GroupIndex], LocalSharedHistogram[GroupIndex + i]);
        }
        GroupMemoryBarrierWithGroupSync();
    }


    if(GroupIndex == 0)
    {
        uint BlackPixelCount = HistogramROnly[0];
        float WightedLogAverage = LocalSharedHistogram[0] / (float) max(InputWidth*InputHeight - BlackPixelCount,1) -1.0f;
        float WightedAverageL = exp2(((WightedLogAverage / 254.0f) * LogRange) + MinLogL);
        LuminanceAverage[0] = WightedAverageL;
    }
}


float3 RGB2XYZ(float3 RGB)
{
    float3 XYZ;
    XYZ.x = dot(RGB,float3(0.4124564f , 0.3575761f, 0.1804375f));
    XYZ.y = dot(RGB,float3(0.2126729f , 0.7151522f, 0.0721750f));
    XYZ.z = dot(RGB,float3(0.0193339f , 0.1191920f, 0.9503041f));
    return XYZ;
}

float3 XYZ2RGB(float3 XYZ)
{
    float3 RGB;
    RGB.x = dot(XYZ,float3(3.2404542f, -1.5371385f, -0.4985314f));
    RGB.y = dot(XYZ,float3(-0.9692660f, 1.8760108f, 0.0415560f));
    RGB.z = dot(XYZ,float3(0.0556434f, -0.2040259f, 1.0572252f));
    return RGB;
}

float3 XYZ2Yxy(float3 XYZ)
{
    float inv = 1.0f / (XYZ.x + XYZ.y + XYZ.z);
    float3 Yxy;
    Yxy.x = XYZ.y;
    Yxy.y = XYZ.x * inv;
    Yxy.z = XYZ.y * inv;
    return Yxy;
}

float3 Yxy2XYZ(float3 Yxy)
{
    float3 XYZ;
    XYZ.x = Yxy.x * Yxy.y / Yxy.z;
    XYZ.y = Yxy.x;
    XYZ.z = Yxy.x * (1.0f - Yxy.y - Yxy.z) / Yxy.z;
    return XYZ;
}

float3 RGB2Yxy(float3 RGB)
{
    return XYZ2Yxy(RGB2XYZ(RGB));
}

float3 Yxy2RGB(float3 Yxy)
{
    return XYZ2RGB(Yxy2XYZ(Yxy));
}

float3 Linear2Gamma(float3 LinearColor)
{
    float3 Gamma = pow(LinearColor,1.f / 2.2f);
    return Gamma;
}

float Reinhard(float x)
{
    return x / (1.0f + x);
}

float Reinhard2(float x, float L_white)
{
    return (x * (1.0f + x / (L_white * L_white))) / (1.0f + x);
}

inline float Tonemap_ACES(float x) {
	// Narkowicz 2015, "ACES Filmic Tone Mapping Curve"
	const float a = 2.51f;
	const float b = 0.03f;
	const float c = 2.43f;
	const float d = 0.59f;
	const float e = 0.14f;
	return (x * (a * x + b)) / (x * (c * x + d) + e);
}

float Tonemap_Unreal(float x) {
	// Unreal 3, Documentation: "Color Grading"
	// Adapted to be close to Tonemap_ACES, with similar range
	// Gamma 2.2 correction is baked in, don't use with sRGB conversion!
	return x / (x + 0.155f) * 1.019f;
}

float4 Tonemapping_Reinhard(float3 LinearColor,float Lavg)
{
    float3 Yxy = RGB2Yxy(LinearColor);
    float Lp = Yxy.x / (9.6f * Lavg + 0.0001f);
    Yxy.x = Reinhard(Lp);
    float3 TonemapColor = Yxy2RGB(Yxy);
    return float4(Linear2Gamma(TonemapColor),1.0f);
}

cbuffer TonemapCB
{
    float L_white;
};

float4 Tonemapping_Reinhard2(float3 LinearColor,float Lavg)
{
    float3 Yxy = RGB2Yxy(LinearColor);
    float Lp = Yxy.x / (9.6f * Lavg + 0.0001f);
    Yxy.x = Reinhard2(Lp,L_white);
    float3 TonemapColor = Yxy2RGB(Yxy);
    return float4(Linear2Gamma(TonemapColor),1.0f);
}

float4 Tonemapping_Unreal(float3 LinearColor,float Lavg)
{
    float3 Yxy = RGB2Yxy(LinearColor);
    float Lp = Yxy.x / (9.6f * Lavg + 0.0001f);
    Yxy.x = Tonemap_Unreal(Lp);
    float3 TonemapColor = Yxy2RGB(Yxy);
    return float4(TonemapColor,1.0f);
}

void VSMain(float2 Position:POSITION,out float4 SVPosition:SV_Position,out float2 UV : TEXCOORD)
{
    SVPosition = float4(Position,0,1);
    UV = Position - float2(-1.f,-1.f)/2.f;
}

Buffer<float> LuminanceAverageROnly;
SamplerState PointSampler;

void PSMain(float4 SVPosition:SV_Position,float2 UV : TEXCOORD,out float4 Color : SV_Target)
{
    float3 LinearColor = Input.SampleLevel(PointSampler,UV,0).rgb;
    float Lavg = LuminanceAverageROnly[0];
    Color = Tonemapping_Reinhard(LinearColor,Lavg);
}