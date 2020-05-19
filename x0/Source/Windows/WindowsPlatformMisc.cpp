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
#include "Misc/SecureHash.h"
//#include "HAL/IConsoleManager.h"
//#include "Misc/EngineVersion.h"
//#include "GenericPlatform/GenericPlatformCrashContext.h"
//#include "Windows/WindowsPlatformCrashContext.h"
// #include "HAL/PlatformOutputDevices.h"

// #include "GenericPlatform/GenericPlatformChunkInstall.h"
#include "GenericPlatform/GenericPlatformDriver.h"
// #include "HAL/ThreadHeartBeat.h"

// Resource includes.
//#include "Runtime/Launch/Resources/Windows/Resource.h"

#include "Misc/CoreMisc.h"

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

int32 FWindowsOSVersionHelper::GetOSVersions(FString& out_OSVersionLabel, FString& out_OSSubVersionLabel)
{
	int32 ErrorCode = (int32)SUCCEEDED;

	// Get system info
	SYSTEM_INFO SystemInfo;
	if (FPlatformMisc::Is64bitOperatingSystem())
	{
		GetNativeSystemInfo(&SystemInfo);
	}
	else
	{
		GetSystemInfo(&SystemInfo);
	}

	OSVERSIONINFOEX OsVersionInfo = { 0 };
	OsVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	out_OSVersionLabel = TEXT("Windows (unknown version)");
	out_OSSubVersionLabel = FString();
#pragma warning(push)
#pragma warning(disable : 4996) // 'function' was declared deprecated
	CA_SUPPRESS(28159)
		if (GetVersionEx((LPOSVERSIONINFO)&OsVersionInfo))
#pragma warning(pop)
		{
			bool bIsInvalidVersion = false;

			switch (OsVersionInfo.dwMajorVersion)
			{
			case 5:
				switch (OsVersionInfo.dwMinorVersion)
				{
				case 0:
					out_OSVersionLabel = TEXT("Windows 2000");
					if (OsVersionInfo.wProductType == VER_NT_WORKSTATION)
					{
						out_OSSubVersionLabel = TEXT("Professional");
					}
					else
					{
						if (OsVersionInfo.wSuiteMask & VER_SUITE_DATACENTER)
						{
							out_OSSubVersionLabel = TEXT("Datacenter Server");
						}
						else if (OsVersionInfo.wSuiteMask & VER_SUITE_ENTERPRISE)
						{
							out_OSSubVersionLabel = TEXT("Advanced Server");
						}
						else
						{
							out_OSSubVersionLabel = TEXT("Server");
						}
					}
					break;
				case 1:
					out_OSVersionLabel = TEXT("Windows XP");
					if (OsVersionInfo.wSuiteMask & VER_SUITE_PERSONAL)
					{
						out_OSSubVersionLabel = TEXT("Home Edition");
					}
					else
					{
						out_OSSubVersionLabel = TEXT("Professional");
					}
					break;
				case 2:
					if (GetSystemMetrics(SM_SERVERR2))
					{
						out_OSVersionLabel = TEXT("Windows Server 2003 R2");
					}
					else if (OsVersionInfo.wSuiteMask & VER_SUITE_STORAGE_SERVER)
					{
						out_OSVersionLabel = TEXT("Windows Storage Server 2003");
					}
					else if (OsVersionInfo.wSuiteMask & VER_SUITE_WH_SERVER)
					{
						out_OSVersionLabel = TEXT("Windows Home Server");
					}
					else if (OsVersionInfo.wProductType == VER_NT_WORKSTATION && SystemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
					{
						out_OSVersionLabel = TEXT("Windows XP");
						out_OSSubVersionLabel = TEXT("Professional x64 Edition");
					}
					else
					{
						out_OSVersionLabel = TEXT("Windows Server 2003");
					}
					break;
				default:
					ErrorCode |= (int32)ERROR_UNKNOWNVERSION;
				}
				break;
			case 6:
				switch (OsVersionInfo.dwMinorVersion)
				{
				case 0:
					if (OsVersionInfo.wProductType == VER_NT_WORKSTATION)
					{
						out_OSVersionLabel = TEXT("Windows Vista");
					}
					else
					{
						out_OSVersionLabel = TEXT("Windows Server 2008");
					}
					break;
				case 1:
					if (OsVersionInfo.wProductType == VER_NT_WORKSTATION)
					{
						out_OSVersionLabel = TEXT("Windows 7");
					}
					else
					{
						out_OSVersionLabel = TEXT("Windows Server 2008 R2");
					}
					break;
				case 2:
					if (OsVersionInfo.wProductType == VER_NT_WORKSTATION)
					{
						out_OSVersionLabel = TEXT("Windows 8");
					}
					else
					{
						out_OSVersionLabel = TEXT("Windows Server 2012");
					}
					break;
				case 3:
					if (OsVersionInfo.wProductType == VER_NT_WORKSTATION)
					{
						out_OSVersionLabel = TEXT("Windows 8.1");
					}
					else
					{
						out_OSVersionLabel = TEXT("Windows Server 2012 R2");
					}
					break;
				default:
					ErrorCode |= (int32)ERROR_UNKNOWNVERSION;
					break;
				}
				break;
			case 10:
				switch (OsVersionInfo.dwMinorVersion)
				{
				case 0:
					if (OsVersionInfo.wProductType == VER_NT_WORKSTATION)
					{
						out_OSVersionLabel = TEXT("Windows 10");
					}
					else
					{
						out_OSVersionLabel = TEXT("Windows Server Technical Preview");
					}
					break;
				default:
					ErrorCode |= (int32)ERROR_UNKNOWNVERSION;
					break;
				}
				break;
			default:
				ErrorCode |= ERROR_UNKNOWNVERSION;
				break;
			}

			if (OsVersionInfo.dwMajorVersion >= 6)
			{
#pragma warning( push )
#pragma warning( disable: 4191 )	// unsafe conversion from 'type of expression' to 'type required'
				typedef BOOL(WINAPI *LPFN_GETPRODUCTINFO)(DWORD, DWORD, DWORD, DWORD, PDWORD);
				LPFN_GETPRODUCTINFO fnGetProductInfo = (LPFN_GETPRODUCTINFO)GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "GetProductInfo");
#pragma warning( pop )
				if (fnGetProductInfo != NULL)
				{
					DWORD Type;
					fnGetProductInfo(OsVersionInfo.dwMajorVersion, OsVersionInfo.dwMinorVersion, 0, 0, &Type);

					switch (Type)
					{
					case PRODUCT_ULTIMATE:
						out_OSSubVersionLabel = TEXT("Ultimate Edition");
						break;
					case PRODUCT_PROFESSIONAL:
						out_OSSubVersionLabel = TEXT("Professional");
						break;
					case PRODUCT_HOME_PREMIUM:
						out_OSSubVersionLabel = TEXT("Home Premium Edition");
						break;
					case PRODUCT_HOME_BASIC:
						out_OSSubVersionLabel = TEXT("Home Basic Edition");
						break;
					case PRODUCT_ENTERPRISE:
						out_OSSubVersionLabel = TEXT("Enterprise Edition");
						break;
					case PRODUCT_BUSINESS:
						out_OSSubVersionLabel = TEXT("Business Edition");
						break;
					case PRODUCT_STARTER:
						out_OSSubVersionLabel = TEXT("Starter Edition");
						break;
					case PRODUCT_CLUSTER_SERVER:
						out_OSSubVersionLabel = TEXT("Cluster Server Edition");
						break;
					case PRODUCT_DATACENTER_SERVER:
						out_OSSubVersionLabel = TEXT("Datacenter Edition");
						break;
					case PRODUCT_DATACENTER_SERVER_CORE:
						out_OSSubVersionLabel = TEXT("Datacenter Edition (core installation)");
						break;
					case PRODUCT_ENTERPRISE_SERVER:
						out_OSSubVersionLabel = TEXT("Enterprise Edition");
						break;
					case PRODUCT_ENTERPRISE_SERVER_CORE:
						out_OSSubVersionLabel = TEXT("Enterprise Edition (core installation)");
						break;
					case PRODUCT_ENTERPRISE_SERVER_IA64:
						out_OSSubVersionLabel = TEXT("Enterprise Edition for Itanium-based Systems");
						break;
					case PRODUCT_SMALLBUSINESS_SERVER:
						out_OSSubVersionLabel = TEXT("Small Business Server");
						break;
					case PRODUCT_SMALLBUSINESS_SERVER_PREMIUM:
						out_OSSubVersionLabel = TEXT("Small Business Server Premium Edition");
						break;
					case PRODUCT_STANDARD_SERVER:
						out_OSSubVersionLabel = TEXT("Standard Edition");
						break;
					case PRODUCT_STANDARD_SERVER_CORE:
						out_OSSubVersionLabel = TEXT("Standard Edition (core installation)");
						break;
					case PRODUCT_WEB_SERVER:
						out_OSSubVersionLabel = TEXT("Web Server Edition");
						break;
					}
				}
				else
				{
					out_OSSubVersionLabel = TEXT("(type unknown)");
					ErrorCode |= (int32)ERROR_GETPRODUCTINFO_FAILED;
				}
			}

#if 0
			// THIS BIT ADDS THE SERVICE PACK INFO TO THE EDITION STRING
			// Append service pack info
			if (OsVersionInfo.szCSDVersion[0] != 0)
			{
				OSSubVersionLabel += FString::Printf(TEXT(" (%s)"), OsVersionInfo.szCSDVersion);
			}
#else
			// THIS BIT USES SERVICE PACK INFO ONLY
			out_OSSubVersionLabel = OsVersionInfo.szCSDVersion;
#endif
		}
		else
		{
			ErrorCode |= ERROR_GETVERSIONEX_FAILED;
		}

	return ErrorCode;
}

