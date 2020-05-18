#pragma once

#include "CoreMinimal.h"
//#include "UObject/ObjectMacros.h"
//#include "UObject/Object.h"
//#include "UObject/Class.h"
//#include "Templates/SubclassOf.h"
//#include "Engine/NetSerialization.h"
//#include "EngineTypes.generated.h"


//UENUM()
enum EMaterialShadingModel
{
	MSM_Unlit				/*UMETA(DisplayName = "Unlit")*/,
	MSM_DefaultLit			/*UMETA(DisplayName = "Default Lit")*/,
	MSM_Subsurface			/*UMETA(DisplayName = "Subsurface")*/,
	MSM_PreintegratedSkin	/*UMETA(DisplayName = "Preintegrated Skin")*/,
	MSM_ClearCoat			/*UMETA(DisplayName = "Clear Coat")*/,
	MSM_SubsurfaceProfile	/*UMETA(DisplayName = "Subsurface Profile")*/,
	MSM_TwoSidedFoliage		/*UMETA(DisplayName = "Two Sided Foliage")*/,
	MSM_Hair				/*UMETA(DisplayName = "Hair")*/,
	MSM_Cloth				/*UMETA(DisplayName = "Cloth")*/,
	MSM_Eye					/*UMETA(DisplayName = "Eye")*/,
	MSM_MAX,
};

/** Note: This is mirrored in Lightmass, be sure to update the blend mode structure and logic there if this changes. */
// Note: Check UMaterialInstance::Serialize if changed!!
//UENUM(BlueprintType)
enum EBlendMode
{
	BLEND_Opaque /*UMETA(DisplayName = "Opaque")*/,
	BLEND_Masked /*UMETA(DisplayName = "Masked")*/,
	BLEND_Translucent /*UMETA(DisplayName = "Translucent")*/,
	BLEND_Additive /*UMETA(DisplayName = "Additive")*/,
	BLEND_Modulate /*UMETA(DisplayName = "Modulate")*/,
	BLEND_AlphaComposite /*UMETA(DisplayName = "AlphaComposite (Premultiplied Alpha)")*/,
	BLEND_MAX,
};

//UENUM()
enum ESamplerSourceMode
{
	/** Get the sampler from the texture.  Every unique texture will consume a sampler slot, which are limited in number. */
	SSM_FromTextureAsset/* UMETA(DisplayName = "From texture asset")*/,
	/** Shared sampler source that does not consume a sampler slot.  Uses wrap addressing and gets filter mode from the world texture group. */
	SSM_Wrap_WorldGroupSettings /*UMETA(DisplayName = "Shared: Wrap")*/,
	/** Shared sampler source that does not consume a sampler slot.  Uses clamp addressing and gets filter mode from the world texture group. */
	SSM_Clamp_WorldGroupSettings/* UMETA(DisplayName = "Shared: Clamp")*/
};

inline uint8 GetDefaultLightingChannelMask()
{
	return 1;
}

#define NUM_LIGHTING_CHANNELS 3


/**
 * Enumerates available options for the translucency sort policy.
 */
/*UENUM()*/
namespace ETranslucentSortPolicy
{
	enum Type
	{
		/** Sort based on distance from camera centerpoint to bounding sphere centerpoint. (Default, best for 3D games.) */
		SortByDistance = 0,

		/** Sort based on the post-projection Z distance to the camera. */
		SortByProjectedZ = 1,

		/** Sort based on the projection onto a fixed axis. (Best for 2D games.) */
		SortAlongAxis = 2,
	};
}

/*UENUM()*/
enum ESceneCaptureCompositeMode
{
	SCCM_Overwrite /*UMETA(DisplayName = "Overwrite")*/,
	SCCM_Additive /*UMETA(DisplayName = "Additive")*/,
	SCCM_Composite /*UMETA(DisplayName = "Composite")*/
};

/*UENUM()*/
enum ESceneCaptureSource
{
	SCS_SceneColorHDR /*UMETA(DisplayName = "SceneColor (HDR) in RGB, Inv Opacity in A")*/,
	SCS_SceneColorHDRNoAlpha /*UMETA(DisplayName = "SceneColor (HDR) in RGB, 0 in A")*/,
	SCS_FinalColorLDR /*UMETA(DisplayName = "Final Color (LDR) in RGB")*/,
	SCS_SceneColorSceneDepth /*UMETA(DisplayName = "SceneColor (HDR) in RGB, SceneDepth in A")*/,
	SCS_SceneDepth /*UMETA(DisplayName = "SceneDepth in R")*/,
	SCS_DeviceDepth /*UMETA(DisplayName = "DeviceDepth in RGB")*/,
	SCS_Normal /*UMETA(DisplayName = "Normal in RGB (Deferred Renderer only)")*/,
	SCS_BaseColor /*UMETA(DisplayName = "BaseColor in RGB (Deferred Renderer only)")*/
};

