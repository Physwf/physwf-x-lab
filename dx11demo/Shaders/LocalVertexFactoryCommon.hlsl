struct VertexFactoryInterpolantsVSToPS
{
    TANGENTTOWORLD_INTERPOLATOR_BLOCK

#if INTERPOLATE_VERTEX_COLOR
	half4	Color : COLOR0;
#endif

#if NUM_TEX_COORD_INTERPOLATORS
	float4	TexCoords[(NUM_TEX_COORD_INTERPOLATORS+1)/2]	: TEXCOORD0;
// #elif USE_PARTICLE_SUBUVS
// 	float4	TexCoords[1] : TEXCOORD0;
#endif
};

#if NUM_TEX_COORD_INTERPOLATORS /*|| USE_PARTICLE_SUBUVS*/
float2 GetUV(VertexFactoryInterpolantsVSToPS Interpolants, int UVIndex)
{
	float4 UVVector = Interpolants.TexCoords[UVIndex / 2];
	return UVIndex % 2 ? UVVector.zw : UVVector.xy;
}

void SetUV(inout VertexFactoryInterpolantsVSToPS Interpolants, int UVIndex, float2 InValue)
{
	[flatten]//FLATTEN
	if (UVIndex % 2)
	{
		Interpolants.TexCoords[UVIndex / 2].zw = InValue;
	}
	else
	{
		Interpolants.TexCoords[UVIndex / 2].xy = InValue;
	}
}
#endif

float4 GetTangentToWorld2(VertexFactoryInterpolantsVSToPS Interpolants)
{
	return Interpolants.TangentToWorld2;
}

float4 GetTangentToWorld0(VertexFactoryInterpolantsVSToPS Interpolants)
{
	return Interpolants.TangentToWorld0;
}

float4 GetColor(VertexFactoryInterpolantsVSToPS Interpolants)
{
    return 0;
}