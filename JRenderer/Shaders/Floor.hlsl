cbuffer Primitive : register(b0)
{
    float4x4 LocalToWorld;
    float3 Translation;
}

cbuffer View : register(b1)
{
    float4 ViewOrigin;
	float4x4 WorldToClip;
	float4x4 ClipToWorld;
	float4x4 SvPositionToWorld;
	float4 ViewSizeAndInvSize;
	float4 ViewRectMin;
}

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
    float Padding00;
    float3 Intencity;
    float Padding01;
    float3 AmbientIntencity;
    float Padding02;
    float2 LightmapViewport;
    float2 Padding03;
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
    float4 SVPosition : SV_POSITION;
    float3 WorldNormal : TEXCOORD0;
    float2 UV : TEXCOORD1;
};

VSOutput VSMain(VSInput Input)
{
    VSOutput Output = (VSOutput)0;
    float4 WorldPosition = mul(LocalToWorld,float4(Input.Position,1.f));
    Output.SVPosition = mul(WorldToClip,WorldPosition);
    Output.WorldNormal = mul(LocalToWorld,float4(Input.Normal,1.f)).xyz;
    Output.UV = Input.UV;

    return Output;
}

Texture2D<float> ShadowDepthMap;
SamplerState ShadowDepthMapSampler;

float3 SVPositionToWorldPosition(float4 SVPosition)
{
    float4 HomoPosition = mul(SvPositionToWorld,float4(SVPosition.xyz,1));
    return HomoPosition.xyz / HomoPosition.w;
}

float3x3 XMMatrixLookToLH(float3 EyePosition,  float3 EyeDirection,  float3 UpDirection)
{
    float3 R2 = normalize(EyeDirection);

    float3 R0 = cross(UpDirection, R2);
    R0 = normalize(R0);

    float3 R1 = cross(R2, R0);

    float3 NegEyePosition = -(EyePosition);

    float3 D0 = dot(R0, NegEyePosition);
    float3 D1 = dot(R1, NegEyePosition);
    float3 D2 = dot(R2, NegEyePosition);

    float3x3 M;
    M[0] = float3(R0.x,R0.y,R0.z);
    M[1] = float3(R1.x,R1.y,R1.z);
    M[2] = float3(R2.x,R2.y,R2.z);

    M = transpose(M);
    return M;
}
float3x3 XMMatrixLookAtLH(float3 EyePosition, float3 FocusPosition, float3 UpDirection)
{
    float3 EyeDirection = FocusPosition - EyePosition;
    return XMMatrixLookToLH(EyePosition, EyeDirection, UpDirection);
}

void PSMain(VSOutput Input,out float4 OutColor:SV_Target)
{
    float3 WorldPosition = SVPositionToWorldPosition(Input.SVPosition);

    float3 PreLightView = WorldPosition - LightPositionAndRadius.xyz;
    float3x3 WorldToLightView = XMMatrixLookToLH(LightPositionAndRadius.xyz,LightOrientation,float3(1.f,0.0f,0.f));
    //WorldToLightView[2] = normalize(LightOrientation);
    //WorldToLightView[0] = normalize(cross(float3(0.f,1.0f,0.f),WorldToLightView[2]));
   // WorldToLightView[1] = normalize(cross(WorldToLightView[2],WorldToLightView[0]));
    float3 LightViewPosition = mul(PreLightView,WorldToLightView);

    float Aspect=LightPerspectiveMatrix.x;
    float TanHalfAlpha = LightPerspectiveMatrix.y;
    float ZNear = LightPerspectiveMatrix.z;
    float ZFar = LightPerspectiveMatrix.w;

    float4x4 LightViewToClip = (float4x4)0;
    LightViewToClip[0] = float4(1.f/(Aspect*TanHalfAlpha),0,0,0);
    LightViewToClip[1] = float4(0,1.f/TanHalfAlpha,0,0);
    LightViewToClip[2] = float4(0,0,(-ZFar-ZNear)/(ZNear-ZFar),1);
    LightViewToClip[3] = float4(0,0,ZNear*ZFar/(ZNear-ZFar),0);

    //float Height = 1.f/ TanHalfAlpha;
    //float Width = Height / Aspect;
    //float fRange = ZFar / (ZFar - ZNear);

    //LightViewToClip[0][0] = Width;
    //LightViewToClip[1][1] = Height;
    //LightViewToClip[2] = float4(0,0,fRange,1.0f);
    //LightViewToClip[3] = float4(0,0,-fRange * LightPerspectiveMatrix.z,0);
    float4 HomoPosition = mul(float4(LightViewPosition,1.0f),LightViewToClip);

    //float4 HomoPosition = mul(LightViewToClip,float4(LightViewPosition,1.0f));
    HomoPosition.xyz /= HomoPosition.w;
    float2 LightMapUV = (HomoPosition.xy*float2(1.0f,-1.0f))+float2(1.0f,1.0f);
    LightMapUV = LightMapUV/2.f;
    //LightMapUV.y = 1.f - LightMapUV.y;
    //
    float3 WorldToLightDir = WorldPosition - LightPositionAndRadius.xyz;
    float WorldToLightDist = dot(WorldToLightDir,LightOrientation);
    float WorldToLightNearPlane = WorldToLightDist - LightPerspectiveMatrix.z;
    float SampleRadius = (WorldToLightNearPlane / WorldToLightDist) * LightPositionAndRadius.w;
    int2 SampleCount = (int2)(LightmapViewport * SampleRadius);
    SampleCount = int2(7,7);
    uint ShadowCount = 0;
    for(int u = - SampleCount.x; u <= SampleCount.x;++u)
    {
        for(int v = - SampleCount.y; v <= SampleCount.y;++v)
        {
            
            float ShadowDepth = ShadowDepthMap.SampleLevel(ShadowDepthMapSampler,LightMapUV + float2(u/LightmapViewport.x,v/LightmapViewport.y),0);
            if(ShadowDepth < HomoPosition.z + 0.022f)
            {
                ShadowCount++;
            }
            
        }
    }
    float fLightPercent = 1 - (float)ShadowCount /( (2.f*SampleCount.x + 1.f)*(2.f*SampleCount.y + 1.f));
    
    // fLightPercent = 0.f;

    // float ShadowDepth = ShadowDepthMap.SampleLevel(ShadowDepthMapSampler,LightMapUV ,0);
    // if(ShadowDepth >  HomoPosition.z + 0.022f)
    // {
    //     fLightPercent = 1.0f;
    // }



    float3 L = normalize(LightPositionAndRadius.xyz - WorldPosition);
    float3 N = normalize(Input.WorldNormal);
    float3 V = normalize(ViewOrigin.xyz - WorldPosition);
    float3 H = normalize((L + V) / 2.f);

    float3 BaseColor = float3(1.0f,1.0f,1.0f);

    float3 AmibientBaseColor = BaseColor*Ka;
    float3 DiffuseBaseColor = BaseColor*Kd;

    float3 AmbientColor = AmibientBaseColor*AmbientIntencity;
    float3 LightIntencity = Intencity * fLightPercent; 

    float3 DiffuseColor = (LightIntencity * DiffuseBaseColor) * max(dot(L,N),0);
    //float3 SpecularColor = LightIntencity * pow(dot(H,V),Ns);

    OutColor = float4(DiffuseColor,1.f);
}