FString FWindowsOSVersionHelper::GetOSVersion()
{
	int32 ErrorCode = (int32)SUCCEEDED;

	// Get system info
	SYSTEM_INFO SystemInfo;
	const TCHAR* Architecture;
	if (FPlatformMisc::Is64bitOperatingSystem())
	{
		Architecture = TEXT("64bit");
		GetNativeSystemInfo(&SystemInfo);
	}
	else
	{
		Architecture = TEXT("32bit");
		GetSystemInfo(&SystemInfo);
	}

	OSVERSIONINFOEX OsVersionInfo = { 0 };
	OsVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
#pragma warning(push)
#pragma warning(disable : 4996) // 'function' was declared deprecated
	CA_SUPPRESS(28159)
		if (GetVersionEx((LPOSVERSIONINFO)&OsVersionInfo))
#pragma warning(pop)
		{
			return FString::Printf(TEXT("%d.%d.%d.%d.%d.%s"), OsVersionInfo.dwMajorVersion, OsVersionInfo.dwMinorVersion, OsVersionInfo.dwBuildNumber, OsVersionInfo.wProductType, OsVersionInfo.wSuiteMask, Architecture);
		}
	return FString();
}

#include "Windows/HideWindowsPlatformTypes.h"

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

#include "Windows/AllowWindowsPlatformTypes.h"

