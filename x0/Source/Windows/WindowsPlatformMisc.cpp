#include "Windows/WindowsPlatformMisc.h"
//#include "Misc/DateTime.h"
#include "Misc/AssertionMacros.h"
#include "Logging/LogMacros.h"
#include "Misc/OutputDevice.h"
//#include "HAL/PlatformStackWalk.h"
#include "HAL/PlatformProcess.h"
#include "HAL/UnrealMemory.h"
#include "Templates/UnrealTemplate.h"
#include "CoreGlobals.h"
//#include "HAL/FileManager.h"
#include "Misc/CString.h"
//#include "Misc/Parse.h"
//#include "Misc/MessageDialog.h"
#include "Containers/StringConv.h"
#include "Containers/UnrealString.h"
#include "CoreGlobals.h"
//#include "Misc/CommandLine.h"
//#include "Misc/Paths.h"
//#include "Internationalization/Text.h"
//#include "Internationalization/Culture.h"
//#include "Internationalization/Internationalization.h"
#include "Math/Color.h"
//#include "Misc/OutputDeviceRedirector.h"
//#include "Misc/OutputDeviceFile.h"
//#include "Misc/OutputDeviceError.h"
//#include "Misc/FeedbackContext.h"
//#include "Misc/CoreDelegates.h"
//#include "Misc/App.h"
//#include "HAL/ExceptionHandling.h"
//#include "Misc/SecureHash.h"
//#include "HAL/IConsoleManager.h"
//#include "Misc/EngineVersion.h"
//#include "GenericPlatform/GenericPlatformCrashContext.h"
//#include "Windows/WindowsPlatformCrashContext.h"
// #include "HAL/PlatformOutputDevices.h"

// #include "GenericPlatform/GenericPlatformChunkInstall.h"
// #include "GenericPlatform/GenericPlatformDriver.h"
// #include "HAL/ThreadHeartBeat.h"

// Resource includes.
//#include "Runtime/Launch/Resources/Windows/Resource.h"

#include "Windows/AllowWindowsPlatformTypes.h"
#include <time.h>
#include <mmsystem.h>
#include <rpcsal.h>
#include <gameux.h>
#include <ShlObj.h>
#include <IntShCut.h>
#include <shellapi.h>
#include <IPHlpApi.h>
#include <VersionHelpers.h>
#include "Windows/HideWindowsPlatformTypes.h"

#include "Modules/ModuleManager.h"

#if !FORCE_ANSI_ALLOCATOR
#include "Windows/AllowWindowsPlatformTypes.h"
#include <Psapi.h>
#include "Windows/HideWindowsPlatformTypes.h"
#pragma comment(lib, "psapi.lib")
#endif

#include <fcntl.h>
#include <io.h>

#include "Windows/AllowWindowsPlatformTypes.h"

//#include "FramePro/FrameProProfiler.h"

/**
* Class that caches __cpuid queried data.
*/
class FCPUIDQueriedData
{
public:
	FCPUIDQueriedData()
		: bHasCPUIDInstruction(CheckForCPUIDInstruction()), Vendor(), CPUInfo(0), CacheLineSize(1)
	{
		if (bHasCPUIDInstruction)
		{
			Vendor = QueryCPUVendor();
			Brand = QueryCPUBrand();
			int Info[4];
			QueryCPUInfo(Info);
			CPUInfo = Info[0];
			CPUInfo2 = Info[2];
			CacheLineSize = QueryCacheLineSize();
		}
	}

	/**
	 * Checks if this CPU supports __cpuid instruction.
	 *
	 * @returns True if this CPU supports __cpuid instruction. False otherwise.
	 */
	static bool HasCPUIDInstruction()
	{
		return CPUIDStaticCache.bHasCPUIDInstruction;
	}

	/**
	 * Gets pre-cached CPU vendor name.
	 *
	 * @returns CPU vendor name.
	 */
	static const FString& GetVendor()
	{
		return CPUIDStaticCache.Vendor;
	}

	/**
	* Gets pre-cached CPU brand string.
	*
	* @returns CPU brand string.
	*/
	static const FString& GetBrand()
	{
		return CPUIDStaticCache.Brand;
	}

