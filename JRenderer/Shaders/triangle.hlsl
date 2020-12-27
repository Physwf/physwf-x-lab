
struct VS_INPUT
{
    float3 Position : POSITION;
    float4 Color : COLOR;
};

struct VS_OUTPUT
{
    float4 Position : SV_Position;
    float4 Color : COLOR;
};

VS_OUTPUT VSMain(VS_INPUT Input)
{
    VS_OUTPUT Output;
    Output.Position = float4(Input.Position,1.0f);
    Output.Color = Input.Color;
    return Output;
}

float4 PSMain(VS_OUTPUT Input) : SV_Target
{
    return Input.Color;
}