struct VSInput
{
    float3 Position : POSITION;
};

struct VSOutput
{
    float3 WorldPosition : TEXCOORD0;
    float4 SVPosition : SV_Position;
};

cbuffer View
{
    float4x4 WorldToProj;
};

VSOutput VSMain(VSInput Input)
{
    VSOutput Output = (VSOutput)0;
    Output.WorldPosition = float4(Input.Position,1);
    Output.SVPosition = mul(float4(Output.WorldPosition,1),WorldToProj);
    return Output;
}

TextureCube SkyBoxTexture;
SamplerState SkyBoxSamplerState;

void PSMain()
{
    
}