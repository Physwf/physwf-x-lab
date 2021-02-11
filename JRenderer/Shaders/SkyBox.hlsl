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
    Output.WorldPosition = Input.Position;
    Output.SVPosition = mul(WorldToProj,float4(Input.Position,1));
    return Output;
}

TextureCube SkyBoxTexture;
SamplerState SkyBoxSamplerState;

float4 PSMain(VSOutput Input) : SV_Target
{
    return SkyBoxTexture.Sample(SkyBoxSamplerState,Input.WorldPosition);
}