/** This is used by the drawing passes to determine tessellation policy, so changes here need to be supported in native code. */
/*UENUM()*/
enum EMaterialTessellationMode
{
	/** Tessellation disabled. */
	MTM_NoTessellation /*UMETA(DisplayName = "No Tessellation")*/,
	/** Simple tessellation. */
	MTM_FlatTessellation/* UMETA(DisplayName = "Flat Tessellation")*/,
	/** Simple spline based tessellation. */
	MTM_PNTriangles/* UMETA(DisplayName = "PN Triangles")*/,
	MTM_MAX,
};

/*UENUM()*/
enum ETranslucencyLightingMode
{
	/**
	* Lighting will be calculated for a volume, without directionality.  Use this on particle effects like smoke and dust.
	* This is the cheapest per-pixel lighting method, however the material normal is not taken into account.
	*/
	TLM_VolumetricNonDirectional /*UMETA(DisplayName = "Volumetric NonDirectional")*/,

	/**
	* Lighting will be calculated for a volume, with directionality so that the normal of the material is taken into account.
	* Note that the default particle tangent space is facing the camera, so enable bGenerateSphericalParticleNormals to get a more useful tangent space.
	*/
	TLM_VolumetricDirectional /*UMETA(DisplayName = "Volumetric Directional")*/,

	/**
	* Same as Volumetric Non Directional, but lighting is only evaluated at vertices so the pixel shader cost is significantly less.
	* Note that lighting still comes from a volume texture, so it is limited in range.  Directional lights become unshadowed in the distance.
	*/
	TLM_VolumetricPerVertexNonDirectional /*UMETA(DisplayName = "Volumetric PerVertex NonDirectional")*/,

	/**
	* Same as Volumetric Directional, but lighting is only evaluated at vertices so the pixel shader cost is significantly less.
	* Note that lighting still comes from a volume texture, so it is limited in range.  Directional lights become unshadowed in the distance.
	*/
	TLM_VolumetricPerVertexDirectional /*UMETA(DisplayName = "Volumetric PerVertex Directional")*/,

	/**
	* Lighting will be calculated for a surface. The light in accumulated in a volume so the result is blurry,
	* limited distance but the per pixel cost is very low. Use this on translucent surfaces like glass and water.
	* Only diffuse lighting is supported.
	*/
	TLM_Surface /*UMETA(DisplayName = "Surface TranslucencyVolume")*/,

	/**
	* Lighting will be calculated for a surface. Use this on translucent surfaces like glass and water.
	* This is implemented with forward shading so specular highlights from local lights are supported, however many deferred-only features are not.
	* This is the most expensive translucency lighting method as each light's contribution is computed per-pixel.
	*/
	TLM_SurfacePerPixelLighting /*UMETA(DisplayName = "Surface ForwardShading")*/,

	TLM_MAX,
};
/** Determines how the refraction offset should be computed for the material. */
/*UENUM()*/
enum ERefractionMode
{
	/**
	* Refraction is computed based on the camera vector entering a medium whose index of refraction is defined by the Refraction material input.
	* The new medium's surface is defined by the material's normal.  With this mode, a flat plane seen from the side will have a constant refraction offset.
	* This is a physical model of refraction but causes reading outside the scene color texture so is a poor fit for large refractive surfaces like water.
	*/
	RM_IndexOfRefraction /*UMETA(DisplayName = "Index Of Refraction")*/,

	/**
	* The refraction offset into Scene Color is computed based on the difference between the per-pixel normal and the per-vertex normal.
	* With this mode, a material whose normal is the default (0, 0, 1) will never cause any refraction.  This mode is only valid with tangent space normals.
	* The refraction material input scales the offset, although a value of 1.0 maps to no refraction, and a value of 2 maps to a scale of 1.0 on the offset.
	* This is a non-physical model of refraction but is useful on large refractive surfaces like water, since offsets have to stay small to avoid reading outside scene color.
	*/
	RM_PixelNormalOffset /*UMETA(DisplayName = "Pixel Normal Offset")*/
};