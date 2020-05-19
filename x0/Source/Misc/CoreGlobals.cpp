#include "CoreGlobals.h"
// #include "Internationalization/Text.h"
// #include "Internationalization/Internationalization.h"
// #include "Stats/Stats.h"
#include "HAL/IConsoleManager.h"
#include "Modules/ModuleManager.h"
// #include "Misc/CoreStats.h"
#include "HAL/PlatformTime.h"

#include "X0.h"

/* Whether we are using the event driven loader */
bool					GEventDrivenLoaderEnabled = false;

X0_API FMalloc*			GMalloc = nullptr;		/* Memory allocator */

bool					GIsRequestingExit = false;					/* Indicates that MainLoop() should be exited at the end of the current iteration */

/** Has GGameThreadId been set yet?																			*/
bool					GIsGameThreadIdInitialized = false;

/** Thread ID of the main/game thread																		*/
uint32					GGameThreadId = 0;
uint32					GRenderThreadId = 0;

/** NEED TO RENAME, for RT version of GFrameTime use View.ViewFamily->FrameNumber or pass down from RT from GFrameTime). */
uint32					GFrameNumberRenderThread = 1;

/** Steadily increasing frame counter.																		*/
TSAN_ATOMIC(uint64)		GFrameCounter(0);

bool					GIsCriticalError = false;					/* An appError() has occured */

/** Time at which FPlatformTime::Seconds() was first initialized (before main)											*/
double					GStartTime = FPlatformTime::InitTiming();

DEFINE_LOG_CATEGORY(LogHAL);
DEFINE_LOG_CATEGORY(LogMac);
DEFINE_LOG_CATEGORY(LogLinux);
DEFINE_LOG_CATEGORY(LogIOS);
DEFINE_LOG_CATEGORY(LogAndroid);
DEFINE_LOG_CATEGORY(LogWindows);
DEFINE_LOG_CATEGORY(LogXboxOne);
DEFINE_LOG_CATEGORY(LogSerialization);
DEFINE_LOG_CATEGORY(LogContentComparisonCommandlet);
DEFINE_LOG_CATEGORY(LogNetPackageMap);
DEFINE_LOG_CATEGORY(LogNetSerialization);
DEFINE_LOG_CATEGORY(LogMemory);
DEFINE_LOG_CATEGORY(LogProfilingDebugging);
DEFINE_LOG_CATEGORY(LogSwitch);

DEFINE_LOG_CATEGORY(LogTemp);