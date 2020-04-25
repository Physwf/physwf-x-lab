#pragma once

#include "CoreTypes.h"
#include "Containers/UnrealString.h"
//#include "UObject/NameTypes.h"
#include "Logging/LogMacros.h"
#include "HAL/PlatformTLS.h"
#include "Templates/Atomic.h"

#include "X0.h"

/** NEED TO RENAME, for RT version of GFrameTime use View.ViewFamily->FrameNumber or pass down from RT from GFrameTime). */
extern X0_API uint32 GFrameNumberRenderThread;

FORCEINLINE bool IsInGameThread()
{
// 	if (GIsGameThreadIdInitialized)
// 	{
// 		const uint32 CurrentThreadId = FPlatformTLS::GetCurrentThreadId();
// 		return CurrentThreadId == GGameThreadId;
// 	}

	return true;
}

extern X0_API bool GIsRequestingExit;