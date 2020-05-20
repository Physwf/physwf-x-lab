// Core includes.
#include "Misc/Paths.h"
//#include "UObject/NameTypes.h"
#include "Logging/LogMacros.h"
#include "HAL/FileManager.h"
#include "HAL/PlatformFile.h"
#include "Misc/Parse.h"
#include "Misc/ScopeLock.h"
//#include "Misc/CommandLine.h"
//#include "Internationalization/Text.h"
//#include "Internationalization/Internationalization.h"
#include "Misc/Guid.h"
//#include "Misc/ConfigCacheIni.h"
#include "Misc/App.h"
//#include "Misc/DataDrivenPlatformInfoRegistry.h"
//#include "Misc/EngineVersion.h"

#include "HAL/PlatformProperties.h"

namespace UE4Paths_Private
{
	auto IsSlashOrBackslash = [](TCHAR C) { return C == TEXT('/') || C == TEXT('\\'); };
	auto IsNotSlashOrBackslash = [](TCHAR C) { return C != TEXT('/') && C != TEXT('\\'); };



	FString ConvertRelativePathToFullInternal(FString&& BasePath, FString&& InPath)
	{
		FString FullyPathed;
		if (FPaths::IsRelative(InPath))
		{
			FullyPathed = MoveTemp(BasePath);
			FullyPathed /= MoveTemp(InPath);
		}
		else
		{
			FullyPathed = MoveTemp(InPath);
		}

		FPaths::NormalizeFilename(FullyPathed);
		FPaths::CollapseRelativeDirectories(FullyPathed);

		if (FullyPathed.Len() == 0)
		{
			// Empty path is not absolute, and '/' is the best guess across all the platforms.
			// This substituion is not valid for Windows of course; however CollapseRelativeDirectories() will not produce an empty
			// absolute path on Windows as it takes care not to remove the drive letter.
			FullyPathed = TEXT("/");
		}

		return FullyPathed;
	}




}


FString FPaths::LaunchDir()
{
	return FString(FPlatformMisc::LaunchDir());
}

FString FPaths::EngineDir()
{
	return FString(FPlatformMisc::EngineDir());
}

FString FPaths::EngineContentDir()
{
	return FPaths::EngineDir() + TEXT("Content/");
}

FString FPaths::EngineConfigDir()
{
	return FPaths::EngineDir() + TEXT("Config/");
}

FString FPaths::EngineIntermediateDir()
{
	return FPaths::EngineDir() + TEXT("Intermediate/");
}

FString FPaths::EngineSavedDir()
{
	return EngineUserDir() + TEXT("Saved/");
}

FString FPaths::EnginePluginsDir()
{
	return FPaths::EngineDir() + TEXT("Plugins/");
}

FString FPaths::EnterpriseDir()
{
	return FPaths::RootDir() + TEXT("Enterprise/");
}

FString FPaths::EnterprisePluginsDir()
{
	return EnterpriseDir() + TEXT("Plugins/");
}

FString FPaths::EnterpriseFeaturePackDir()
{
	return FPaths::EnterpriseDir() + TEXT("FeaturePacks/");
}

FString FPaths::RootDir()
{
	return FString(FPlatformMisc::RootDir());
}

FString FPaths::ProjectDir()
{
	return FString(FPlatformMisc::ProjectDir());
}

