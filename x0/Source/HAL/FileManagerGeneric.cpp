#include "HAL/FileManagerGeneric.h"
#include "Logging/LogMacros.h"
//#include "Misc/Parse.h"
//#include "Misc/CommandLine.h"
#include "Misc/Paths.h"
#include "Misc/SecureHash.h"
#include "Templates/UniquePtr.h"
#include <time.h>

#include "HAL/PlatformProcess.h"

#define COPYBLOCKSIZE	32768

/*-----------------------------------------------------------------------------
File Manager.
-----------------------------------------------------------------------------*/






int64 FFileManagerGeneric::FileSize(const TCHAR* Filename)
{
	return GetLowLevel().FileSize(Filename);
}


bool FFileManagerGeneric::FileExists(const TCHAR* Filename)
{
	return GetLowLevel().FileExists(Filename);
}

bool FFileManagerGeneric::DirectoryExists(const TCHAR* InDirectory)
{
	return GetLowLevel().DirectoryExists(InDirectory);
}

bool FFileManagerGeneric::MakeDirectory(const TCHAR* Path, bool Tree)
{
	if (Tree)
	{
		return GetLowLevel().CreateDirectoryTree(Path);
	}
	else
	{
		return GetLowLevel().CreateDirectory(Path);
	}
}

bool FFileManagerGeneric::DeleteDirectory(const TCHAR* Path, bool RequireExists, bool Tree)
{
	if (Tree)
	{
		return GetLowLevel().DeleteDirectoryRecursively(Path) || (!RequireExists && !GetLowLevel().DirectoryExists(Path));
	}
	return GetLowLevel().DeleteDirectory(Path) || (!RequireExists && !GetLowLevel().DirectoryExists(Path));
}

FFileStatData FFileManagerGeneric::GetStatData(const TCHAR* FilenameOrDirectory)
{
	return GetLowLevel().GetStatData(FilenameOrDirectory);
}

void FFileManagerGeneric::FindFiles(TArray<FString>& Result, const TCHAR* InFilename, bool Files, bool Directories)
{
	class FFileMatch : public IPlatformFile::FDirectoryVisitor
	{
	public:
		TArray<FString>& Result;
		FString WildCard;
		bool bFiles;
		bool bDirectories;
		FFileMatch(TArray<FString>& InResult, const FString& InWildCard, bool bInFiles, bool bInDirectories)
			: Result(InResult)
			, WildCard(InWildCard)
			, bFiles(bInFiles)
			, bDirectories(bInDirectories)
		{
		}
		virtual bool Visit(const TCHAR* FilenameOrDirectory, bool bIsDirectory)
		{
			if (((bIsDirectory && bDirectories) || (!bIsDirectory && bFiles))
				&& FPaths::GetCleanFilename(FilenameOrDirectory).MatchesWildcard(WildCard))
			{
				new(Result) FString(FPaths::GetCleanFilename(FilenameOrDirectory));
			}
			return true;
		}
	};
	FString Filename(InFilename);
	FPaths::NormalizeFilename(Filename);
	const FString CleanFilename = FPaths::GetCleanFilename(Filename);
	const bool bFindAllFiles = CleanFilename == TEXT("*") || CleanFilename == TEXT("*.*");
	FFileMatch FileMatch(Result, bFindAllFiles ? TEXT("*") : CleanFilename, Files, Directories);
	GetLowLevel().IterateDirectory(*FPaths::GetPath(Filename), FileMatch);
}

void FFileManagerGeneric::FindFiles(TArray<FString>& FoundFiles, const TCHAR* Directory, const TCHAR* FileExtension)
{
	if (!Directory)
	{
		return;
	}

	FString RootDir(Directory);
	FString ExtStr = (FileExtension != nullptr) ? FString(FileExtension) : "";

	// No Directory?
	if (RootDir.Len() < 1)
	{
		return;
	}

	FPaths::NormalizeDirectoryName(RootDir);

	// Don't modify the ExtStr if the user supplied the form "*.EXT" or "*" or "*.*" or "Name.*"
	if (!ExtStr.Contains(TEXT("*")))
	{
		if (ExtStr == "")
		{
			ExtStr = "*.*";
		}
		else
		{
			//Complete the supplied extension with * or *. to yield "*.EXT"
			ExtStr = (ExtStr.Left(1) == ".") ? "*" + ExtStr : "*." + ExtStr;
		}
	}

	// Create the full filter, which is "Directory/*.EXT".
	FString FinalPath = RootDir + "/" + ExtStr;
	FindFiles(FoundFiles, *FinalPath, true, false);
}

bool FFileManagerGeneric::IterateDirectory(const TCHAR* Directory, IPlatformFile::FDirectoryVisitor& Visitor)
{
	return GetLowLevel().IterateDirectory(Directory, Visitor);
}

bool FFileManagerGeneric::IterateDirectoryRecursively(const TCHAR* Directory, IPlatformFile::FDirectoryVisitor& Visitor)
{
	return GetLowLevel().IterateDirectoryRecursively(Directory, Visitor);
}

bool FFileManagerGeneric::IterateDirectoryStat(const TCHAR* Directory, IPlatformFile::FDirectoryStatVisitor& Visitor)
{
	return GetLowLevel().IterateDirectoryStat(Directory, Visitor);
}

bool FFileManagerGeneric::IterateDirectoryStatRecursively(const TCHAR* Directory, IPlatformFile::FDirectoryStatVisitor& Visitor)
{
	return GetLowLevel().IterateDirectoryStatRecursively(Directory, Visitor);
}