void FWindowsPlatformMisc::PromptForRemoteDebugging(bool bIsEnsure)
{

}

FString FWindowsPlatformMisc::GetPrimaryGPUBrand()
{
	static FString PrimaryGPUBrand;
	if (PrimaryGPUBrand.IsEmpty())
	{
		// Find primary display adapter and get the device name.
		PrimaryGPUBrand = FGenericPlatformMisc::GetPrimaryGPUBrand();

		DISPLAY_DEVICE DisplayDevice;
		DisplayDevice.cb = sizeof(DisplayDevice);
		DWORD DeviceIndex = 0;

		while (EnumDisplayDevices(0, DeviceIndex, &DisplayDevice, 0))
		{
			if ((DisplayDevice.StateFlags & (DISPLAY_DEVICE_ATTACHED_TO_DESKTOP | DISPLAY_DEVICE_PRIMARY_DEVICE)) > 0)
			{
				PrimaryGPUBrand = DisplayDevice.DeviceString;
				break;
			}

			FMemory::Memzero(DisplayDevice);
			DisplayDevice.cb = sizeof(DisplayDevice);
			DeviceIndex++;
		}
	}

	return PrimaryGPUBrand;
}

static void GetVideoDriverDetails(const FString& Key, FGPUDriverInfo& Out)
{
	// https://msdn.microsoft.com/en-us/library/windows/hardware/ff569240(v=vs.85).aspx

	const TCHAR* DeviceDescriptionValueName = TEXT("Device Description");

	bool bDevice = FWindowsPlatformMisc::QueryRegKey(HKEY_LOCAL_MACHINE, *Key, DeviceDescriptionValueName, Out.DeviceDescription); // AMD and NVIDIA
	if (!bDevice)
	{
		bDevice = FWindowsPlatformMisc::QueryRegKey(HKEY_LOCAL_MACHINE, *Key, TEXT("DriverDesc"), Out.DeviceDescription);
	}

	if (!bDevice)
	{
		// in the case where this failed we also have:
		//  "DriverDesc"="NVIDIA GeForce GTX 670"

		// e.g. "GeForce GTX 680" (no NVIDIA prefix so no good for string comparison with DX)
		//	FWindowsPlatformMisc::QueryRegKey(HKEY_LOCAL_MACHINE, *Key, TEXT("HardwareInformation.AdapterString"), Out.DeviceDescription); // AMD and NVIDIA

		// Try again in Settings subfolder
		const FString SettingsSubKey = Key + TEXT("\\Settings");
		bDevice = FWindowsPlatformMisc::QueryRegKey(HKEY_LOCAL_MACHINE, *SettingsSubKey, DeviceDescriptionValueName, Out.DeviceDescription); // AMD and NVIDIA

		if (!bDevice)
		{
			// Neither root nor Settings subfolder contained a "Device Description" value so this is probably not a device
			Out = FGPUDriverInfo();
			return;
		}
	}

	// some key/value pairs explained: http://www.helpdoc-online.com/SCDMS01EN1A330P306~Windows-NT-Workstation-3.51-Resource-Kit-Help-en~Video-Device-Driver-Entries.htm

	FWindowsPlatformMisc::QueryRegKey(HKEY_LOCAL_MACHINE, *Key, TEXT("ProviderName"), Out.ProviderName);

	if (!Out.ProviderName.IsEmpty())
	{
		if (Out.ProviderName.Find(TEXT("NVIDIA")) != INDEX_NONE)
		{
			Out.SetNVIDIA();
		}
		else if (Out.ProviderName.Find(TEXT("Advanced Micro Devices")) != INDEX_NONE)
		{
			Out.SetAMD();
		}
		else if (Out.ProviderName.Find(TEXT("Intel")) != INDEX_NONE)	// usually TEXT("Intel Corporation")
		{
			Out.SetIntel();
		}
	}

	// technical driver version, AMD and NVIDIA
	FWindowsPlatformMisc::QueryRegKey(HKEY_LOCAL_MACHINE, *Key, TEXT("DriverVersion"), Out.InternalDriverVersion);

	Out.UserDriverVersion = Out.InternalDriverVersion;

	if (Out.IsNVIDIA())
	{
		Out.UserDriverVersion = Out.GetUnifiedDriverVersion();
	}
	else if (Out.IsAMD())
	{
		if (FWindowsPlatformMisc::QueryRegKey(HKEY_LOCAL_MACHINE, *Key, TEXT("Catalyst_Version"), Out.UserDriverVersion))
		{
			Out.UserDriverVersion = FString(TEXT("Catalyst ")) + Out.UserDriverVersion;
		}

		FString Edition;
		if (FWindowsPlatformMisc::QueryRegKey(HKEY_LOCAL_MACHINE, *Key, TEXT("RadeonSoftwareEdition"), Edition))
		{
			FString Version;
			if (FWindowsPlatformMisc::QueryRegKey(HKEY_LOCAL_MACHINE, *Key, TEXT("RadeonSoftwareVersion"), Version))
			{
				// e.g. TEXT("Crimson 15.12") or TEXT("Catalyst 14.1")
				Out.UserDriverVersion = Edition + TEXT(" ") + Version;
			}
		}
	}

	// AMD and NVIDIA
	FWindowsPlatformMisc::QueryRegKey(HKEY_LOCAL_MACHINE, *Key, TEXT("DriverDate"), Out.DriverDate);
}

