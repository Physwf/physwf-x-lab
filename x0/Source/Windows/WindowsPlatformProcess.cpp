#include "Windows/WindowsPlatformProcess.h"
#include "HAL/PlatformMisc.h"
#include "Misc/AssertionMacros.h"
#include "Logging/LogMacros.h"
#include "HAL/PlatformAffinity.h"
#include "HAL/UnrealMemory.h"
#include "Templates/UnrealTemplate.h"
#include "CoreGlobals.h"
//#include "HAL/FileManager.h"
//#include "Misc/Parse.h"
#include "Containers/StringConv.h"
#include "Containers/UnrealString.h"
#include "Containers/Set.h"
#include "Misc/SingleThreadEvent.h"
//#include "Misc/CommandLine.h"
//#include "Misc/Paths.h"
//#include "Internationalization/Internationalization.h"
#include "CoreGlobals.h"
//#include "Stats/Stats.h"
//#include "Misc/CoreStats.h"
#include "Windows/WindowsHWrapper.h"

#include "Windows/AllowWindowsPlatformTypes.h"
#include <shellapi.h>
#include <ShlObj.h>
#include <LM.h>
#include <Psapi.h>
#include <TlHelp32.h>

namespace ProcessConstants
{
	uint32 WIN_STD_INPUT_HANDLE = STD_INPUT_HANDLE;
	uint32 WIN_STD_OUTPUT_HANDLE = STD_OUTPUT_HANDLE;
	uint32 WIN_ATTACH_PARENT_PROCESS = ATTACH_PARENT_PROCESS;
	uint32 WIN_STILL_ACTIVE = STILL_ACTIVE;
}

#include "Windows/HideWindowsPlatformTypes.h"
#include "Windows/WindowsPlatformMisc.h"

uint32 FWindowsPlatformProcess::GetCurrentProcessId()
{
	return ::GetCurrentProcessId();
}

void FWindowsPlatformProcess::SetThreadAffinityMask(uint64 AffinityMask)
{
	if (AffinityMask != FPlatformAffinity::GetNoAffinityMask())
	{
		::SetThreadAffinityMask(::GetCurrentThread(), (DWORD_PTR)AffinityMask);
	}
}

bool FWindowsPlatformProcess::IsApplicationRunning(const TCHAR* ProcName)
{
	// append the extension

	FString ProcNameWithExtension = ProcName;
	if (ProcNameWithExtension.Find(TEXT(".exe"), ESearchCase::IgnoreCase, ESearchDir::FromEnd) == INDEX_NONE)
	{
		ProcNameWithExtension += TEXT(".exe");
	}

	HANDLE SnapShot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (SnapShot != INVALID_HANDLE_VALUE)
	{
		PROCESSENTRY32 Entry;
		Entry.dwSize = sizeof(PROCESSENTRY32);

		if (::Process32First(SnapShot, &Entry))
		{
			do
			{
				if (FCString::Stricmp(*ProcNameWithExtension, Entry.szExeFile) == 0)
				{
					::CloseHandle(SnapShot);
					return true;
				}
			} while (::Process32Next(SnapShot, &Entry));
		}
	}

	::CloseHandle(SnapShot);
	return false;
}

bool FWindowsPlatformProcess::IsApplicationRunning(uint32 ProcessId)
{
	bool bApplicationRunning = true;
	HANDLE ProcessHandle = ::OpenProcess(SYNCHRONIZE, false, ProcessId);
	if (ProcessHandle == NULL)
	{
		bApplicationRunning = false;
	}
	else
	{
		uint32 WaitResult = WaitForSingleObject(ProcessHandle, 0);
		if (WaitResult != WAIT_TIMEOUT)
		{
			bApplicationRunning = false;
		}
		::CloseHandle(ProcessHandle);
	}
	return bApplicationRunning;
}

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

#include "WindowsEvent.h"

FEvent* FWindowsPlatformProcess::CreateSynchEvent(bool bIsManualReset)
{
	// Allocate the new object
	FEvent* Event = NULL;
	if (FPlatformProcess::SupportsMultithreading())
	{
		Event = new FEventWin();
	}
	else
	{
		// Fake event object.
		Event = new FSingleThreadEvent();
	}
	// If the internal create fails, delete the instance and return NULL
	if (!Event->Create(bIsManualReset))
	{
		delete Event;
		Event = NULL;
	}
	return Event;
}

#include "Windows/AllowWindowsPlatformTypes.h"

bool FEventWin::Wait(uint32 WaitTime, const bool bIgnoreThreadIdleStats /*= false*/)
{
	WaitForStats();

	//SCOPE_CYCLE_COUNTER(STAT_EventWait);
	check(Event);

	//FThreadIdleStats::FScopeIdle Scope(bIgnoreThreadIdleStats);
	return (WaitForSingleObject(Event, WaitTime) == WAIT_OBJECT_0);
}

void FEventWin::Trigger()
{
	TriggerForStats();
	check(Event);
	SetEvent(Event);
}

void FEventWin::Reset()
{
	ResetForStats();
	check(Event);
	ResetEvent(Event);
}

#include "Windows/HideWindowsPlatformTypes.h"

#include "WindowsRunnableThread.h"

FRunnableThread* FWindowsPlatformProcess::CreateRunnableThread()
{
	return new FRunnableThreadWin();
}