double FFileManagerGeneric::GetFileAgeSeconds(const TCHAR* Filename)
{
	// make sure it exists
	if (!GetLowLevel().FileExists(Filename))
	{
		return -1.0;
	}
	// get difference in time between now (UTC) and the filetime
	FTimespan Age = FDateTime::UtcNow() - GetTimeStamp(Filename);
	return Age.GetTotalSeconds();
}

FDateTime FFileManagerGeneric::GetTimeStamp(const TCHAR* Filename)
{
	// ask low level for timestamp
	return GetLowLevel().GetTimeStamp(Filename);
}

void FFileManagerGeneric::GetTimeStampPair(const TCHAR* PathA, const TCHAR* PathB, FDateTime& OutTimeStampA, FDateTime& OutTimeStampB)
{
	GetLowLevel().GetTimeStampPair(PathA, PathB, OutTimeStampA, OutTimeStampB);
}

bool FFileManagerGeneric::SetTimeStamp(const TCHAR* Filename, FDateTime DateTime)
{
	// if the file doesn't exist, fail
	if (!GetLowLevel().FileExists(Filename))
	{
		return false;
	}

	GetLowLevel().SetTimeStamp(Filename, DateTime);
	return true;
}

FDateTime FFileManagerGeneric::GetAccessTimeStamp(const TCHAR* Filename)
{
	// ask low level for timestamp
	return GetLowLevel().GetAccessTimeStamp(Filename);
}

FString FFileManagerGeneric::GetFilenameOnDisk(const TCHAR* Filename)
{
	return GetLowLevel().GetFilenameOnDisk(Filename);
}

FString FFileManagerGeneric::DefaultConvertToRelativePath(const TCHAR* Filename)
{
	//default to the full absolute path of this file
	FString RelativePath(Filename);
	FPaths::NormalizeFilename(RelativePath);

	// See whether it is a relative path.
	FString RootDirectory(FPlatformMisc::RootDir());
	FPaths::NormalizeFilename(RootDirectory);

	//the default relative directory it to the app root which is 3 directories up from the starting directory
	int32 NumberOfDirectoriesToGoUp = 3;

	//temp holder for current position of the slash
	int32 CurrentSlashPosition;

	//while we haven't run out of parent directories until we which a drive name
	while ((CurrentSlashPosition = RootDirectory.Find(TEXT("/"), ESearchCase::CaseSensitive, ESearchDir::FromEnd)) != INDEX_NONE)
	{
		if (RelativePath.StartsWith(RootDirectory))
		{
			FString BinariesDir = FString(FPlatformProcess::BaseDir());
			FPaths::MakePathRelativeTo(RelativePath, *BinariesDir);
			break;
		}
		int32 PositionOfNextSlash = RootDirectory.Find(TEXT("/"), ESearchCase::CaseSensitive, ESearchDir::FromEnd, CurrentSlashPosition);
		//if there is another slash to find
		if (PositionOfNextSlash != INDEX_NONE)
		{
			//move up a directory and on an extra .. TEXT("/")
			// the +1 from "InStr" moves to include the "\" at the end of the directory name
			NumberOfDirectoriesToGoUp++;
			RootDirectory = RootDirectory.Left(PositionOfNextSlash + 1);
		}
		else
		{
			RootDirectory.Empty();
		}
	}
	return RelativePath;
}

FString FFileManagerGeneric::ConvertToRelativePath(const TCHAR* Filename)
{
	return DefaultConvertToRelativePath(Filename);
}

FString FFileManagerGeneric::ConvertToAbsolutePathForExternalAppForRead(const TCHAR* AbsolutePath)
{
	return GetLowLevel().ConvertToAbsolutePathForExternalAppForRead(AbsolutePath);
}

FString FFileManagerGeneric::ConvertToAbsolutePathForExternalAppForWrite(const TCHAR* AbsolutePath)
{
	return GetLowLevel().ConvertToAbsolutePathForExternalAppForWrite(AbsolutePath);
}

void FFileManagerGeneric::FindFilesRecursive(TArray<FString>& FileNames, const TCHAR* StartDirectory, const TCHAR* Filename, bool Files, bool Directories, bool bClearFileNames)
{
	if (bClearFileNames)
	{
		FileNames.Empty();
	}
	FindFilesRecursiveInternal(FileNames, StartDirectory, Filename, Files, Directories);
}

void FFileManagerGeneric::FindFilesRecursiveInternal(TArray<FString>& FileNames, const TCHAR* StartDirectory, const TCHAR* Filename, bool Files, bool Directories)
{
	FString CurrentSearch = FString(StartDirectory) / Filename;
	TArray<FString> Result;
	FindFiles(Result, *CurrentSearch, Files, Directories);

	for (int32 i = 0; i<Result.Num(); i++)
	{
		FileNames.Add(FString(StartDirectory) / Result[i]);
	}

	TArray<FString> SubDirs;
	FString RecursiveDirSearch = FString(StartDirectory) / TEXT("*");
	FindFiles(SubDirs, *RecursiveDirSearch, false, true);

	for (int32 SubDirIdx = 0; SubDirIdx<SubDirs.Num(); SubDirIdx++)
	{
		FString SubDir = FString(StartDirectory) / SubDirs[SubDirIdx];
		FindFilesRecursiveInternal(FileNames, *SubDir, Filename, Files, Directories);
	}
}
