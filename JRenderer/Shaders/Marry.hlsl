cbuffer Primitive
{
    float3x3 LocalToWorld;
    float3 Translation;
}

cbuffer View
{
    float4x4 WorldToClip;
    float4x4 SVPositionToWorld;
    float3 ViewPosition;
}

struct VSInput
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float2 UV : TEXCOORD;
};

struct VSOutput
{
    float4 SVPosition : SV_Position;
    float3 WorldNormal : TEXCOORD;
    float2 UV : TEXCOORD;
};

VSOutput VSMain(VSInput Input)
{
    VSOutput Output = (VSOutput)0;
    float3 WorldPosition = mul(Input.Position,LocalToWorld);
    Output.SVPosition = mul(float4(WorldPosition,1.0f),WorldToClip);
    Output.WorldNormal = mul(Input.Normal,LocalToWorld);
    Output.UV = Input.UV;

    return Output;
}

cbuffer Light
{
    float4 LightPositionAndRadius;
    float4 LightOrientationAndNeerPlane;
    float2 LightmapViewport;
    float3 AmbientIntencity;
    float3 Intencity;
};

cbuffer Material
{
    float3 ka;
	float3 kd;
	float3 ks;
	float Ns;//高光指数
	float Ni;//折射率
	float ShadingModel;
};

Texture2D<float> ShadowDepthMap;
SamplerState ShadowDepthMapSampler;
Texture2D<float> DiffuseMap;
SamplerState DiffuseMapSampler;

float3 SVPositionToWorldPosition(float4 SVPosition)
{
    float4 HomoPosition = mul(SVPositionToWorld,SVPosition);
    return HomoPosition.xyz / HomoPosition.w;
}

void PSMain(VSOutput Input,out float4 OutColor)
{
    float3 WorldPosition = SVPositionToWorldPosition(Input.SVPosition);
    float3x3 WorldToLightView;
    float3 LightViewPosition = mul(WorldToLightView,WorldPosition);
    float4x4 LightViewToClip;
    float4 HomoPosition = mul(LightViewToClip,float4(LightViewPosition,1.0f));
    float2 LightMapUV = saturate(HomoPosition.xy);
    //
    float3 WorldToLightDir = WorldPosition - LightPositionAndRadius.xyz;
    float WorldToLightDist = dot(WorldToLightDir,LightOrientationAndNeerPlane.xyz);
    float WorldToLightNearPlane = WorldToLightDist - LightOrientationAndNeerPlane.w;
    float SampleRadius = (WorldToLightNearPlane / WorldToLightDist) * LightPositionAndRadius.w;
    uint2 SampleCount = (uint2)(LightmapViewport * SampleRadius);

    uint LightPassCount = 0;
    for(uint u = - SampleCount.x; u <= SampleCount.x;++u)
    {
        for(uint v = - SampleCount.y; v <= SampleCount.y;++u)
        {
            float ShadowDepth = ShadowDepthMap.Sample(ShadowDepthMapSampler,LightMapUV + float2(u,v));
            if(ShadowDepth > HomoPosition.z)
            {
                LightPassCount++;
            }
        }
    }
    float fLightPercent = (float)LightPassCount / dot(SampleCount.xy,float2(1.0f,1.0f));
    
    float3 L = normalize(LightPositionAndRadius.xyz - WorldPosition);
    float3 N = normalize(Input.WorldNormal);
    float3 V = normalize(ViewPosition - WorldPosition);
    float3 H = normalize((L + V) / 2.f);

    float3 BaseColor = DiffuseMap.Sample(DiffuseMapSampler,Input.UV);

    float3 AmibientBaseColor = mul(BaseColor,Ka);
    float3 DiffuseBaseColor = mul(BaseColor,Kd);

    float3 AmbientColor = mul(AmibientBaseColor,AmbientIntencity);
    float LightIntencity = Intencity * fLightPercent; 

    float3 DiffuseColor = mul(LightIntencity, DiffuseBaseColor) * max(dot(L,N),0);
    //float3 SpecularColor = LightIntencity * pow(dot(H,V),Ns);

    OutColor = AmbientColor + DiffuseColor;
}