bool FPaths::IsDrive(const FString& InPath)
{
	FString ConvertedPathString = InPath;

	ConvertedPathString = ConvertedPathString.Replace(TEXT("/"), TEXT("\\"), ESearchCase::CaseSensitive);
	const TCHAR* ConvertedPath = *ConvertedPathString;

	// Does Path refer to a drive letter or BNC path?
	if (ConvertedPath[0] == TCHAR(0))
	{
		return true;
	}
	else if (FChar::ToUpper(ConvertedPath[0]) != FChar::ToLower(ConvertedPath[0]) && ConvertedPath[1] == TEXT(':') && ConvertedPath[2] == 0)
	{
		return true;
	}
	else if (FCString::Strcmp(ConvertedPath, TEXT("\\")) == 0)
	{
		return true;
	}
	else if (FCString::Strcmp(ConvertedPath, TEXT("\\\\")) == 0)
	{
		return true;
	}
	else if (ConvertedPath[0] == TEXT('\\') && ConvertedPath[1] == TEXT('\\') && !FCString::Strchr(ConvertedPath + 2, TEXT('\\')))
	{
		return true;
	}
	else
	{
		// Need to handle cases such as X:\A\B\..\..\C\..
		// This assumes there is no actual filename in the path (ie, not c:\DIR\File.ext)!
		FString TempPath(ConvertedPath);
		// Make sure there is a '\' at the end of the path
		if (TempPath.Find(TEXT("\\"), ESearchCase::CaseSensitive, ESearchDir::FromEnd) != (TempPath.Len() - 1))
		{
			TempPath += TEXT("\\");
		}

		FString CheckPath = TEXT("");
		int32 ColonSlashIndex = TempPath.Find(TEXT(":\\"), ESearchCase::CaseSensitive);
		if (ColonSlashIndex != INDEX_NONE)
		{
			// Remove the 'X:\' from the start
			CheckPath = TempPath.Right(TempPath.Len() - ColonSlashIndex - 2);
		}
		else
		{
			// See if the first two characters are '\\' to handle \\Server\Foo\Bar cases
			if (TempPath.StartsWith(TEXT("\\\\"), ESearchCase::CaseSensitive) == true)
			{
				CheckPath = TempPath.Right(TempPath.Len() - 2);
				// Find the next slash
				int32 SlashIndex = CheckPath.Find(TEXT("\\"), ESearchCase::CaseSensitive);
				if (SlashIndex != INDEX_NONE)
				{
					CheckPath = CheckPath.Right(CheckPath.Len() - SlashIndex - 1);
				}
				else
				{
					CheckPath = TEXT("");
				}
			}
		}

		if (CheckPath.Len() > 0)
		{
			// Replace any remaining '\\' instances with '\'
			CheckPath.Replace(TEXT("\\\\"), TEXT("\\"), ESearchCase::CaseSensitive);

			int32 CheckCount = 0;
			int32 SlashIndex = CheckPath.Find(TEXT("\\"), ESearchCase::CaseSensitive);
			while (SlashIndex != INDEX_NONE)
			{
				FString FolderName = CheckPath.Left(SlashIndex);
				if (FolderName == TEXT(".."))
				{
					// It's a relative path, so subtract one from the count
					CheckCount--;
				}
				else
				{
					// It's a real folder, so add one to the count
					CheckCount++;
				}
				CheckPath = CheckPath.Right(CheckPath.Len() - SlashIndex - 1);
				SlashIndex = CheckPath.Find(TEXT("\\"), ESearchCase::CaseSensitive);
			}

			if (CheckCount <= 0)
			{
				// If there were the same number or greater relative to real folders, it's the root dir
				return true;
			}
		}
	}

	// It's not a drive...
	return false;
}

void FPaths::NormalizeDirectoryName(FString& InPath)
{
	InPath.ReplaceInline(TEXT("\\"), TEXT("/"), ESearchCase::CaseSensitive);
	if (InPath.EndsWith(TEXT("/"), ESearchCase::CaseSensitive) && !InPath.EndsWith(TEXT("//"), ESearchCase::CaseSensitive) && !InPath.EndsWith(TEXT(":/"), ESearchCase::CaseSensitive))
	{
		// overwrite trailing slash with terminator
		InPath.GetCharArray()[InPath.Len() - 1] = 0;
		// shrink down
		InPath.TrimToNullTerminator();
	}

	FPlatformMisc::NormalizePath(InPath);
}


FString FPaths::ConvertRelativePathToFull(const FString& InPath)
{
	return UE4Paths_Private::ConvertRelativePathToFullInternal(FString(FPlatformProcess::BaseDir()), FString(InPath));
}

FString FPaths::ConvertRelativePathToFull(FString&& InPath)
{
	return UE4Paths_Private::ConvertRelativePathToFullInternal(FString(FPlatformProcess::BaseDir()), MoveTemp(InPath));
}

FString FPaths::ConvertRelativePathToFull(const FString& BasePath, const FString& InPath)
{
	return UE4Paths_Private::ConvertRelativePathToFullInternal(CopyTemp(BasePath), CopyTemp(InPath));
}

FString FPaths::ConvertRelativePathToFull(const FString& BasePath, FString&& InPath)
{
	return UE4Paths_Private::ConvertRelativePathToFullInternal(CopyTemp(BasePath), MoveTemp(InPath));
}

FString FPaths::ConvertRelativePathToFull(FString&& BasePath, const FString& InPath)
{
	return UE4Paths_Private::ConvertRelativePathToFullInternal(MoveTemp(BasePath), CopyTemp(InPath));
}

FString FPaths::ConvertRelativePathToFull(FString&& BasePath, FString&& InPath)
{
	return UE4Paths_Private::ConvertRelativePathToFullInternal(MoveTemp(BasePath), MoveTemp(InPath));
}

void FPaths::RemoveDuplicateSlashes(FString& InPath)
{
	while (InPath.Contains(TEXT("//"), ESearchCase::CaseSensitive))
	{
		InPath = InPath.Replace(TEXT("//"), TEXT("/"), ESearchCase::CaseSensitive);
	}
}

