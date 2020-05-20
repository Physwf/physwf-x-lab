#include "GenericPlatform/GenericPlatformMisc.h"
#include "Misc/AssertionMacros.h"
//#include "HAL/PlatformFilemanager.h"
#include "Math/XMathUtility.h"
#include "HAL/UnrealMemory.h"
#include "Containers/Array.h"
#include "Containers/UnrealString.h"
#include "Logging/LogMacros.h"
#include "CoreGlobals.h"
#include "Misc/Parse.h"
#include "Misc/CommandLine.h"
#include "Misc/Paths.h"
//#include "Internationalization/Text.h"
//#include "Internationalization/Internationalization.h"
//#include "Misc/Guid.h"
#include "Math/Color.h"
//#include "GenericPlatform/GenericPlatformCompression.h"
//#include "Misc/ConfigCacheIni.h"
#include "Misc/App.h"
//#include "GenericPlatform/GenericPlatformChunkInstall.h"
#include "HAL/FileManagerGeneric.h"
#include "Misc/VarargsHelper.h"
#include "Misc/SecureHash.h"
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

void FGenericPlatformMisc::LowLevelOutputDebugString(const TCHAR *Message)
{
	FPlatformMisc::LocalPrint(Message);
}

void VARARGS FGenericPlatformMisc::LowLevelOutputDebugStringf(const TCHAR *Fmt, ...)
{
	GROWABLE_LOGF(
		FPlatformMisc::LowLevelOutputDebugString(Buffer);
	);
}

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

const TCHAR* FGenericPlatformMisc::ProjectDir()
{
#if 0
	static FString ProjectDir = TEXT("");

	// track if last time we called this function the .ini was ready and had fixed the GameName case
	static bool bWasIniReady = false;
	bool bIsIniReady = GConfig && GConfig->IsReadyForUse();
	if (bWasIniReady != bIsIniReady)
	{
		ProjectDir = TEXT("");
		bWasIniReady = bIsIniReady;
	}

	// try using the override game dir if it exists, which will override all below logic
	if (ProjectDir.Len() == 0)
	{
		ProjectDir = OverrideProjectDir;
	}

	if (ProjectDir.Len() == 0)
	{
		if (FPlatformProperties::IsProgram())
		{
			// monolithic, game-agnostic executables, the ini is in Engine/Config/Platform
			ProjectDir = FString::Printf(TEXT("../../../Engine/Programs/%s/"), FApp::GetProjectName());
		}
		else
		{
			if (FPaths::IsProjectFilePathSet())
			{
				GenericPlatformMisc_GetProjectFilePathProjectDir(ProjectDir);
			}
			else if (FApp::HasProjectName())
			{
				if (FPlatformProperties::IsMonolithicBuild() == false)
				{
					// No game project file, but has a game name, use the game folder next to the working directory
					ProjectDir = FString::Printf(TEXT("../../../%s/"), FApp::GetProjectName());
					FString GameBinariesDir = ProjectDir / TEXT("Binaries/");
					if (FPlatformFileManager::Get().GetPlatformFile().DirectoryExists(*GameBinariesDir) == false)
					{
						// The game binaries folder was *not* found
						// 
						FPlatformMisc::LowLevelOutputDebugStringf(TEXT("Failed to find game directory: %s\n"), *ProjectDir);

						// Use the uprojectdirs
						FString GameProjectFile = FUProjectDictionary::GetDefault().GetRelativeProjectPathForGame(FApp::GetProjectName(), FPlatformProcess::BaseDir());
						if (GameProjectFile.IsEmpty() == false)
						{
							// We found a project folder for the game
							FPaths::SetProjectFilePath(GameProjectFile);
							ProjectDir = FPaths::GetPath(GameProjectFile);
							if (ProjectDir.EndsWith(TEXT("/")) == false)
							{
								ProjectDir += TEXT("/");
							}
						}
					}
				}
				else
				{
#if !PLATFORM_DESKTOP
					ProjectDir = FString::Printf(TEXT("../../../%s/"), FApp::GetProjectName());
#else
					// This assumes the game executable is in <GAME>/Binaries/<PLATFORM>
					ProjectDir = TEXT("../../");

					// Determine a relative path that includes the game folder itself, if possible...
					FString LocalBaseDir = FString(FPlatformProcess::BaseDir());
					FString LocalRootDir = FPaths::RootDir();
					FString BaseToRoot = LocalRootDir;
					FPaths::MakePathRelativeTo(BaseToRoot, *LocalBaseDir);
					FString LocalProjectDir = LocalBaseDir / TEXT("../../");
					FPaths::CollapseRelativeDirectories(LocalProjectDir);
					FPaths::MakePathRelativeTo(LocalProjectDir, *(FPaths::RootDir()));
					LocalProjectDir = BaseToRoot / LocalProjectDir;
					if (LocalProjectDir.EndsWith(TEXT("/")) == false)
					{
						LocalProjectDir += TEXT("/");
					}

					FString CheckLocal = FPaths::ConvertRelativePathToFull(LocalProjectDir);
					FString CheckGame = FPaths::ConvertRelativePathToFull(ProjectDir);
					if (CheckLocal == CheckGame)
					{
						ProjectDir = LocalProjectDir;
					}

					if (ProjectDir.EndsWith(TEXT("/")) == false)
					{
						ProjectDir += TEXT("/");
					}
#endif
				}
			}
			else
			{
				// Get a writable engine directory
				ProjectDir = FPaths::EngineUserDir();
				FPaths::NormalizeFilename(ProjectDir);
				ProjectDir = FFileManagerGeneric::DefaultConvertToRelativePath(*ProjectDir);
				if (!ProjectDir.EndsWith(TEXT("/"))) ProjectDir += TEXT("/");
			}
		}
	}

	return *ProjectDir;
#endif
}
