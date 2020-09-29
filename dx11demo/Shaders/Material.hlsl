// #include "/Engine/Private/SceneTexturesCommon.ush"
// #include "/Engine/Private/Random.ush"
// #include "/Engine/Private/MonteCarlo.ush"
#include "MaterialUniformBuffers.hlsl"

#define NUM_MATERIAL_TEXCOORDS_VERTEX 1
#define NUM_TEX_COORD_INTERPOLATORS 1

/** 
 * Parameters calculated from the pixel material inputs.
 */
struct PixelMaterialInputs
{
	MaterialFloat3 EmissiveColor;
	MaterialFloat Opacity;
	MaterialFloat OpacityMask;
	MaterialFloat3 BaseColor;
	MaterialFloat Metallic;
	MaterialFloat Specular;
	MaterialFloat Roughness;
	MaterialFloat3 Normal;
	MaterialFloat4 Subsurface;
	MaterialFloat AmbientOcclusion;
	MaterialFloat2 Refraction;
	MaterialFloat PixelDepthOffset;

};

/** 
 * Parameters needed by pixel shader material inputs, related to Geometry.
 * These are independent of vertex factory.
 */
struct MaterialPixelParameters
{
#if NUM_TEX_COORD_INTERPOLATORS
	float2 TexCoords[NUM_TEX_COORD_INTERPOLATORS];
#endif

	/** Interpolated vertex color, in linear color space. */
	half4 VertexColor;

	/** Normalized world space normal. */
	half3 WorldNormal;

	/** Normalized world space reflected camera vector. */
	half3 ReflectionVector;

	/** Normalized world space camera vector, which is the vector from the point being shaded to the camera position. */
	half3 CameraVector;

	/** World space light vector, only valid when rendering a light function. */
	half3 LightVector;

	/**
	 * Like SV_Position (.xy is pixel position at pixel center, z:DeviceZ, .w:SceneDepth)
	 * using shader generated value SV_POSITION
	 * Note: this is not relative to the current viewport.  RelativePixelPosition = MaterialParameters.SvPosition.xy - View.ViewRectMin.xy;
	 */
	float4 SvPosition;
		
	/** Post projection position reconstructed from SvPosition, before the divide by W. left..top -1..1, bottom..top -1..1  within the viewport, W is the SceneDepth */
	float4 ScreenPosition;

	half UnMirrored;

	half TwoSidedSign;

	/**
	 * Orthonormal rotation-only transform from tangent space to world space
	 * The transpose(TangentToWorld) is WorldToTangent, and TangentToWorld[2] is WorldVertexNormal
	 */
	half3x3 TangentToWorld;

#if USE_WORLDVERTEXNORMAL_CENTER_INTERPOLATION
	/** World vertex normal interpolated at the pixel center that is safe to use for derivatives. */
	half3 WorldVertexNormal_Center;
#endif

	/** 
	 * Interpolated worldspace position of this pixel
	 * todo: Make this TranslatedWorldPosition and also rename the VS/DS/HS WorldPosition to be TranslatedWorldPosition
	 */
	float3 AbsoluteWorldPosition;

	/** 
	 * Interpolated worldspace position of this pixel, centered around the camera
	 */
	float3 WorldPosition_CamRelative;

	/** 
	 * Interpolated worldspace position of this pixel, not including any world position offset or displacement.
	 * Only valid if shader is compiled with NEEDS_WORLD_POSITION_EXCLUDING_SHADER_OFFSETS, otherwise just contains 0
	 */
	float3 WorldPosition_NoOffsets;

	/** 
	 * Interpolated worldspace position of this pixel, not including any world position offset or displacement.
	 * Only valid if shader is compiled with NEEDS_WORLD_POSITION_EXCLUDING_SHADER_OFFSETS, otherwise just contains 0
	 */
	float3 WorldPosition_NoOffsets_CamRelative;

	/** Offset applied to the lighting position for translucency, used to break up aliasing artifacts. */
	half3 LightingPositionOffset;

	float AOMaterialMask;

#if LIGHTMAP_UV_ACCESS
	float2	LightmapUVs;
#endif

#if USE_INSTANCING
	half4 PerInstanceParams;
#endif

