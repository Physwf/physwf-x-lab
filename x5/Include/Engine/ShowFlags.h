#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineBaseTypes.h"

/** for FShowFlagData, order is not important, adding new elements requires updating some editor code */
enum EShowFlagGroup
{
	SFG_Normal,				// currently one level higher in the hierarchy the the other groups
	SFG_Advanced,
	SFG_PostProcess,
	SFG_CollisionModes,
	SFG_Developer,
	SFG_Visualize,
	SFG_LightTypes,
	SFG_LightingComponents,
	SFG_LightingFeatures,
	SFG_Hidden,
	SFG_Max
};

/** To customize the EngineShowFlags constructor */
enum EShowFlagInitMode
{
	ESFIM_Game,
	ESFIM_Editor,
	ESFIM_All0
};

#if (UE_BUILD_SHIPPING)
#define UE_BUILD_OPTIMIZED_SHOWFLAGS 1
#else
#define UE_BUILD_OPTIMIZED_SHOWFLAGS 0
#endif

/**
 * ShowFlags are a set of bits (some are fixed in SHIPPING) that are stored in the ViewFamily.
 * They can be used by the artist/designer/developer to debug/profile parts of the rendering.
 * Some ShowFlags are used to customize the rendering e.g. by the SceneCaptureActor, those should not be locked in shipping and not used in inner loops (for performance)
 * They should not be used for scalability (as some might be compiled out for SHIPPING), there we use console variables.
 * ViewModes are on a higher level, they can manipulate show flags before they get used.
 */
struct FEngineShowFlags
{
};