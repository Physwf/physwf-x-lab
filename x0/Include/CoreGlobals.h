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

/** @return True if called from the rendering thread, or if called from ANY thread during single threaded rendering */
extern X0_API bool IsInRenderingThread();

/** @return True if called from the rendering thread, or if called from ANY thread that isn't the game thread, except that during single threaded rendering the game thread is ok too.*/
extern X0_API bool IsInParallelRenderingThread();

extern X0_API bool GIsRequestingExit;

/** Steadily increasing frame counter. */
extern X0_API TSAN_ATOMIC(uint64) GFrameCounter;

/** Incremented once per frame before the scene is being rendered. In split screen mode this is incremented once for all views (not for each view). */
extern X0_API uint32 GFrameNumber;