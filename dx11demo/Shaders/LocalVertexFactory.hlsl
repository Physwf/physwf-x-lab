#include "VertexFactoryCommon.hlsl"
#include "LocalVertexFactoryCommon.hlsl"

struct VertexFactoryInput
{
    float4 Position 	        : ATTRIBUTE0;
    float3 TangentX 		    : ATTRIBUTE1;
    float4 TangentZ 		    : ATTRIBUTE2;
	float4 Color 		        : ATTRIBUTE3;
    float2 TexCoords[1]         : ATTRIBUTE4;
    float2	LightMapCoordinate  : ATTRIBUTE5;
};

struct PositionOnlyVertexFactoryInput
{
    float4 Position 	        : ATTRIBUTE0;
};

struct VertexFactoryIntermediates
{
    half3x3 TangentToLocal;
	half3x3 TangentToWorld;
	half TangentToWorldSign;

    half4 Color;
};

half3x3 CalcTangentToLocal(VertexFactoryInput Input,out float TangentSign)
{
    half3x3 Result;

    half3 TangentInputX = Input.TangentX;
    half4 TangentInputZ = Input.TangentZ;

    //half3 TangentX = TangentBias(TangentInputX);
	//half4 TangentZ = TangentBias(TangentInputZ);

    half3 TangentX = TangentInputX;
	half4 TangentZ = TangentInputZ;

    TangentSign = TangentZ.w;

    half3 TangentY = cross(TangentZ.xyz,TangentX) * TangentZ.w;

    Result[0] = cross(TangentY,TangentZ.xyz) * TangentZ.w;
    Result[1] = TangentY;
    Result[2] = TangentZ.xyz;

    return Result;
}

half3x3 CalcTangentToWorldNoScale(in half3x3 TangentToLocal)
{
    half3x3 LocalToWorld = GetLocalToWorld3x3();
	half3 InvScale = Primitive.InvNonUniformScale.xyz;
	// LocalToWorld[0] *= InvScale.x;
	// LocalToWorld[1] *= InvScale.y;
	// LocalToWorld[2] *= InvScale.z;
    return mul(TangentToLocal,LocalToWorld);
}

half3x3 CalcTangentToWorld(VertexFactoryIntermediates Intermediates, half3x3 TangentToLocal)
{
    half3x3 TangentToWorld = CalcTangentToWorldNoScale(TangentToLocal);
    return TangentToWorld;
}
//颜色,切线空间坐标系
VertexFactoryIntermediates GetVertexFactoryIntermediates(VertexFactoryInput Input)
{
    VertexFactoryIntermediates Intermediates;
    Intermediates = (VertexFactoryIntermediates)0;

    Intermediates.Color = Input.Color;

    float TangentSign;
    Intermediates.TangentToLocal = CalcTangentToLocal(Input,TangentSign);
    Intermediates.TangentToWorld = CalcTangentToWorld(Intermediates,Intermediates.TangentToLocal);

    return Intermediates;
};

float4 CalcWorldPosition(float4 Position)
{
    return TransformLocalToTranslatedWorld(Position.xyz);
}

// @return translated world position
float4 VertexFactoryGetWorldPosition(VertexFactoryInput Input, VertexFactoryIntermediates Intermediates)
{
    return CalcWorldPosition(Input.Position);
}

/** for depth-only pass */
float4 VertexFactoryGetWorldPosition(PositionOnlyVertexFactoryInput Input)
{
	float4 Position = Input.Position;

    return CalcWorldPosition(Position);
}

/**
* Get the 3x3 tangent basis vectors for this vertex factory
* this vertex factory will calculate the binormal on-the-fly
*
* @param Input - vertex input stream structure
* @return 3x3 matrix
*/
half3x3 VertexFactoryGetTangentToLocal(VertexFactoryInput Input, VertexFactoryIntermediates Intermediates )
{
	return Intermediates.TangentToLocal;
}

/** Converts from vertex factory specific input to a FMaterialVertexParameters, which is used by vertex shader material inputs. */
MaterialVertexParameters GetMaterialVertexParameters(VertexFactoryInput Input, VertexFactoryIntermediates Intermediates, float3 WorldPosition, half3x3 TangentToLocal)
{
	MaterialVertexParameters Result = (MaterialVertexParameters)0;
	Result.WorldPosition = WorldPosition;
	Result.VertexColor = Intermediates.Color;

	// does not handle instancing!
	Result.TangentToWorld = Intermediates.TangentToWorld;

    Result.PreSkinnedPosition = Input.Position.xyz;
	Result.PreSkinnedNormal = TangentToLocal[2]; //TangentBias(Input.TangentZ.xyz);

    return Result;
}

float4 VertexFactoryGetRasterizedWorldPosition(VertexFactoryInput Input, VertexFactoryIntermediates Intermediates, float4 InWorldPosition)
{
	return InWorldPosition;
}

VertexFactoryInterpolantsVSToPS VertexFactoryGetInterpolantsVSToPS(VertexFactoryInput Input, VertexFactoryIntermediates Intermediates, MaterialVertexParameters VertexParameters)
{
	VertexFactoryInterpolantsVSToPS Interpolants;
    Interpolants = (VertexFactoryInterpolantsVSToPS)0;

#if NUM_TEX_COORD_INTERPOLATORS
    float2 CustomizedUVs[NUM_TEX_COORD_INTERPOLATORS];
	GetMaterialCustomizedUVs(VertexParameters, CustomizedUVs);
	GetCustomInterpolators(VertexParameters, CustomizedUVs);

    [unroll]//UNROLL
	for (int CoordinateIndex = 0; CoordinateIndex < NUM_TEX_COORD_INTERPOLATORS; CoordinateIndex++)
	{
		SetUV(Interpolants, CoordinateIndex, CustomizedUVs[CoordinateIndex]);
	}
//#elif NUM_MATERIAL_TEXCOORDS_VERTEX == 0 && USE_PARTICLE_SUBUVS

#endif

// #if NEEDS_LIGHTMAP_COORDINATE
// #endif	// NEEDS_LIGHTMAP_COORDINATE

	SetTangents(Interpolants, Intermediates.TangentToWorld[0], Intermediates.TangentToWorld[2], Intermediates.TangentToWorldSign);
	SetColor(Interpolants, Intermediates.Color);

    return Interpolants;
}

MaterialPixelParameters GetMaterialPixelParameters(VertexFactoryInterpolantsVSToPS Interpolants, float4 SvPosition)
{
    MaterialPixelParameters Result = MakeInitializedMaterialPixelParameters();

    #if NUM_TEX_COORD_INTERPOLATORS
	[unroll]//UNROLL
	for( int CoordinateIndex = 0; CoordinateIndex < NUM_TEX_COORD_INTERPOLATORS; CoordinateIndex++ )
	{
		Result.TexCoords[CoordinateIndex] = GetUV(Interpolants, CoordinateIndex);
	}
    #endif

    half3 TangentToWorld0 = GetTangentToWorld0(Interpolants).xyz;
	half4 TangentToWorld2 = GetTangentToWorld2(Interpolants);
	Result.UnMirrored = TangentToWorld2.w;

	Result.VertexColor = GetColor(Interpolants);

    Result.TangentToWorld = AssembleTangentToWorld( TangentToWorld0, TangentToWorld2 );
	// Required for previewing materials that use ParticleColor
	//Result.Particle.Color = half4(1,1,1,1);

    Result.TwoSidedSign = 1;
    return Result;
}