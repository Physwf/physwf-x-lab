#include "Windows/WindowsPlatformMemory.h"
#include "Misc/AssertionMacros.h"
#include "Logging/LogMacros.h"
#include "Misc/OutputDevice.h"
#include "Math/NumericLimits.h"
//#include "Containers/UnrealString.h"
//#include "CoreGlobals.h"
//#include "Misc/OutputDeviceRedirector.h"
//#include "Stats/Stats.h"
//#include "GenericPlatform/GenericPlatformMemoryPoolStats.h"
#include "HAL/PlatformMath.h"

//#include "HAL/MallocTBB.h"
#include "HAL/MallocAnsi.h"
//#include "HAL/MallocStomp.h"
//#include "GenericPlatform/GenericPlatformMemoryPoolStats.h"
#include "HAL/MemoryMisc.h"
//#include "HAL/MallocBinned.h"
//#include "HAL/MallocBinned2.h"
#include "Windows/WindowsHWrapper.h"

#include "Windows/AllowWindowsPlatformTypes.h"
#include <Psapi.h>
#pragma comment(lib, "psapi.lib")

void FWindowsPlatformMemory::Init()
{
	FGenericPlatformMemory::Init();
}

FMalloc* FWindowsPlatformMemory::BaseAllocator()
{
#if ENABLE_WIN_ALLOC_TRACKING
	// This allows tracking of allocations that don't happen within the engine's wrappers.
	// This actually won't be compiled unless bDebugBuildsActuallyUseDebugCRT is set in the
	// build configuration for UBT.
	_CrtSetAllocHook(WindowsAllocHook);
#endif // ENABLE_WIN_ALLOC_TRACKING

	AllocatorToUse = EMemoryAllocatorToUse::Ansi;


	switch (AllocatorToUse)
	{
	default:	// intentional fall-through
	case EMemoryAllocatorToUse::Ansi:
		return new FMallocAnsi();
	}
}

FPlatformMemoryStats FWindowsPlatformMemory::GetStats()
{
	/**
	*	GlobalMemoryStatusEx
	*	MEMORYSTATUSEX
	*		ullTotalPhys
	*		ullAvailPhys
	*		ullTotalVirtual
	*		ullAvailVirtual
	*
	*	GetProcessMemoryInfo
	*	PROCESS_MEMORY_COUNTERS
	*		WorkingSetSize
	*		UsedVirtual
	*		PeakUsedVirtual
	*
	*	GetPerformanceInfo
	*		PPERFORMANCE_INFORMATION
	*		PageSize
	*/

	// This method is slow, do not call it too often.
	// #TODO Should be executed only on the background thread.

	FPlatformMemoryStats MemoryStats;

	// Gather platform memory stats.
	MEMORYSTATUSEX MemoryStatusEx;
	FPlatformMemory::Memzero(&MemoryStatusEx, sizeof(MemoryStatusEx));
	MemoryStatusEx.dwLength = sizeof(MemoryStatusEx);
	::GlobalMemoryStatusEx(&MemoryStatusEx);

	PROCESS_MEMORY_COUNTERS ProcessMemoryCounters;
	FPlatformMemory::Memzero(&ProcessMemoryCounters, sizeof(ProcessMemoryCounters));
	::GetProcessMemoryInfo(::GetCurrentProcess(), &ProcessMemoryCounters, sizeof(ProcessMemoryCounters));

	MemoryStats.AvailablePhysical = MemoryStatusEx.ullAvailPhys;
	MemoryStats.AvailableVirtual = MemoryStatusEx.ullAvailVirtual;

	MemoryStats.UsedPhysical = ProcessMemoryCounters.WorkingSetSize;
	MemoryStats.PeakUsedPhysical = ProcessMemoryCounters.PeakWorkingSetSize;
	MemoryStats.UsedVirtual = ProcessMemoryCounters.PagefileUsage;
	MemoryStats.PeakUsedVirtual = ProcessMemoryCounters.PeakPagefileUsage;

	return MemoryStats;
}

void FWindowsPlatformMemory::GetStatsForMallocProfiler(FGenericMemoryStats& out_Stats)
{

}

const FPlatformMemoryConstants& FWindowsPlatformMemory::GetConstants()
{
	static FPlatformMemoryConstants MemoryConstants;

	if (MemoryConstants.TotalPhysical == 0)
	{
		// Gather platform memory constants.
		MEMORYSTATUSEX MemoryStatusEx;
		FPlatformMemory::Memzero(&MemoryStatusEx, sizeof(MemoryStatusEx));
		MemoryStatusEx.dwLength = sizeof(MemoryStatusEx);
		::GlobalMemoryStatusEx(&MemoryStatusEx);

		SYSTEM_INFO SystemInfo;
		FPlatformMemory::Memzero(&SystemInfo, sizeof(SystemInfo));
		::GetSystemInfo(&SystemInfo);

		MemoryConstants.TotalPhysical = MemoryStatusEx.ullTotalPhys;
		MemoryConstants.TotalVirtual = MemoryStatusEx.ullTotalVirtual;
		MemoryConstants.BinnedPageSize = SystemInfo.dwAllocationGranularity;	// Use this so we get larger 64KiB pages, instead of 4KiB
		MemoryConstants.OsAllocationGranularity = SystemInfo.dwAllocationGranularity;	// VirtualAlloc cannot allocate memory less than that
		MemoryConstants.PageSize = SystemInfo.dwPageSize;
		MemoryConstants.AddressLimit = FPlatformMath::RoundUpToPowerOfTwo64(MemoryConstants.TotalPhysical);

		MemoryConstants.TotalPhysicalGB = (MemoryConstants.TotalPhysical + 1024 * 1024 * 1024 - 1) / 1024 / 1024 / 1024;
	}

	return MemoryConstants;
}

