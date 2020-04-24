#include "Windows/WindowsPlatformProcess.h"
#include "HAL/PlatformMisc.h"
#include "Misc/AssertionMacros.h"
#include "Logging/LogMacros.h"
//#include "HAL/PlatformAffinity.h"
#include "HAL/UnrealMemory.h"
#include "Templates/UnrealTemplate.h"
#include "CoreGlobals.h"
//#include "HAL/FileManager.h"
//#include "Misc/Parse.h"
#include "Containers/StringConv.h"
#include "Containers/UnrealString.h"
#include "Containers/Set.h"
//#include "Misc/SingleThreadEvent.h"
//#include "Misc/CommandLine.h"
//#include "Misc/Paths.h"
//#include "Internationalization/Internationalization.h"
#include "CoreGlobals.h"
//#include "Stats/Stats.h"
//#include "Misc/CoreStats.h"
#include "Windows/WindowsHWrapper.h"

void FWindowsPlatformProcess::Sleep(float Seconds)
{
	//SCOPE_CYCLE_COUNTER(STAT_Sleep);
	//FThreadIdleStats::FScopeIdle Scope;
	SleepNoStats(Seconds);
}

void FWindowsPlatformProcess::SleepNoStats(float Seconds)
{
	uint32 Milliseconds = (uint32)(Seconds * 1000.0);
	if (Milliseconds == 0)
	{
		::SwitchToThread();
	}
	::Sleep(Milliseconds);
}