	/** Per-particle properties. Only valid for particle vertex factories. */
	//FMaterialParticleParameters Particle;

// #if (ES2_PROFILE || ES3_1_PROFILE)
// 	float4 LayerWeights;
// #endif

#if TEX_COORD_SCALE_ANALYSIS
	/** Parameters used by the MaterialTexCoordScales shader. */
	FTexCoordScalesParams TexCoordScalesParams;
#endif

// #if POST_PROCESS_MATERIAL && (FEATURE_LEVEL <= FEATURE_LEVEL_ES3_1)
// 	/** Used in mobile custom pp material to preserve original SceneColor Alpha */
// 	half BackupSceneColorAlpha;
// #endif

#if COMPILER_HLSL
	// Workaround for "error X3067: 'GetObjectWorldPosition': ambiguous function call"
	// Which happens when FMaterialPixelParameters and FMaterialVertexParameters have the same number of floats with the HLSL compiler ver 9.29.952.3111
	// Function overload resolution appears to identify types based on how many floats / ints / etc they contain
	uint Dummy;
#endif
};

// @todo compat hack
MaterialPixelParameters MakeInitializedMaterialPixelParameters()
{
	MaterialPixelParameters MPP;
	MPP = (MaterialPixelParameters)0;
	MPP.TangentToWorld = float3x3(1,0,0,0,1,0,0,0,1);
	return MPP;
}

/** 
 * Parameters needed by vertex shader material inputs.
 * These are independent of vertex factory.
 */
struct MaterialVertexParameters
{
	// Position in the translated world (VertexFactoryGetWorldPosition).
	// Previous position in the translated world (VertexFactoryGetPreviousWorldPosition) if
	//    computing material's output for previous frame (See {BasePassVertex,Velocity}Shader.usf).
	float3 WorldPosition;
	// TangentToWorld[2] is WorldVertexNormal
	half3x3 TangentToWorld;

	float3 PreSkinnedPosition;
	float3 PreSkinnedNormal;

	half4 VertexColor;

    #if NUM_MATERIAL_TEXCOORDS_VERTEX
	float2 TexCoords[NUM_MATERIAL_TEXCOORDS_VERTEX];
	// #if (ES2_PROFILE || ES3_1_PROFILE)
	// float2 TexCoordOffset; // Offset for UV localization for large UV values
	// #endif
#endif

};

#if NUM_TEX_COORD_INTERPOLATORS
void GetMaterialCustomizedUVs(MaterialVertexParameters Parameters, inout float2 OutTexCoords[NUM_TEX_COORD_INTERPOLATORS])
{
	OutTexCoords[0] = Parameters.TexCoords[0].xy.rg.rg.rg.rg;

}

void GetCustomInterpolators(MaterialVertexParameters Parameters, inout float2 OutTexCoords[NUM_TEX_COORD_INTERPOLATORS])
{

}
#endif

half3 GetMaterialBaseColorRaw(PixelMaterialInputs Inputs)
{
	return Inputs.BaseColor;
}

half3 GetMaterialBaseColor(PixelMaterialInputs Inputs)
{
	return saturate(GetMaterialBaseColorRaw(Inputs));
}

// can return in tangent space or world space (use MATERIAL_TANGENTSPACENORMAL)
half3 GetMaterialNormalRaw(PixelMaterialInputs Inputs)
{
	return Inputs.Normal;
}

half GetMaterialMetallicRaw(PixelMaterialInputs Inputs)
{
	return Inputs.Metallic;
}

half GetMaterialMetallic(PixelMaterialInputs Inputs)
{
	return saturate(GetMaterialMetallicRaw(Inputs));
}

half GetMaterialSpecularRaw(PixelMaterialInputs Inputs)
{
	return Inputs.Specular;
}

half GetMaterialSpecular(PixelMaterialInputs Inputs)
{
	return saturate(GetMaterialSpecularRaw(Inputs));
}

half GetMaterialRoughnessRaw(PixelMaterialInputs Inputs)
{
	return Inputs.Roughness;
}

half GetMaterialRoughness(PixelMaterialInputs Inputs)
{
    return saturate(GetMaterialRoughnessRaw(Inputs));
}

half GetMaterialAmbientOcclusionRaw(PixelMaterialInputs Inputs)
{
	return Inputs.AmbientOcclusion;
}

half GetMaterialAmbientOcclusion(PixelMaterialInputs Inputs)
{
	return saturate(GetMaterialAmbientOcclusionRaw(Inputs));
}

