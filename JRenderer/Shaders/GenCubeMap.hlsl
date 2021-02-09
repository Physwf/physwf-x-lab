
struct VSInput
{
    float3 Position : POSITION;
};

struct VSOutput
{
    float3 Position : POSITION;
};

struct GSOutput
{
    float3 WorldPosition : TEXCOORD0;
    float4 SVPosition : SV_Position;
    uint RenderTargetArrayIndex : SV_RenderTargetArrayIndex;
};

VSOutput VSMain(VSInput Input) 
{
    VSOutput Output = (VSOutput)0;
    Output.Position = Input.Position;
    return Output;
}

cbuffer View 
{
    float4x4 FaceTransform[6];
    float4x4 Projection;
};

[maxvertexcount(18)]
void GSMain(
    triangle VSOutput Input[3],
    inout TriangleStream<GSOutput> TriStream)
{
    [unroll]
    for(int CubeFaceIndex=0;CubeFaceIndex<6;++CubeFaceIndex)
    {
        GSOutput Output = (GSOutput)0;
        Output.RenderTargetArrayIndex = CubeFaceIndex;
        [unroll]
        for(int VertexIndex=0;VertexIndex<3;++VertexIndex)
        {
            Output.WorldPosition = mul(float4(Input[VertexIndex].Position,1.0f), FaceTransform[0]).xyz;
            Output.SVPosition = mul(float4(Input[VertexIndex].Position,1.0f),Projection);
            TriStream.Append(Output);
        }
        TriStream.RestartStrip();
    }
     
}

Texture2D HDRI;
SamplerState HDRISampler;

float4 PSMain(GSOutput Input) :SV_Target
{
    float3 ViewDir = normalize(Input.WorldPosition);
    float2 ViewDirXZ = normalize(ViewDir.xz);
    float2 UV;
    UV.x = ViewDirXZ.x;
    UV.y = ViewDir.y * 0.5 + 0.5;
    return HDRI.Sample(HDRISampler,UV);
}
