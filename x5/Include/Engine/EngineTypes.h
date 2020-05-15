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