	/**
	 * Gets __cpuid CPU info.
	 *
	 * @returns CPU info unsigned int queried using __cpuid.
	 */
	static uint32 GetCPUInfo()
	{
		return CPUIDStaticCache.CPUInfo;
	}

	/**
	* Gets __cpuid CPU info.
	*
	* @returns CPU info unsigned int queried using __cpuid.
	*/
	static uint32 GetCPUInfo2()
	{
		return CPUIDStaticCache.CPUInfo2;
	}

	/**
	 * Gets cache line size.
	 *
	 * @returns Cache line size.
	 */
	static int32 GetCacheLineSize()
	{
		return CPUIDStaticCache.CacheLineSize;
	}

private:
	/**
	 * Checks if __cpuid instruction is present on current machine.
	 *
	 * @returns True if this CPU supports __cpuid instruction. False otherwise.
	 */
	static bool CheckForCPUIDInstruction()
	{
#if PLATFORM_SEH_EXCEPTIONS_DISABLED
		return false;
#else
		__try
		{
			int Args[4];
			__cpuid(Args, 0);
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			return false;
		}
		return true;
#endif
	}

	/**
	 * Queries Vendor name using __cpuid instruction.
	 *
	 * @returns CPU vendor name.
	 */
	static FString QueryCPUVendor()
	{
		union
		{
			char Buffer[12 + 1];
			struct
			{
				int dw0;
				int dw1;
				int dw2;
			} Dw;
		} VendorResult;

		int Args[4];
		__cpuid(Args, 0);

		VendorResult.Dw.dw0 = Args[1];
		VendorResult.Dw.dw1 = Args[3];
		VendorResult.Dw.dw2 = Args[2];
		VendorResult.Buffer[12] = 0;

		return ANSI_TO_TCHAR(VendorResult.Buffer);
	}

	/**
	 * Queries brand string using __cpuid instruction.
	 *
	 * @returns CPU brand string.
	 */
	static FString QueryCPUBrand()
	{
		// @see for more information http://msdn.microsoft.com/en-us/library/vstudio/hskdteyh(v=vs.100).aspx
		ANSICHAR BrandString[0x40] = { 0 };
		int32 CPUInfo[4] = { -1 };
		const SIZE_T CPUInfoSize = sizeof(CPUInfo);

		__cpuid(CPUInfo, 0x80000000);
		const uint32 MaxExtIDs = CPUInfo[0];

		if (MaxExtIDs >= 0x80000004)
		{
			const uint32 FirstBrandString = 0x80000002;
			const uint32 NumBrandStrings = 3;
			for (uint32 Index = 0; Index < NumBrandStrings; Index++)
			{
				__cpuid(CPUInfo, FirstBrandString + Index);
				FPlatformMemory::Memcpy(BrandString + CPUInfoSize * Index, CPUInfo, CPUInfoSize);
			}
		}

		return ANSI_TO_TCHAR(BrandString);
	}

	/**
	 * Queries CPU info using __cpuid instruction.
	 *
	 * @returns CPU info unsigned int queried using __cpuid.
	 */
	static void QueryCPUInfo(int Args[4])
	{
		__cpuid(Args, 1);
	}

	/**
	 * Queries cache line size using __cpuid instruction.
	 *
	 * @returns Cache line size.
	 */
	static int32 QueryCacheLineSize()
	{
		int32 Result = 1;

		int Args[4];
		__cpuid(Args, 0x80000006);

		Result = Args[2] & 0xFF;
		check(Result && !(Result & (Result - 1))); // assumed to be a power of two

		return Result;
	}

	/** Static field with pre-cached __cpuid data. */
	static FCPUIDQueriedData CPUIDStaticCache;

	/** If machine has CPUID instruction. */
	bool bHasCPUIDInstruction;

	/** Vendor of the CPU. */
	FString Vendor;

	/** CPU brand. */
	FString Brand;

	/** CPU info from __cpuid. */
	uint32 CPUInfo;
	uint32 CPUInfo2;

	/** CPU cache line size. */
	int32 CacheLineSize;
};

/** Static initialization of data to pre-cache __cpuid queries. */
FCPUIDQueriedData FCPUIDQueriedData::CPUIDStaticCache;

int32 FWindowsPlatformMisc::GetCacheLineSize()
{
	return FCPUIDQueriedData::GetCacheLineSize();
}