struct FGPUDriverInfo FWindowsPlatformMisc::GetGPUDriverInfo(const FString& DeviceDescription)
{
	// to distinguish failed GetGPUDriverInfo() from call to GetGPUDriverInfo()
	FGPUDriverInfo Ret;

	Ret.InternalDriverVersion = TEXT("Unknown");
	Ret.UserDriverVersion = TEXT("Unknown");
	Ret.DriverDate = TEXT("Unknown");

	// for debugging, useful even in shipping to see what went wrong
	FString DebugString;

	uint32 FoundDriverCount = 0;

	int32 Method = 4/*CVarDriverDetectionMethod.GetValueOnGameThread()*/;

	if (Method == 3 || Method == 4)
	{
		UE_LOG(LogWindows, Log, TEXT("EnumDisplayDevices:"));

		for (uint32 i = 0; i < 256; ++i)
		{
			DISPLAY_DEVICE Device;

			ZeroMemory(&Device, sizeof(Device));
			Device.cb = sizeof(Device);

			// see https://msdn.microsoft.com/en-us/library/windows/desktop/dd162609(v=vs.85).aspx
			if (EnumDisplayDevices(0, i, &Device, EDD_GET_DEVICE_INTERFACE_NAME) == 0)
			{
				// last device or error
				break;
			}

			UE_LOG(LogWindows, Log, TEXT("   %d. '%s' (P:%d D:%d)"),
				i,
				Device.DeviceString,
				(Device.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE) != 0,
				(Device.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP) != 0
			);

			if (Method == 3)
			{
				if (!(Device.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE))
				{
					// see http://www.vistaheads.com/forums/microsoft-public-windows-vista-hardware-devices/286017-find-out-active-graphics-device-programmatically-registry-key.html
					DebugString += TEXT("JumpOverNonPrimary ");
					// we want the primary device
					continue;
				}
			}

			FString DriverLocation = Device.DeviceKey;

			if (DriverLocation.Left(18) == TEXT("\\Registry\\Machine\\"))		// not case sensitive
			{
				DriverLocation = FString(TEXT("\\HKEY_LOCAL_MACHINE\\")) + DriverLocation.RightChop(18);
			}
			if (DriverLocation.Left(20) == TEXT("\\HKEY_LOCAL_MACHINE\\"))		// not case sensitive
			{
				FString DriverKey = DriverLocation.RightChop(20);

				FGPUDriverInfo Local;
				GetVideoDriverDetails(DriverKey, Local);

				if (!Local.IsValid())
				{
					DebugString += TEXT("GetVideoDriverDetailsInvalid ");
				}

				if ((Method == 3) || (Local.DeviceDescription == DeviceDescription))
				{
					if (!FoundDriverCount)
					{
						Ret = Local;
					}
					++FoundDriverCount;
				}
				else
				{
					DebugString += TEXT("PrimaryIsNotTheChoosenAdapter ");
				}
			}
			else
			{
				DebugString += TEXT("PrimaryDriverLocationFailed ");
			}
		}

		if (FoundDriverCount != 1)
		{
			// We assume if multiple entries are found they are all the same driver. If that is correct - this is no error.
			DebugString += FString::Printf(TEXT("FoundDriverCount:%d "), FoundDriverCount);
		}

		if (!DebugString.IsEmpty())
		{
			UE_LOG(LogWindows, Log, TEXT("DebugString: %s"), *DebugString);
		}

		return Ret;
	}

