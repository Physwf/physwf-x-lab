#ifndef __Common_H__
#define __Common_H__


#include "Platform.hlsl"

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

#include "Definitions.hlsl"

#include "Random.hlsl"

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

MaterialFloat Luminance( MaterialFloat3 LinearColor )
{
	return dot( LinearColor, MaterialFloat3( 0.3, 0.59, 0.11 ) );
}

const static float PI = 3.1415926535897932f;

MaterialFloat length2(MaterialFloat2 v)
{
	return dot(v, v);
}
MaterialFloat length2(MaterialFloat3 v)
{
	return dot(v, v);
}
MaterialFloat length2(MaterialFloat4 v)
{
	return dot(v, v);
}

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

// shadow and light function
Texture2D		LightAttenuationTexture;
SamplerState	LightAttenuationTextureSampler;

// We don't use an inline function so we can avoid type promotion/ coercion.
#define RETURN_COLOR( Color ) ( Color )

float Square( float x )
{
	return x*x;
}

float2 Square( float2 x )
{
	return x*x;
}

float3 Square( float3 x )
{
	return x*x;
}

float4 Square( float4 x )
{
	return x*x;
}

float Pow2( float x )
{
	return x*x;
}

float2 Pow2( float2 x )
{
	return x*x;
}

float3 Pow2( float3 x )
{
	return x*x;
}

float4 Pow2( float4 x )
{
	return x*x;
}

float Pow3( float x )
{
	return x*x*x;
}

float2 Pow3( float2 x )
{
	return x*x*x;
}

float3 Pow3( float3 x )
{
	return x*x*x;
}

float4 Pow3( float4 x )
{
	return x*x*x;
}

float Pow4( float x )
{
	float xx = x*x;
	return xx * xx;
}

float2 Pow4( float2 x )
{
	float2 xx = x*x;
	return xx * xx;
}

float3 Pow4( float3 x )
{
	float3 xx = x*x;
	return xx * xx;
}

float4 Pow4( float4 x )
{
	float4 xx = x*x;
	return xx * xx;
}

float Pow5( float x )
{
	float xx = x*x;
	return xx * xx * x;
}

float2 Pow5( float2 x )
{
	float2 xx = x*x;
	return xx * xx * x;
}

float3 Pow5( float3 x )
{
	float3 xx = x*x;
	return xx * xx * x;
}

float4 Pow5( float4 x )
{
	float4 xx = x*x;
	return xx * xx * x;
}

float Pow6( float x )
{
	float xx = x*x;
	return xx * xx * xx;
}

float2 Pow6( float2 x )
{
	float2 xx = x*x;
	return xx * xx * xx;
}

float3 Pow6( float3 x )
{
	float3 xx = x*x;
	return xx * xx * xx;
}

float4 Pow6( float4 x )
{
	float4 xx = x*x;
	return xx * xx * xx;
}
// This would need to be a #define in GLSL to ignore the SamplerState, however, it is currently a function call in HLSL
// for type checking of the parameters - ironically the type checking is really only needed in GLSL!
MaterialFloat4 Texture1DSample(Texture1D Tex, SamplerState Sampler, float UV)
{
#if COMPUTESHADER
	return Tex.SampleLevel(Sampler, UV, 0);
#else
	return Tex.Sample(Sampler, UV);
#endif
}
MaterialFloat4 Texture2DSample(Texture2D Tex, SamplerState Sampler, float2 UV)
{
#if COMPUTESHADER
	return Tex.SampleLevel(Sampler, UV, 0);
#else
	return Tex.Sample(Sampler, UV);
#endif
}
MaterialFloat4 Texture2DSampleLevel(Texture2D Tex, SamplerState Sampler, float2 UV, MaterialFloat Mip)
{
	return Tex.SampleLevel(Sampler, UV, Mip);
}
MaterialFloat4 Texture3DSampleLevel(Texture3D Tex, SamplerState Sampler, float3 UV, MaterialFloat Mip)
{
	return Tex.SampleLevel(Sampler, UV, Mip);
}
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

float ConvertFromDeviceZ(float DeviceZ)
{
    return DeviceZ * View.InvDeviceZToWorldZTransform[0] + View.InvDeviceZToWorldZTransform[1] + 1.0f / (DeviceZ * View.InvDeviceZToWorldZTransform[2] - View.InvDeviceZToWorldZTransform[3]);
}

float2 ScreenPositionToBufferUV(float4 ScreenPosition)
{
	return float2(ScreenPosition.xy / ScreenPosition.w * ResolvedView.ScreenPositionScaleBias.xy + ResolvedView.ScreenPositionScaleBias.wz);
}

float2 SvPositionToBufferUV(float4 SvPosition)
{
	return SvPosition.xy * View.BufferSizeAndInvSize.zw;
}

// Used for post process shaders which don't need to resolve the view	
float3 SvPositionToTranslatedWorld(float4 SvPosition)
{
	float4 HomWorldPos = mul(float4(SvPosition.xyz, 1), View.SVPositionToTranslatedWorld);

	return HomWorldPos.xyz / HomWorldPos.w;
}

// prefer to use SvPositionToTranslatedWorld() for better quality
float3 SvPositionToWorld(float4 SvPosition)
{
	return SvPositionToTranslatedWorld(SvPosition) - View.PreViewTranslation;
}

float4 GetPerPixelLightAttenuation(float2 UV)
{
	return Square(Texture2DSampleLevel(LightAttenuationTexture, LightAttenuationTextureSampler, UV, 0));
}

void DrawRectangle(in float4 InPosition, in float2 InTexCoord,out float4 OutPosition, out float2 OutTexCoord)
{
    OutPosition = InPosition;
    OutPosition.xy = -1.0f + 2.f * (DrawRectangleParameters.PosScaleBias.zw + (InPosition.xy * DrawRectangleParameters.PosScaleBias.xy)) * DrawRectangleParameters.InvTargetSizeAndTextureSize.xy;
    OutPosition.xy *= float2(1,-1);
    OutTexCoord.xy = (DrawRectangleParameters.UVScaleBias.zw + (InTexCoord.xy * DrawRectangleParameters.UVScaleBias.xy)) * DrawRectangleParameters.InvTargetSizeAndTextureSize.xy;
}

/** Converts a linear input value into a value to be stored in the light attenuation buffer. */
MaterialFloat EncodeLightAttenuation(MaterialFloat InColor)
{
	// Apply a 1/2 power to the input, which allocates more bits for the darks and prevents banding
	// Similar to storing colors in gamma space, except this uses less instructions than a pow(x, 1/2.2)
	return sqrt(InColor);
}

/** Converts a linear input value into a value to be stored in the light attenuation buffer. */
MaterialFloat4 EncodeLightAttenuation(MaterialFloat4 InColor)
{
	return sqrt(InColor);
}

#endif