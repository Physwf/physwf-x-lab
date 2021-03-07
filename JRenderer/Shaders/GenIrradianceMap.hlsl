
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
            Output.WorldPosition = mul(FaceTransform[CubeFaceIndex],float4(Input[VertexIndex].Position,1.0f)).xyz;
            Output.SVPosition = mul(float4(Input[VertexIndex].Position,1.0f),Projection);
            TriStream.Append(Output);
        }
        TriStream.RestartStrip();
    }
     
}

TextureCube EnvironmentMap;
SamplerState EnvironmentMapSampler;

void CoordinateSystem(const float3 InZ,out float3 OutX, out float3 OutY )
{
    if(InZ.x == 0.f && InZ.y == 0.f)
    {
        OutX = float3(1.f,0.f,0.f);
    }
    else if(abs(InZ.x) > abs(InZ.y))
    {
        OutX = float3(-InZ.z, 0, InZ.x) / sqrt(InZ.x*InZ.x + InZ.z * InZ.z);
    }
    else 
    {
        OutX = float3( 0,InZ.z, -InZ.y) / sqrt(InZ.y*InZ.y + InZ.z * InZ.z);
    }
    OutY = cross(InZ,OutX);
}

float4 PSMain(GSOutput Input) : SV_Target
{
    float3 WorldPosition = normalize(Input.WorldPosition);
    float4 Color = 0;

    float3x3 WorldCoordinate;
    WorldCoordinate[2] = WorldPosition;
    CoordinateSystem(WorldCoordinate[2],WorldCoordinate[0],WorldCoordinate[1]);

    const float RadiansPerDegree = 3.14f / 180;

    float RadiansPhi = 0.f;
    for(int phi=0;phi<360;++phi)
    {
        float RadiuansTheta = 0.f;
        for(int theta=0;theta < 90;++theta)
        {
            float3 UV;
            UV.x = sin(RadiuansTheta) * cos(RadiansPhi);
            UV.y = sin(RadiuansTheta) * sin(RadiansPhi);
            UV.z = cos(RadiuansTheta);
            UV = mul(WorldCoordinate,UV);
            Color += EnvironmentMap.Sample(EnvironmentMapSampler,UV) * cos(RadiuansTheta);
        }
    }
    return Color / (360*90);
}
