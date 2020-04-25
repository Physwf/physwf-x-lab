#include "CoreGlobals.h"
// #include "Internationalization/Text.h"
// #include "Internationalization/Internationalization.h"
// #include "Stats/Stats.h"
// #include "HAL/IConsoleManager.h"
// #include "Modules/ModuleManager.h"
// #include "Misc/CoreStats.h"

#include "X0.h"


X0_API FMalloc*			GMalloc = nullptr;		/* Memory allocator */

bool					GIsRequestingExit = false;					/* Indicates that MainLoop() should be exited at the end of the current iteration */