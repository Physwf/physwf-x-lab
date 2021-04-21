cbuffer Primitive : register(b0)
{
    float4x4 LocalToWorld;
    float3 Translation;
};

cbuffer View : register(b1)
{
    float4 ViewOrigin;
	float4x4 WorldToClip;
	float4x4 ClipToWorld;
	float4x4 SvPositionToWorld;
	float4 ViewSizeAndInvSize;
	float4 ViewRectMin;
};

cbuffer Material : register(b2)
{
    float3 Ka;
	float3 Kd;
	float3 Ks;
	float Ns;//高光指数
	float Ni;//折射率
	float ShadingModel;
};

cbuffer Light : register(b3)
{
    float4 LightPositionAndRadius;
    float4 LightPerspectiveMatrix;//aspect,tan(alpha/2),Zn,Zf
    float3 LightOrientation;
    float3 Intencity;
    float3 AmbientIntencity;
    float2 LightmapViewport;
};

struct VSInput
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float4 Tangent :TANGENT;
    float2 UV : TEXCOORD;
};

struct VSOutput
{
    float4 SVPosition : SV_Position;
    float3 WorldNormal : TEXCOORD0;
    float2 UV : TEXCOORD1;
};

VSOutput VSMain(VSInput Input)
{
    VSOutput Output = (VSOutput)0;
    float4 WorldPosition = mul(float4(Input.Position,1.f),LocalToWorld);
    Output.SVPosition = mul(WorldPosition,WorldToClip);
    Output.WorldNormal = mul(float4(Input.Normal,1.f),LocalToWorld).xyz;
    Output.UV = Input.UV;

    return Output;
}

Texture2D<float> ShadowDepthMap;
SamplerState ShadowDepthMapSampler;
Texture2D<float> DiffuseMap;
SamplerState DiffuseMapSampler;

float3 SVPositionToWorldPosition(float4 SVPosition)
{
    float4 HomoPosition = mul(SvPositionToWorld,SVPosition);
    return HomoPosition.xyz / HomoPosition.w;
}

void PSMain(VSOutput Input,out float4 OutColor:SV_Target)
{
    float3 WorldPositionPreView = SVPositionToWorldPosition(Input.SVPosition);
    float3 WorldPosition = WorldPositionPreView - LightPositionAndRadius.xyz; 

    float3x3 WorldToLightView;
    WorldToLightView[2] = normalize(LightOrientation);
    WorldToLightView[0] = cross(float3(0.f,1.0f,0.f), WorldToLightView[2]);
    WorldToLightView[1] = cross(WorldToLightView[2], WorldToLightView[0]);
    float3 LightViewPosition = mul(WorldToLightView,WorldPosition);
    float4x4 LightViewToClip = {
        {1.f/(LightPerspectiveMatrix.x*LightPerspectiveMatrix.y),0,0,0},
        {0,1.f/LightPerspectiveMatrix.y,0,0},
        {0,0,(-LightPerspectiveMatrix.z-LightPerspectiveMatrix.w)/(LightPerspectiveMatrix.z-LightPerspectiveMatrix.w),2.f*LightPerspectiveMatrix.z*LightPerspectiveMatrix.w/(LightPerspectiveMatrix.z-LightPerspectiveMatrix.w)},
        {0,0,1,0},
        };
    float4 HomoPosition = mul(LightViewToClip,float4(LightViewPosition,1.0f));
    float2 LightMapUV = saturate(HomoPosition.xy);
    //
    float3 WorldToLightDir = WorldPosition - LightPositionAndRadius.xyz;
    float WorldToLightDist = dot(WorldToLightDir,LightOrientation);
    float WorldToLightNearPlane = WorldToLightDist - LightPerspectiveMatrix.z;
    float SampleRadius = (WorldToLightNearPlane / WorldToLightDist) * LightPositionAndRadius.w;
    int2 SampleCount = (int2)(LightmapViewport * SampleRadius);

    uint LightPassCount = 0;
    for(int u = - SampleCount.x; u <= SampleCount.x;++u)
    {
        for(int v = - SampleCount.y; v <= SampleCount.y;++v)
        {
            float ShadowDepth = ShadowDepthMap.SampleLevel(ShadowDepthMapSampler,LightMapUV + float2(u,v),0);
            if(ShadowDepth > HomoPosition.z)
            {
                LightPassCount++;
            }
        }
    }
    float fLightPercent = (float)LightPassCount / dot(SampleCount.xy,float2(1.0f,1.0f));
    
    float3 L = normalize(LightPositionAndRadius.xyz - WorldPosition);
    float3 N = normalize(Input.WorldNormal);
    float3 V = normalize(ViewOrigin.xyz - WorldPosition);
    float3 H = normalize((L + V) / 2.f);

    float3 BaseColor = DiffuseMap.Sample(DiffuseMapSampler,Input.UV);

    float3 AmibientBaseColor = mul(BaseColor,Ka);
    float3 DiffuseBaseColor = mul(BaseColor,Kd);

    float3 AmbientColor = mul(AmibientBaseColor,AmbientIntencity);
    float3 LightIntencity = Intencity.xyz * fLightPercent; 

    float3 DiffuseColor = mul(LightIntencity, DiffuseBaseColor) * max(dot(L,N),0);
    //float3 SpecularColor = LightIntencity * pow(dot(H,V),Ns);

    OutColor = float4(AmbientColor + DiffuseColor,1.f);
}