	const bool bIterateAvailableAndChoose = Method == 0;

	if (bIterateAvailableAndChoose)
	{
		for (uint32 i = 0; i < 256; ++i)
		{
			// Iterate all installed display adapters
			const FString DriverNKey = FString::Printf(TEXT("SYSTEM\\CurrentControlSet\\Control\\Class\\{4D36E968-E325-11CE-BFC1-08002BE10318}\\%04d"), i);

			FGPUDriverInfo Local;
			GetVideoDriverDetails(DriverNKey, Local);

			if (!Local.IsValid())
			{
				// last device or error
				DebugString += TEXT("GetVideoDriverDetailsInvalid ");
				break;
			}

			if (Local.DeviceDescription == DeviceDescription)
			{
				// found the one we are searching for
				Ret = Local;
				++FoundDriverCount;
				break;
			}
		}
	}

	// FoundDriverCount can be != 1, we take the primary adapater (can be from upgrading a machine to a new OS or old drivers) which also might be wrong
	// see: http://win7settings.blogspot.com/2014/10/how-to-extract-installed-drivers-from.html
	// https://support.microsoft.com/en-us/kb/200435
	// http://www.experts-exchange.com/questions/10198207/Windows-NT-Display-adapter-information.html
	// alternative: from https://support.microsoft.com/en-us/kb/200435
	if (FoundDriverCount != 1)
	{
		// we start again, this time we only look at the primary adapter
		Ret.InternalDriverVersion = TEXT("Unknown");
		Ret.UserDriverVersion = TEXT("Unknown");
		Ret.DriverDate = TEXT("Unknown");

		if (bIterateAvailableAndChoose)
		{
			DebugString += FString::Printf(TEXT("FoundDriverCount:%d FallbackToPrimary "), FoundDriverCount);
		}

		FString DriverLocation; // e.g. HKEY_LOCAL_MACHINE\System\CurrentControlSet\Services\<videodriver>\Device0
		// Video0 is the first logical one, not neccesarily the primary, would have to iterate multiple to get the right one (see https://support.microsoft.com/en-us/kb/102992)
		bool bOk = FWindowsPlatformMisc::QueryRegKey(HKEY_LOCAL_MACHINE, TEXT("HARDWARE\\DEVICEMAP\\VIDEO"), TEXT("\\Device\\Video0"), /*out*/ DriverLocation);

		if (bOk)
		{
			if (DriverLocation.Left(18) == TEXT("\\Registry\\Machine\\"))		// not case sensitive
			{
				DriverLocation = FString(TEXT("\\HKEY_LOCAL_MACHINE\\")) + DriverLocation.RightChop(18);
			}
			if (DriverLocation.Left(20) == TEXT("\\HKEY_LOCAL_MACHINE\\"))		// not case sensitive
			{
				FString DriverLocationKey = DriverLocation.RightChop(20);

				FGPUDriverInfo Local;
				GetVideoDriverDetails(DriverLocationKey, Local);

				if (!Local.IsValid())
				{
					DebugString += TEXT("GetVideoDriverDetailsInvalid ");
				}

				if (Local.DeviceDescription == DeviceDescription)
				{
					Ret = Local;
					FoundDriverCount = 1;
				}
				else
				{
					DebugString += TEXT("PrimaryIsNotTheChoosenAdapter ");
				}
			}
			else
			{
				DebugString += TEXT("PrimaryDriverLocationFailed ");
			}
		}
		else
		{
			DebugString += TEXT("QueryForPrimaryFailed ");
		}
	}

