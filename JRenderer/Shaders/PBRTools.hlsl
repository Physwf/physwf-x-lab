

struct VS_Input
{
    float3 Position : POSITION;
    float4 Color : COLOR;
};

struct VS_Output
{
    float4 Postion : SV_Position;
    float4 Color : COLOR;
};


cbuffer View
{
    float4x4 WorldToView;
    float4x4 ViewToClip;
}

void VSMain(VS_Input Input,out VS_Output Output) 
{
    float3 ViewPostion = mul(WorldToView,float4(Input.Position,1));
    Output.Postion = mul(ViewToClip,float4(ViewPostion,1));
    Output.Color = Input.Color;
}

float4 PSMain(VS_Output Input) : SV_Target
{
    return Input.Color;
}