FString FPaths::ScreenShotDir()
{
	return FPaths::ProjectSavedDir() + TEXT("Screenshots/") + FPlatformProperties::PlatformName() + TEXT("/");
}

FString FPaths::ProjectLogDir()
{
#if PLATFORM_PS4

	const FString* OverrideDir = FPS4PlatformFile::GetOverrideLogDirectory();
	if (OverrideDir != nullptr)
	{
		return *OverrideDir;
	}

#endif

#if PLATFORM_MAC || PLATFORM_XBOXONE
	if (UE4Paths_Private::CustomUserDirArgument().IsEmpty())
	{
		return FPlatformProcess::UserLogsDir();
	}
#endif

	return FPaths::ProjectSavedDir() + TEXT("Logs/");
}


FString FPaths::GetCleanFilename(const FString& InPath)
{
	static_assert(INDEX_NONE == -1, "INDEX_NONE assumed to be -1");

	int32 EndPos = InPath.FindLastCharByPredicate(UE4Paths_Private::IsNotSlashOrBackslash) + 1;
	int32 StartPos = InPath.FindLastCharByPredicate(UE4Paths_Private::IsSlashOrBackslash, EndPos) + 1;

	FString Result = InPath.Mid(StartPos, EndPos - StartPos);
	return Result;
}

FString FPaths::GetCleanFilename(FString&& InPath)
{
	static_assert(INDEX_NONE == -1, "INDEX_NONE assumed to be -1");

	int32 EndPos = InPath.FindLastCharByPredicate(UE4Paths_Private::IsNotSlashOrBackslash) + 1;
	int32 StartPos = InPath.FindLastCharByPredicate(UE4Paths_Private::IsSlashOrBackslash, EndPos) + 1;

	InPath.RemoveAt(EndPos, InPath.Len() - EndPos, false);
	InPath.RemoveAt(0, StartPos, false);

	return MoveTemp(InPath);
}

FString FPaths::GetPath(const FString& InPath)
{
	int32 Pos = InPath.FindLastCharByPredicate(UE4Paths_Private::IsSlashOrBackslash);

	FString Result;
	if (Pos != INDEX_NONE)
	{
		Result = InPath.Left(Pos);
	}

	return Result;
}

FString FPaths::GetPath(FString&& InPath)
{
	int32 Pos = InPath.FindLastCharByPredicate(UE4Paths_Private::IsSlashOrBackslash);

	FString Result;
	if (Pos != INDEX_NONE)
	{
		InPath.RemoveAt(Pos, InPath.Len() - Pos, false);
		Result = MoveTemp(InPath);
	}

	return Result;
}

void FPaths::NormalizeFilename(FString& InPath)
{
	InPath.ReplaceInline(TEXT("\\"), TEXT("/"), ESearchCase::CaseSensitive);

	FPlatformMisc::NormalizePath(InPath);
}

bool FPaths::MakePathRelativeTo(FString& InPath, const TCHAR* InRelativeTo)
{
	FString Target = FPaths::ConvertRelativePathToFull(InPath);
	FString Source = FPaths::ConvertRelativePathToFull(InRelativeTo);

	Source = FPaths::GetPath(Source);
	Source.ReplaceInline(TEXT("\\"), TEXT("/"), ESearchCase::CaseSensitive);
	Target.ReplaceInline(TEXT("\\"), TEXT("/"), ESearchCase::CaseSensitive);

	TArray<FString> TargetArray;
	Target.ParseIntoArray(TargetArray, TEXT("/"), true);
	TArray<FString> SourceArray;
	Source.ParseIntoArray(SourceArray, TEXT("/"), true);

	if (TargetArray.Num() && SourceArray.Num())
	{
		// Check for being on different drives
		if ((TargetArray[0][1] == TEXT(':')) && (SourceArray[0][1] == TEXT(':')))
		{
			if (FChar::ToUpper(TargetArray[0][0]) != FChar::ToUpper(SourceArray[0][0]))
			{
				// The Target and Source are on different drives... No relative path available.
				return false;
			}
		}
	}

	while (TargetArray.Num() && SourceArray.Num() && TargetArray[0] == SourceArray[0])
	{
		TargetArray.RemoveAt(0);
		SourceArray.RemoveAt(0);
	}
	FString Result;
	for (int32 Index = 0; Index < SourceArray.Num(); Index++)
	{
		Result += TEXT("../");
	}
	for (int32 Index = 0; Index < TargetArray.Num(); Index++)
	{
		Result += TargetArray[Index];
		if (Index + 1 < TargetArray.Num())
		{
			Result += TEXT("/");
		}
	}

	InPath = Result;
	return true;
}