	if (!DebugString.IsEmpty())
	{
		UE_LOG(LogWindows, Log, TEXT("DebugString: %s"), *DebugString);
	}

	return Ret;
}

#include "Windows/HideWindowsPlatformTypes.h"

void FWindowsPlatformMisc::GetOSVersions(FString& out_OSVersionLabel, FString& out_OSSubVersionLabel)
{
	static FString OSVersionLabel;
	static FString OSSubVersionLabel;

	if (OSVersionLabel.IsEmpty() && OSSubVersionLabel.IsEmpty())
	{
		FWindowsOSVersionHelper::GetOSVersions(OSVersionLabel, OSSubVersionLabel);
	}

	out_OSVersionLabel = OSVersionLabel;
	out_OSSubVersionLabel = OSSubVersionLabel;
}

FString FWindowsPlatformMisc::GetOSVersion()
{
	static FString CachedOSVersion = FWindowsOSVersionHelper::GetOSVersion();
	return CachedOSVersion;
}


bool FWindowsPlatformMisc::QueryRegKey(const Windows::HKEY InKey, const TCHAR* InSubKey, const TCHAR* InValueName, FString& OutData)
{
	bool bSuccess = false;

	// Redirect key depending on system
	for (int32 RegistryIndex = 0; RegistryIndex < 2 && !bSuccess; ++RegistryIndex)
	{
		HKEY Key = 0;
		const uint32 RegFlags = (RegistryIndex == 0) ? KEY_WOW64_32KEY : KEY_WOW64_64KEY;
		if (RegOpenKeyEx(InKey, InSubKey, 0, KEY_READ | RegFlags, &Key) == ERROR_SUCCESS)
		{
			::DWORD Size = 0;
			// First, we'll call RegQueryValueEx to find out how large of a buffer we need
			if ((RegQueryValueEx(Key, InValueName, NULL, NULL, NULL, &Size) == ERROR_SUCCESS) && Size)
			{
				// Allocate a buffer to hold the value and call the function again to get the data
				char *Buffer = new char[Size];
				if (RegQueryValueEx(Key, InValueName, NULL, NULL, (LPBYTE)Buffer, &Size) == ERROR_SUCCESS)
				{
					OutData = FString(Size - 1, (TCHAR*)Buffer);
					OutData.TrimToNullTerminator();
					bSuccess = true;
				}
				delete[] Buffer;
			}
			RegCloseKey(Key);
		}
	}

	return bSuccess;
}

