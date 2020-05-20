#include "Misc/CoreMisc.h"
#include "Misc/Parse.h"
#include "Misc/CommandLine.h"
//#include "Containers/Ticker.h"
#include "HAL/IConsoleManager.h"
#include "HAL/PlatformTime.h"
#include "Misc/App.h"
#include "Misc/ScopeLock.h"

/** For FConfigFile in appInit							*/
//#include "Misc/ConfigCacheIni.h"

#include "Modules/ModuleManager.h"
//#include "DerivedDataCacheInterface.h"
//#include "Interfaces/ITargetPlatformManagerModule.h"

bool X0_API StringHasBadDashes(const TCHAR* Str)
{
	// Detect dashes (0x2013) and report an error if they're found
	while (TCHAR Ch = *Str++)
	{
		if ((UCS2CHAR)Ch == 0x2013)
			return true;
	}

	return false;
}
