
struct VS_INPUT
{
    float2 Position : POSITION;
    float2 UV : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 Position : SV_Position;
    float2 UV : TEXCOORD0;
};

VS_OUTPUT VSMain(VS_INPUT Input)
{
    VS_OUTPUT Output;
    Output.Position = float4(Input.Position,1.f, 1.f) ;
    Output.UV = Input.UV;
    return Output;
}

uint SampleCount;
Texture2DMS<float4> TriangleSceneColorMS;

float4 PSMain(VS_OUTPUT Input) : SV_Target
{
    float4 Color = 0;

    //[unroll]//unable to unroll loop, loop does not appear to terminate in a timely manner 
    for(uint i=0;i<SampleCount;++i)
    {
        int2 Coords = Input.UV * float2(1920,1080);
        Color += TriangleSceneColorMS.Load(Coords,i);
    }
    return Color / SampleCount;
}