int32 FWindowsPlatformMisc::GetCacheLineSize()
{
	return FCPUIDQueriedData::GetCacheLineSize();
}

#if !UE_BUILD_SHIPPING
bool FWindowsPlatformMisc::IsDebuggerPresent()
{
	return /*!GIgnoreDebugger &&*/ !!::IsDebuggerPresent();
}

#endif

void FWindowsPlatformMisc::RequestExit(bool Force)
{
	UE_LOG(LogWindows, Log, TEXT("FPlatformMisc::RequestExit(%i)"), Force);

	//FCoreDelegates::ApplicationWillTerminateDelegate.Broadcast();

	if (Force)
	{
		// Force immediate exit. In case of an error set the exit code to 3.
		// Dangerous because config code isn't flushed, global destructors aren't called, etc.
		// Suppress abort message and MS reports.
		//_set_abort_behavior( 0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT );
		//abort();

		// Make sure the log is flushed.
// 		if (GLog)
// 		{
// 			// This may be called from other thread, so set this thread as the master.
// 			GLog->SetCurrentThreadAsMasterThread();
// 			GLog->TearDown();
// 		}

		//TerminateProcess(GetCurrentProcess(), GIsCriticalError ? 3 : 0);
		TerminateProcess(GetCurrentProcess(), 0);
	}
	else
	{
		// Tell the platform specific code we want to exit cleanly from the main loop.
		PostQuitMessage(0);
		GIsRequestingExit = 1;
	}
}

void FWindowsPlatformMisc::CreateGuid(struct FGuid& Result)
{
	verify(CoCreateGuid((GUID*)&Result) == S_OK);
}

