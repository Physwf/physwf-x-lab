#ifndef __Common_H__
#define __Common_H__


//#include "/Engine/Public/Platform.ush"

// These types are used for material translator generated code, or any functions the translated code can call
#if PIXELSHADER
	#define MaterialFloat half
	#define MaterialFloat2 half2
	#define MaterialFloat3 half3
	#define MaterialFloat4 half4
	#define MaterialFloat3x3 half3x3
	#define MaterialFloat4x4 half4x4 
	#define MaterialFloat4x3 half4x3 
#else
	// Material translated vertex shader code always uses floats, 
	// Because it's used for things like world position and UVs
	#define MaterialFloat float
	#define MaterialFloat2 float2
	#define MaterialFloat3 float3
	#define MaterialFloat4 float4
	#define MaterialFloat3x3 float3x3
	#define MaterialFloat4x4 float4x4 
	#define MaterialFloat4x3 float4x3 
#endif

#include "UniformBuffers.hlsl"

//#include "CommonViewUniformBuffer.ush"

#include "InstancedStereo.hlsl"

//#include "Definitions.usf"

// Helper macro to determine whether we need to separately interpolate the world vertex normal to the pixel center.
// Currently only curvature-to-roughness needs this interpolation, so disable it when it's not used to save on interpolants.
#define USE_WORLDVERTEXNORMAL_CENTER_INTERPOLATION	1 //(FEATURE_LEVEL >= FEATURE_LEVEL_SM5 && MATERIAL_NORMAL_CURVATURE_TO_ROUGHNESS)
	// The D3D shader compiler combines _centroid and non centroid. Using float3 would result in a internal
	// shader compiler error. This block is using float4 to prevent that.
#if USE_WORLDVERTEXNORMAL_CENTER_INTERPOLATION
	#define TANGENTTOWORLD_INTERPOLATOR_BLOCK	float4 TangentToWorld0 : TEXCOORD10_centroid; float4	TangentToWorld2	: TEXCOORD11_centroid; \
		float4	TangentToWorld2_Center	: TEXCOORD15;
#else
	#define TANGENTTOWORLD_INTERPOLATOR_BLOCK	float4 TangentToWorld0 : TEXCOORD10_centroid; float4	TangentToWorld2	: TEXCOORD11_centroid;
#endif

// see PixelShaderOutputCommon
struct PixelShaderIn
{
	// read only
	float4 SvPosition;

	// Pixel Shader InCoverage, only usable if PIXELSHADEROUTPUT_COVERAGE is 1
	uint Coverage;

	//
	bool bIsFrontFace;
};
// see PixelShaderOutputCommon
struct PixelShaderOut
{
	// [0..7], only usable if PIXELSHADEROUTPUT_MRT0, PIXELSHADEROUTPUT_MRT1, ... is 1
	float4 MRT[8];

	// Pixel Shader OutCoverage, only usable if PIXELSHADEROUTPUT_COVERAGE is 1
	uint Coverage;

	// Pixel Shader OutDepth
	float Depth;
};

// Used for vertex factory shaders which need to use the resolved view
float3 SvPositionToResolvedTranslatedWorld(float4 SvPosition)
{
	float4 HomWorldPos = mul(float4(SvPosition.xyz, 1), ResolvedView.SVPositionToTranslatedWorld);

	return HomWorldPos.xyz / HomWorldPos.w;
}

// Used for vertex factory shaders which need to use the resolved view
float4 SvPositionToResolvedScreenPosition(float4 SvPosition)
{
	float2 PixelPos = SvPosition.xy - ResolvedView.ViewRectMin.xy;	

	// NDC (NormalizedDeviceCoordinates, after the perspective divide)
	float3 NDCPos = float3( (PixelPos * ResolvedView.ViewSizeAndInvSize.zw - 0.5f) * float2(2, -2), SvPosition.z);

	// SvPosition.w: so .w has the SceneDepth, some mobile code and the DepthFade material expression wants that
	return float4(NDCPos.xyz, 1) * SvPosition.w;
}

/** Transforms a vector from tangent space to world space */
MaterialFloat3 TransformTangentVectorToWorld(MaterialFloat3x3 TangentToWorld, MaterialFloat3 InTangentVector)
{
	// Transform directly to world space
	// The vector transform is optimized for this case, only one vector-matrix multiply is needed
	return mul(InTangentVector, TangentToWorld);
}

float3 TransformTangentNormalToWorld(MaterialFloat3x3 TangentToWorld, float3 TangentNormal)
{
	return normalize(float3(TransformTangentVectorToWorld(TangentToWorld, TangentNormal)));
}

/**
 * Returns the upper 3x3 portion of the LocalToWorld matrix.
 */
MaterialFloat3x3 GetLocalToWorld3x3()
{
	return (MaterialFloat3x3)Primitive.LocalToWorld;
}
#endif