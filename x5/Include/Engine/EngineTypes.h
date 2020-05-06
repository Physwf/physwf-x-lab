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