bool FWindowsPlatformMisc::Is64bitOperatingSystem()
{
#if PLATFORM_64BITS
	return true;
#else
#pragma warning( push )
#pragma warning( disable: 4191 )	// unsafe conversion from 'type of expression' to 'type required'
	typedef BOOL(WINAPI *LPFN_ISWOW64PROCESS)(HANDLE, PBOOL);
	LPFN_ISWOW64PROCESS fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(GetModuleHandle(TEXT("kernel32")), "IsWow64Process");
	BOOL bIsWoW64Process = 0;
	if (fnIsWow64Process != NULL)
	{
		if (fnIsWow64Process(GetCurrentProcess(), &bIsWoW64Process) == 0)
		{
			bIsWoW64Process = 0;
		}
	}
#pragma warning( pop )
	return bIsWoW64Process == 1;
#endif
}

int32 FWindowsPlatformMisc::NumberOfCores()
{
	static int32 CoreCount = 0;
	if (CoreCount == 0)
	{
		/*if (FParse::Param(FCommandLine::Get(), TEXT("usehyperthreading")))*/
		if (false)
		{
			CoreCount = NumberOfCoresIncludingHyperthreads();
		}
		else
		{
			// Get only physical cores
			PSYSTEM_LOGICAL_PROCESSOR_INFORMATION InfoBuffer = NULL;
			::DWORD BufferSize = 0;

			// Get the size of the buffer to hold processor information.
			::BOOL Result = GetLogicalProcessorInformation(InfoBuffer, &BufferSize);
			check(!Result && GetLastError() == ERROR_INSUFFICIENT_BUFFER);
			check(BufferSize > 0);

			// Allocate the buffer to hold the processor info.
			InfoBuffer = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION)FMemory::Malloc(BufferSize);
			check(InfoBuffer);

			// Get the actual information.
			Result = GetLogicalProcessorInformation(InfoBuffer, &BufferSize);
			check(Result);

			// Count physical cores
			const int32 InfoCount = (int32)(BufferSize / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION));
			for (int32 Index = 0; Index < InfoCount; ++Index)
			{
				SYSTEM_LOGICAL_PROCESSOR_INFORMATION* Info = &InfoBuffer[Index];
				if (Info->Relationship == RelationProcessorCore)
				{
					CoreCount++;
				}
			}
			FMemory::Free(InfoBuffer);
		}
	}
	return CoreCount;
}

int32 FWindowsPlatformMisc::NumberOfCoresIncludingHyperthreads()
{
	static int32 CoreCount = 0;
	if (CoreCount == 0)
	{
		// Get the number of logical processors, including hyperthreaded ones.
		SYSTEM_INFO SI;
		GetSystemInfo(&SI);
		CoreCount = (int32)SI.dwNumberOfProcessors;
	}
	return CoreCount;
}

int32 FWindowsPlatformMisc::NumberOfWorkerThreadsToSpawn()
{
	static int32 MaxServerWorkerThreads = 4;
	static int32 MaxWorkerThreads = 26;

	int32 NumberOfCores = FWindowsPlatformMisc::NumberOfCores();
	int32 NumberOfCoresIncludingHyperthreads = FWindowsPlatformMisc::NumberOfCoresIncludingHyperthreads();
	int32 NumberOfThreads = 0;

	if (NumberOfCoresIncludingHyperthreads > NumberOfCores)
	{
		NumberOfThreads = NumberOfCoresIncludingHyperthreads - 2;
	}
	else
	{
		NumberOfThreads = NumberOfCores - 1;
	}

	int32 MaxWorkerThreadsWanted = IsRunningDedicatedServer() ? MaxServerWorkerThreads : MaxWorkerThreads;
	// need to spawn at least one worker thread (see FTaskGraphImplementation)
	return FMath::Max(FMath::Min(NumberOfThreads, MaxWorkerThreadsWanted), 1);
}

uint32 FWindowsPlatformMisc::GetLastError()
{
	return (uint32)::GetLastError();
}
