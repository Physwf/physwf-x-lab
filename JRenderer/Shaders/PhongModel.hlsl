
struct VSInput
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float4 Tangent : TANGENT;
    float2 UV : TEXCOORD;
};

struct VS_Output
{
    float4 Position : SV_Position;
    float3 Normal :NORMAL ;
    float2 UV : TEXCOORD0;
    float4 TangentToWorld0          : TEXCOORD10_centroid; 
    float4 TangentToWorld2	        : TEXCOORD11_centroid;
    float4 TangentToWorld2_Center   : TEXCOORD15;
};

cbuffer View : register(b0)
{
    float4x4 WorldToClip;
    float4x4 ClipToWorld;
}

cbuffer Primitive : register(b1)
{
    float4x4 LocalToWorld;
    float LocalToWorldDeterminantSign;
}

void VSMain(VSInput Input, out VS_Output Output)
{
    //see CalcTangentToLocal
    float3x3 TangentToLocal;
    float3x3 TangentToWorld;
    float TangentSign;
    
    float4 TangentX = Input.Tangent;
    float3 TangentZ = Input.Normal;

    TangentSign = TangentX.w;

    float TangentToWorldSign = TangentSign * LocalToWorldDeterminantSign;

    float3 TangentY = cross(TangentZ,TangentX.xyz);

    TangentToLocal[0] = cross(TangentY,TangentZ);
    TangentToLocal[1] = TangentY;
    TangentToLocal[2] = TangentZ;

    TangentToWorld = mul(TangentToLocal,(float3x3)LocalToWorld);

    Output.TangentToWorld0 = float4(TangentToWorld[0],0);
	Output.TangentToWorld2 = float4(TangentToWorld[2],TangentToWorldSign);
	Output.TangentToWorld2_Center = Output.TangentToWorld2;

    float4 WorldPostion = mul(LocalToWorld,float4(Input.Position,1.0f));
    Output.Position = mul(WorldToClip,WorldPostion);
    Output.Normal = float3(0,0,1);
    Output.UV = Input.UV;
}

cbuffer AmbientLight : register(b2)
{
    float3 AmbientLightColor;
};

cbuffer PointLight : register(b3)
{
    float3 LightPosition;
    float3 LightColor;
};

cbuffer Material: register(b4)
{
    float3 ka;
    float3 kd;
    float3 ks;
    float alpha;
}

void PSMain(VS_Output Input,out float4 OutColor : SV_Target)
{
    float3 TangentToWorld1 = cross(Input.TangentToWorld2.xyz,Input.TangentToWorld0.xyz) * Input.TangentToWorld2.w;
    float3x3 TangentToWorld = float3x3(Input.TangentToWorld0.xyz, TangentToWorld1, Input.TangentToWorld2.xyz);

    float3 WorldNormal = mul(TangentToWorld, Input.Normal);
    //WorldNormal = WorldNormal*0.5 + 0.5;
    float3 WorldPostion = mul(float4(Input.Position.xyz,1),ClipToWorld);
    float3 CameraVector = WorldPostion;

    float3 V = normalize(CameraVector);
    float3 N = WorldNormal;
    float3 L = normalize(LightPosition);
    float3 R = 2.f * dot(L,N) * N - L;
    float3 DisffuseColor = ka * LightColor * max(dot(N,L),0);
    float3 SpecularColor = ks * LightColor * pow(max(dot(R,V),0),alpha);
    float3 AmbientColor = ka * AmbientLightColor;

    OutColor = float4(AmbientColor+DisffuseColor+SpecularColor,1);
}