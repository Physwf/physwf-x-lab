#include "CoreGlobals.h"
// #include "Internationalization/Text.h"
// #include "Internationalization/Internationalization.h"
// #include "Stats/Stats.h"
// #include "HAL/IConsoleManager.h"
// #include "Modules/ModuleManager.h"
// #include "Misc/CoreStats.h"

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