#pragma once

#include "CoreTypes.h"
#include "Containers/UnrealString.h"
//#include "UObject/NameTypes.h"
#include "Logging/LogMacros.h"
#include "HAL/PlatformTLS.h"
#include "Templates/Atomic.h"

#include "X0.h"

class Error;
class FConfigCacheIni;
class FFixedUObjectArray;
class FChunkedFixedUObjectArray;
class FOutputDeviceConsole;
class FOutputDeviceRedirector;
class ITransaction;

X0_API DECLARE_LOG_CATEGORY_EXTERN(LogHAL, Log, All);
X0_API DECLARE_LOG_CATEGORY_EXTERN(LogMac, Log, All);
X0_API DECLARE_LOG_CATEGORY_EXTERN(LogLinux, Log, All);
X0_API DECLARE_LOG_CATEGORY_EXTERN(LogIOS, Log, All);
X0_API DECLARE_LOG_CATEGORY_EXTERN(LogAndroid, Log, All);
X0_API DECLARE_LOG_CATEGORY_EXTERN(LogPS4, Log, All);
X0_API DECLARE_LOG_CATEGORY_EXTERN(LogXboxOne, Log, All);
X0_API DECLARE_LOG_CATEGORY_EXTERN(LogWindows, Log, All);
X0_API DECLARE_LOG_CATEGORY_EXTERN(LogSwitch, Log, All);
X0_API DECLARE_LOG_CATEGORY_EXTERN(LogQuail, Log, All);
X0_API DECLARE_LOG_CATEGORY_EXTERN(LogSerialization, Log, All);
X0_API DECLARE_LOG_CATEGORY_EXTERN(LogUnrealMath, Log, All);
X0_API DECLARE_LOG_CATEGORY_EXTERN(LogUnrealMatrix, Log, All);
X0_API DECLARE_LOG_CATEGORY_EXTERN(LogContentComparisonCommandlet, Log, All);
X0_API DECLARE_LOG_CATEGORY_EXTERN(LogNetPackageMap, Warning, All);
X0_API DECLARE_LOG_CATEGORY_EXTERN(LogNetSerialization, Warning, All);
X0_API DECLARE_LOG_CATEGORY_EXTERN(LogMemory, Log, All);
X0_API DECLARE_LOG_CATEGORY_EXTERN(LogProfilingDebugging, Log, All);
X0_API DECLARE_LOG_CATEGORY_EXTERN(LogCore, Log, All);
X0_API DECLARE_LOG_CATEGORY_EXTERN(LogOutputDevice, Log, All);

X0_API DECLARE_LOG_CATEGORY_EXTERN(LogSHA, Warning, All);
X0_API DECLARE_LOG_CATEGORY_EXTERN(LogStats, Log, All);
X0_API DECLARE_LOG_CATEGORY_EXTERN(LogStreaming, Display, All);
X0_API DECLARE_LOG_CATEGORY_EXTERN(LogInit, Log, All);
X0_API DECLARE_LOG_CATEGORY_EXTERN(LogExit, Log, All);
X0_API DECLARE_LOG_CATEGORY_EXTERN(LogExec, Warning, All);
X0_API DECLARE_LOG_CATEGORY_EXTERN(LogScript, Warning, All);
X0_API DECLARE_LOG_CATEGORY_EXTERN(LogLocalization, Error, All);
X0_API DECLARE_LOG_CATEGORY_EXTERN(LogLongPackageNames, Log, All);
X0_API DECLARE_LOG_CATEGORY_EXTERN(LogProcess, Log, All);
X0_API DECLARE_LOG_CATEGORY_EXTERN(LogLoad, Log, All);

// Temporary log category, generally you should not check things in that use this
X0_API DECLARE_LOG_CATEGORY_EXTERN(LogTemp, Log, All);

/* Whether we are using the event driven loader */
extern X0_API bool GEventDrivenLoaderEnabled;

extern X0_API bool GIsCriticalError;

/** Has GGameThreadId been set yet? */
extern X0_API bool GIsGameThreadIdInitialized;
/** NEED TO RENAME, for RT version of GFrameTime use View.ViewFamily->FrameNumber or pass down from RT from GFrameTime). */
extern X0_API uint32 GFrameNumberRenderThread;

/** Thread ID of the main/game thread */
extern X0_API uint32 GGameThreadId;

/** Thread ID of the render thread, if any */
extern X0_API uint32 GRenderThreadId;

FORCEINLINE bool IsInGameThread()
{
	if (GIsGameThreadIdInitialized)
	{
		const uint32 CurrentThreadId = FPlatformTLS::GetCurrentThreadId();
		return CurrentThreadId == GGameThreadId;
	}

	return true;
}

/** @return True if called from the rendering thread, or if called from ANY thread during single threaded rendering */
extern X0_API bool IsInRenderingThread();

/** @return True if called from the rendering thread, or if called from ANY thread that isn't the game thread, except that during single threaded rendering the game thread is ok too.*/
extern X0_API bool IsInParallelRenderingThread();

/** @return True if called from the rendering thread. */
// Unlike IsInRenderingThread, this will always return false if we are running single threaded. It only returns true if this is actually a separate rendering thread. Mostly useful for checks
extern X0_API bool IsInActualRenderingThread();

extern X0_API bool GIsRequestingExit;

/** Steadily increasing frame counter. */
extern X0_API TSAN_ATOMIC(uint64) GFrameCounter;

/** Incremented once per frame before the scene is being rendered. In split screen mode this is incremented once for all views (not for each view). */
extern X0_API uint32 GFrameNumber;

/** Thread used for rendering */
extern X0_API FRunnableThread* GRenderingThread;

/** Whether the rendering thread is suspended (not even processing the tickables) */
extern X0_API TAtomic<int32> GIsRenderingThreadSuspended;

/** @return True if called from the RHI thread, or if called from ANY thread during single threaded rendering */
extern X0_API bool IsInRHIThread();

/** Thread used for RHI */
extern X0_API FRunnableThread* GRHIThread_InternalUseOnly;

/** Thread ID of the the thread we are executing RHI commands on. This could either be a constant dedicated thread or changing every task if we run the rhi thread on tasks. */
extern X0_API uint32 GRHIThreadId;

X0_API FOutputDeviceRedirector* GetGlobalLogSingleton();

X0_API extern class FOutputDeviceError*			GError;

#define GLog GetGlobalLogSingleton()

/** Time at which FPlatformTime::Seconds() was first initialized (very early on) */
extern X0_API double GStartTime;

extern X0_API TCHAR GErrorHist[16384];

// #crashReport: 2014-08-19 Combine into one, refactor.
extern X0_API TCHAR GErrorExceptionDescription[4096];
extern X0_API TCHAR GErrorMessage[4096];