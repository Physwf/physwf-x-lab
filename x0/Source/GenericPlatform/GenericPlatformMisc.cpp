#include "GenericPlatform/GenericPlatformMisc.h"
#include "Misc/AssertionMacros.h"
//#include "HAL/PlatformFilemanager.h"
#include "Math/XMathUtility.h"
#include "HAL/UnrealMemory.h"
#include "Containers/Array.h"
#include "Containers/UnrealString.h"
#include "Logging/LogMacros.h"
#include "CoreGlobals.h"
//#include "Misc/Parse.h"
//#include "Misc/CommandLine.h"
#include "Misc/Paths.h"
//#include "Internationalization/Text.h"
//#include "Internationalization/Internationalization.h"
//#include "Misc/Guid.h"
#include "Math/Color.h"
//#include "GenericPlatform/GenericPlatformCompression.h"
//#include "Misc/ConfigCacheIni.h"
#include "Misc/App.h"
//#include "GenericPlatform/GenericPlatformChunkInstall.h"
//#include "HAL/FileManagerGeneric.h"
//#include "Misc/VarargsHelper.h"
//#include "Misc/SecureHash.h"
//#include "HAL/ExceptionHandling.h"
//#include "GenericPlatform/GenericPlatformCrashContext.h"
//#include "GenericPlatform/GenericPlatformDriver.h"
//#include "ProfilingDebugging/ExternalProfiler.h"
#//include "HAL/LowLevelMemTracker.h"
#include "Templates/Function.h"

//#include "Misc/UProjectInfo.h"
//#include "Internationalization/Culture.h"

#if UE_ENABLE_ICU
THIRD_PARTY_INCLUDES_START
#include <unicode/locid.h>
THIRD_PARTY_INCLUDES_END
#endif

FString FGenericPlatformMisc::GetPrimaryGPUBrand()
{
	return FString(TEXT("GenericGPUBrand"));
}

#if !UE_BUILD_SHIPPING
bool FGenericPlatformMisc::bShouldPromptForRemoteDebugging = false;
bool FGenericPlatformMisc::bPromptForRemoteDebugOnEnsure = false;
#endif	

const TCHAR* FGenericPlatformMisc::RootDir()
{
	static FString Path;
	if (Path.Len() == 0)
	{
		FString TempPath = FPaths::EngineDir();
		int32 chopPos = TempPath.Find(TEXT("/Engine"), ESearchCase::IgnoreCase, ESearchDir::FromEnd);
		if (chopPos != INDEX_NONE)
		{
			TempPath = TempPath.Left(chopPos + 1);
		}
		else
		{
			TempPath = FPlatformProcess::BaseDir();

			// if the path ends in a separator, remove it
			if (TempPath.Right(1) == TEXT("/"))
			{
				TempPath = TempPath.LeftChop(1);
			}

			// keep going until we've removed Binaries
#if IS_MONOLITHIC && !IS_PROGRAM
			int32 pos = TempPath.Find(*FString::Printf(TEXT("/%s/Binaries"), FApp::GetProjectName()));
#else
			int32 pos = TempPath.Find(TEXT("/Engine/Binaries"), ESearchCase::IgnoreCase, ESearchDir::FromEnd);
#endif
			if (pos != INDEX_NONE)
			{
				TempPath = TempPath.Left(pos + 1);
			}
			else
			{
				pos = TempPath.Find(TEXT("/../Binaries"), ESearchCase::IgnoreCase, ESearchDir::FromEnd);
				if (pos != INDEX_NONE)
				{
					TempPath = TempPath.Left(pos + 1) + TEXT("../../");
				}
				else
				{
					while (TempPath.Len() && TempPath.Right(1) != TEXT("/"))
					{
						TempPath = TempPath.LeftChop(1);
					}
				}
			}
		}

		Path = FPaths::ConvertRelativePathToFull(TempPath);
		FPaths::RemoveDuplicateSlashes(Path);
	}
	return *Path;
}