bool FWindowsPlatformMemory::PageProtect(void* const Ptr, const SIZE_T Size, const bool bCanRead, const bool bCanWrite)
{
	DWORD flOldProtect;
	uint32 ProtectMode = 0;
	if (bCanRead && bCanWrite)
	{
		ProtectMode = PAGE_READWRITE;
	}
	else if (bCanWrite)
	{
		ProtectMode = PAGE_READWRITE;
	}
	else if (bCanRead)
	{
		ProtectMode = PAGE_READONLY;
	}
	else
	{
		ProtectMode = PAGE_NOACCESS;
	}
	return VirtualProtect(Ptr, Size, ProtectMode, &flOldProtect) != 0;
}

void* FWindowsPlatformMemory::BinnedAllocFromOS(SIZE_T Size)
{
	void* Ptr = VirtualAlloc(NULL, Size, MEM_COMMIT, PAGE_READWRITE);
	//LLM(FLowLevelMemTracker::Get().OnLowLevelAlloc(ELLMTracker::Platform, Ptr, Size));
	return Ptr;
}

void FWindowsPlatformMemory::BinnedFreeToOS(void* Ptr, SIZE_T Size)
{
	//LLM(FLowLevelMemTracker::Get().OnLowLevelFree(ELLMTracker::Platform, Ptr));

	CA_SUPPRESS(6001)
		// Windows maintains the size of allocation internally, so Size is unused
	verify(VirtualFree(Ptr, 0, MEM_RELEASE) != 0);
}

bool FWindowsPlatformMemory::UnmapNamedSharedMemoryRegion(FSharedMemoryRegion * MemoryRegion)
{
	bool bAllSucceeded = true;

	if (MemoryRegion)
	{
		FWindowsSharedMemoryRegion * WindowsRegion = static_cast<FWindowsSharedMemoryRegion*>(MemoryRegion);

		if (!UnmapViewOfFile(WindowsRegion->GetAddress()))
		{
			bAllSucceeded = false;

			int ErrNo = GetLastError();
// 			UE_LOG(LogHAL, Warning, TEXT("UnmapViewOfFile(address=%p) failed with GetLastError() = %d"),
// 				WindowsRegion->GetAddress(),
// 				ErrNo
// 			);
		}

		if (!CloseHandle(WindowsRegion->GetMapping()))
		{
			bAllSucceeded = false;

			int ErrNo = GetLastError();
// 			UE_LOG(LogHAL, Warning, TEXT("CloseHandle(handle=0x%x) failed with GetLastError() = %d"),
// 				WindowsRegion->GetMapping(),
// 				ErrNo
// 			);
		}

		// delete the region
		delete WindowsRegion;
	}

	return bAllSucceeded;
}

/**
* LLM uses these low level functions (LLMAlloc and LLMFree) to allocate memory. It grabs
* the function pointers by calling FPlatformMemory::GetLLMAllocFunctions. If these functions
* are not implemented GetLLMAllocFunctions should return false and LLM will be disabled.
*/

#if ENABLE_LOW_LEVEL_MEM_TRACKER

int64 LLMMallocTotal = 0;

const size_t LLMPageSize = 4096;

void* LLMAlloc(size_t Size)
{
	int AlignedSize = Align(Size, LLMPageSize);

	off_t DirectMem = 0;
	void* Addr = VirtualAlloc(NULL, Size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	check(Addr);

	LLMMallocTotal += AlignedSize;

	return Addr;
}

void LLMFree(void* Addr, size_t Size)
{
	VirtualFree(Addr, 0, MEM_RELEASE);

	int AlignedSize = Align(Size, LLMPageSize);
	LLMMallocTotal -= AlignedSize;
}


bool FWindowsPlatformMemory::GetLLMAllocFunctions(void*(*&OutAllocFunction)(size_t), void(*&OutFreeFunction)(void*, size_t), int32& OutAlignment)
{
	OutAllocFunction = LLMAlloc;
	OutFreeFunction = LLMFree;
	OutAlignment = LLMPageSize;

	return true;
}

#endif // ENABLE_LOW_LEVEL_MEM_TRACKER

#include "Windows/HideWindowsPlatformTypes.h"