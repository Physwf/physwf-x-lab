#pragma once

#include "CoreMinimal.h"
//#include "UObject/ObjectMacros.h"
#include "Templates/RefCounting.h"
#include "Containers/List.h"

//#include "SceneTypes.generated.h"


/** Quality levels that a material can be compiled for. */
namespace EMaterialQualityLevel
{
	enum Type
	{
		Low,
		High,
		Medium,
		Num
	};
}

//
//	EMaterialProperty
//
/*UENUM(BlueprintType)*/
enum EMaterialProperty
{
	MP_EmissiveColor = 0 /*UMETA(DisplayName = "Emissive")*/,
	MP_Opacity /*UMETA(DisplayName = "Opacity")*/,
	MP_OpacityMask /*UMETA(DisplayName = "Opacity Mask")*/,
	MP_DiffuseColor /*UMETA(Hidden)*/,			// used in Lightmass, not exposed to user, computed from: BaseColor, Metallic
	MP_SpecularColor /*UMETA(Hidden)*/,			// used in Lightmass, not exposed to user, derived from: SpecularColor, Metallic, Specular
	MP_BaseColor/* UMETA(DisplayName = "Diffuse")*/,
	MP_Metallic /*UMETA(DisplayName = "Metallic")*/,
	MP_Specular /*UMETA(DisplayName = "Specular")*/,
	MP_Roughness /*UMETA(DisplayName = "Roughness ")*/,
	MP_Normal /*UMETA(DisplayName = "Normal")*/,
	MP_WorldPositionOffset /*UMETA(Hidden)*/,
	MP_WorldDisplacement /*UMETA(Hidden)*/,
	MP_TessellationMultiplier /*UMETA(Hidden)*/,
	MP_SubsurfaceColor /*UMETA(DisplayName = "Subsurface")*/,
	MP_CustomData0 /*UMETA(Hidden)*/,
	MP_CustomData1 /*UMETA(Hidden)*/,
	MP_AmbientOcclusion /*UMETA(DisplayName = "Ambient Occlusion")*/,
	MP_Refraction /*UMETA(DisplayName = "Refraction")*/,
	MP_CustomizedUVs0 /*UMETA(Hidden)*/,
	MP_CustomizedUVs1 /*UMETA(Hidden)*/,
	MP_CustomizedUVs2 /*UMETA(Hidden)*/,
	MP_CustomizedUVs3 /*UMETA(Hidden)*/,
	MP_CustomizedUVs4 /*UMETA(Hidden)*/,
	MP_CustomizedUVs5 /*UMETA(Hidden)*/,
	MP_CustomizedUVs6 /*UMETA(Hidden)*/,
	MP_CustomizedUVs7 /*UMETA(Hidden)*/,
	MP_PixelDepthOffset /*UMETA(Hidden)*/,

	//^^^ New material properties go above here ^^^^
	MP_MaterialAttributes /*UMETA(Hidden)*/,
	MP_CustomOutput /*UMETA(Hidden)*/,
	MP_MAX /*UMETA(DisplayName = "None")*/,
};