half3 GetMaterialNormal(MaterialPixelParameters Parameters, PixelMaterialInputs Inputs)
{
	half3 RetNormal;

	RetNormal = GetMaterialNormalRaw(Inputs);
		
	// #if (USE_EDITOR_SHADERS && !(ES2_PROFILE || ES3_1_PROFILE || ESDEFERRED_PROFILE)) || MOBILE_EMULATION
	// {
	// 	// this feature is only needed for development/editor - we can compile it out for a shipping build (see r.CompileShadersForDevelopment cvar help)
	// 	half3 OverrideNormal = ResolvedView.NormalOverrideParameter.xyz;

	// 	#if !MATERIAL_TANGENTSPACENORMAL
	// 		OverrideNormal = Parameters.TangentToWorld[2] * (1 - ResolvedView.NormalOverrideParameter.w);
	// 	#endif

	// 	RetNormal = RetNormal * ResolvedView.NormalOverrideParameter.w + OverrideNormal;
	// }
	// #endif

	return RetNormal;
}


void CalcPixelMaterialInputs(in out MaterialPixelParameters Parameters, in out PixelMaterialInputs Inputs)
{
    Inputs.Normal = MaterialFloat3(0,0,1);

    float3 MaterialNormal = GetMaterialNormal(Parameters, Inputs);

#if MATERIAL_TANGENTSPACENORMAL
// #if SIMPLE_FORWARD_SHADING
// 	Parameters.WorldNormal = float3(0, 0, 1);
// #endif

//#if FEATURE_LEVEL >= FEATURE_LEVEL_SM4
	// ES2 will rely on only the final normalize for performance
	MaterialNormal = normalize(MaterialNormal);
//#endif

	// normalizing after the tangent space to world space conversion improves quality with sheared bases (UV layout to WS causes shrearing)
	// use full precision normalize to avoid overflows
	Parameters.WorldNormal = TransformTangentNormalToWorld(Parameters.TangentToWorld, MaterialNormal);

#else //MATERIAL_TANGENTSPACENORMAL

	Parameters.WorldNormal = normalize(MaterialNormal);

#endif //MATERIAL_TANGENTSPACENORMAL

#if MATERIAL_TANGENTSPACENORMAL
	// flip the normal for backfaces being rendered with a two-sided material
	Parameters.WorldNormal *= Parameters.TwoSidedSign;
#endif

    Inputs.EmissiveColor = Material.VectorExpressions[2].rgb;
	Inputs.Opacity = 1.00000000;
	Inputs.OpacityMask = 1.00000000;
	Inputs.BaseColor = float3(.5f, .6f, .7f);
	Inputs.Metallic = 1.0f;
	Inputs.Specular = 0.50000000;
	Inputs.Roughness = 0.5f;
	Inputs.Subsurface = 0;
	Inputs.AmbientOcclusion = 1.00000000;
	Inputs.Refraction = 0;
	Inputs.PixelDepthOffset = 0.00000000;
}

void CalcMaterialParametersEx(
	in out MaterialPixelParameters Parameters,
	in out PixelMaterialInputs Inputs,
	float4 SvPosition,
	float4 ScreenPosition,
	bool bIsFrontFace,
	float3 TranslatedWorldPosition,
	float3 TranslatedWorldPositionExcludingShaderOffsets)
{
    // Remove the pre view translation
	Parameters.WorldPosition_CamRelative = TranslatedWorldPosition.xyz;
	Parameters.AbsoluteWorldPosition = TranslatedWorldPosition.xyz - ResolvedView.PreViewTranslation.xyz;

    // If the material uses any non-offset world position expressions, calculate those parameters. If not, 
	// the variables will have been initialised to 0 earlier.
// #if USE_WORLD_POSITION_EXCLUDING_SHADER_OFFSETS
// 	Parameters.WorldPosition_NoOffsets_CamRelative = TranslatedWorldPositionExcludingShaderOffsets;
// 	Parameters.WorldPosition_NoOffsets = TranslatedWorldPositionExcludingShaderOffsets - ResolvedView.PreViewTranslation.xyz;
// #endif

    Parameters.SvPosition = SvPosition;
	Parameters.ScreenPosition = ScreenPosition;

    Parameters.CameraVector = normalize(-Parameters.WorldPosition_CamRelative.xyz);

    Parameters.LightVector = 0;

	Parameters.TwoSidedSign = 1.0f;

    CalcPixelMaterialInputs(Parameters, Inputs);
}

#define INPUT_POSITION_QUALIFIERS linear noperspective centroid
