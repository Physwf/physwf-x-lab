#include "Misc/FileHelper.h"
#include "Containers/StringConv.h"
#include "Logging/LogMacros.h"
#include "CoreGlobals.h"
#include "Misc/ByteSwap.h"
#include "Misc/CoreMisc.h"
#include "Misc/Paths.h"
#include "Math/IntRect.h"
//#include "Misc/OutputDeviceFile.h"
//#include "ProfilingDebugging/ProfilingHelpers.h"
//#include "Misc/ConfigCacheIni.h"
#include "Misc/SecureHash.h"

#include "Templates/UniquePtr.h"
#include "Serialization/Archive.h"
/**
* Load a text file to an FString.
* Supports all combination of ANSI/Unicode files and platforms.
* @param Result string representation of the loaded file
* @param Filename name of the file to load
* @param VerifyFlags flags controlling the hash verification behavior ( see EHashOptions )
*/
bool FFileHelper::LoadFileToString(FString& Result, const TCHAR* Filename, EHashOptions VerifyFlags)
{
	//FScopedLoadingState ScopedLoadingState(Filename);

	TUniquePtr<FArchive> Reader(IFileManager::Get().CreateFileReader(Filename));
	if (!Reader)
	{
		return false;
	}

	int32 Size = Reader->TotalSize();
	if (!Size)
	{
		Result.Empty();
		return true;
	}

	uint8* Ch = (uint8*)FMemory::Malloc(Size);
	Reader->Serialize(Ch, Size);
	bool Success = Reader->Close();
	Reader = nullptr;
	BufferToString(Result, Ch, Size);

	// handle SHA verify of the file
// 	if (EnumHasAnyFlags(VerifyFlags, EHashOptions::EnableVerify) && (EnumHasAnyFlags(VerifyFlags, EHashOptions::ErrorMissingHash) || FSHA1::GetFileSHAHash(Filename, NULL)))
// 	{
// 		// kick off SHA verify task. this frees the buffer on close
// 		FBufferReaderWithSHA Ar(Ch, Size, true, Filename, false, true);
// 	}
// 	else
	{
		// free manually since not running SHA task
		FMemory::Free(Ch);
	}

	return Success;
}