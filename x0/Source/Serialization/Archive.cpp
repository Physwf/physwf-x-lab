#include "Serialization/Archive.h"
#include "Math/XMathUtility.h"
#include "HAL/UnrealMemory.h"
#include "Containers/Array.h"
#include "Containers/UnrealString.h"
//#include "UObject/NameTypes.h"
#include "Logging/LogMacros.h"
//#include "Misc/Parse.h"
//#include "UObject/ObjectVersion.h"
//#include "Serialization/ArchiveProxy.h"
//#include "Serialization/NameAsStringProxyArchive.h"
//#include "Misc/CommandLine.h"
//#include "Internationalization/Text.h"
//#include "Stats/StatsMisc.h"
#//include "Stats/Stats.h"
#include "Async/AsyncWork.h"
//#include "Serialization/CustomVersion.h"
//#include "Misc/EngineVersion.h"
//#include "Misc/NetworkVersion.h"
//#include "Interfaces/ITargetPlatform.h"
//#include "Serialization/CompressedChunkInfo.h"
//#include "Serialization/ArchiveSerializedPropertyChain.h"

#include "Containers/StringConv.h"


/*-----------------------------------------------------------------------------
FArchive implementation.
-----------------------------------------------------------------------------*/

FArchive::FArchive()
{
#if DEVIRTUALIZE_FLinkerLoad_Serialize
	ActiveFPLB = &InlineFPLB;
#endif
	//CustomVersionContainer = nullptr;

	//SerializedPropertyChain = nullptr;

#if USE_STABLE_LOCALIZATION_KEYS
	LocalizationNamespacePtr = nullptr;
#endif // USE_STABLE_LOCALIZATION_KEYS

	Reset();
}

FArchive::FArchive(const FArchive& ArchiveToCopy)
{
#if DEVIRTUALIZE_FLinkerLoad_Serialize
	ActiveFPLB = &InlineFPLB;
#endif
#if USE_STABLE_LOCALIZATION_KEYS
	LocalizationNamespacePtr = nullptr;
#endif // USE_STABLE_LOCALIZATION_KEYS

	CopyTrivialFArchiveStatusMembers(ArchiveToCopy);

	//SerializedPropertyChain = nullptr;
	//SetSerializedPropertyChain(ArchiveToCopy.SerializedPropertyChain, ArchiveToCopy.SerializedProperty);

	// Don't know why this is set to false, but this is what the original copying code did
	ArIsFilterEditorOnly = false;

	bCustomVersionsAreReset = ArchiveToCopy.bCustomVersionsAreReset;
// 	if (ArchiveToCopy.CustomVersionContainer)
// 	{
// 		CustomVersionContainer = new FCustomVersionContainer(*ArchiveToCopy.CustomVersionContainer);
// 	}
// 	else
// 	{
// 		CustomVersionContainer = nullptr;
// 	}
}

FArchive& FArchive::operator=(const FArchive& ArchiveToCopy)
{
#if DEVIRTUALIZE_FLinkerLoad_Serialize
	ActiveFPLB = &InlineFPLB;
	ActiveFPLB->Reset();
#endif
	CopyTrivialFArchiveStatusMembers(ArchiveToCopy);

	//SetSerializedPropertyChain(ArchiveToCopy.SerializedPropertyChain, ArchiveToCopy.SerializedProperty);

	// Don't know why this is set to false, but this is what the original copying code did
	ArIsFilterEditorOnly = false;

	bCustomVersionsAreReset = ArchiveToCopy.bCustomVersionsAreReset;
// 	if (ArchiveToCopy.CustomVersionContainer)
// 	{
// 		if (!CustomVersionContainer)
// 		{
// 			CustomVersionContainer = new FCustomVersionContainer(*ArchiveToCopy.CustomVersionContainer);
// 		}
// 		else
// 		{
// 			*CustomVersionContainer = *ArchiveToCopy.CustomVersionContainer;
// 		}
// 	}
// 	else if (CustomVersionContainer)
// 	{
// 		delete CustomVersionContainer;
// 		CustomVersionContainer = nullptr;
// 	}

	return *this;
}

FArchive::~FArchive()
{
	//delete CustomVersionContainer;

	//delete SerializedPropertyChain;

#if USE_STABLE_LOCALIZATION_KEYS
	delete LocalizationNamespacePtr;
#endif // USE_STABLE_LOCALIZATION_KEYS
}

PRAGMA_DISABLE_DEPRECATION_WARNINGS
void FArchive::Reset()
{
#if DEVIRTUALIZE_FLinkerLoad_Serialize
	ActiveFPLB->Reset();
#endif
	//ArUE4Ver = GPackageFileUE4Version;
	//ArLicenseeUE4Ver = GPackageFileLicenseeUE4Version;
	//ArEngineVer = FEngineVersion::Current();
	//ArEngineNetVer = FNetworkVersion::GetEngineNetworkProtocolVersion();
	//ArGameNetVer = FNetworkVersion::GetGameNetworkProtocolVersion();
	ArIsLoading = false;
	ArIsSaving = false;
	ArIsTransacting = false;
	ArIsTextFormat = false;
	ArWantBinaryPropertySerialization = false;
	ArForceUnicode = false;
	ArIsPersistent = false;
	ArIsError = false;
	ArIsCriticalError = false;
	ArContainsCode = false;
	ArContainsMap = false;
	ArRequiresLocalizationGather = false;
	ArForceByteSwapping = false;
	ArSerializingDefaults = false;
	ArIgnoreArchetypeRef = false;
	ArNoDelta = false;
	ArIgnoreOuterRef = false;
	ArIgnoreClassGeneratedByRef = false;
	ArIgnoreClassRef = false;
	ArAllowLazyLoading = false;
	ArIsObjectReferenceCollector = false;
	ArIsModifyingWeakAndStrongReferences = false;
	ArIsCountingMemory = false;
	ArPortFlags = 0;
	ArShouldSkipBulkData = false;
	ArMaxSerializeSize = 0;
	ArIsFilterEditorOnly = false;
	ArIsSaveGame = false;
	ArIsNetArchive = false;
	ArCustomPropertyList = nullptr;
	ArUseCustomPropertyList = false;
	//CookingTargetPlatform = nullptr;
	//SerializedProperty = nullptr;

	//delete SerializedPropertyChain;
	//SerializedPropertyChain = nullptr;

#if USE_STABLE_LOCALIZATION_KEYS
	SetBaseLocalizationNamespace(FString());
#endif // USE_STABLE_LOCALIZATION_KEYS

#if WITH_EDITOR
	ArDebugSerializationFlags = 0;
#endif

	// Reset all custom versions to the current registered versions.
	//ResetCustomVersions();
}

void FArchive::CopyTrivialFArchiveStatusMembers(const FArchive& ArchiveToCopy)
{
	ArUE4Ver = ArchiveToCopy.ArUE4Ver;
	ArLicenseeUE4Ver = ArchiveToCopy.ArLicenseeUE4Ver;
	//ArEngineVer = ArchiveToCopy.ArEngineVer;
	ArEngineNetVer = ArchiveToCopy.ArEngineNetVer;
	ArGameNetVer = ArchiveToCopy.ArGameNetVer;
	ArIsLoading = ArchiveToCopy.ArIsLoading;
	ArIsSaving = ArchiveToCopy.ArIsSaving;
	ArIsTransacting = ArchiveToCopy.ArIsTransacting;
	ArIsTextFormat = ArchiveToCopy.ArIsTextFormat;
	ArWantBinaryPropertySerialization = ArchiveToCopy.ArWantBinaryPropertySerialization;
	ArForceUnicode = ArchiveToCopy.ArForceUnicode;
	ArIsPersistent = ArchiveToCopy.ArIsPersistent;
	ArIsError = ArchiveToCopy.ArIsError;
	ArIsCriticalError = ArchiveToCopy.ArIsCriticalError;
	ArContainsCode = ArchiveToCopy.ArContainsCode;
	ArContainsMap = ArchiveToCopy.ArContainsMap;
	ArRequiresLocalizationGather = ArchiveToCopy.ArRequiresLocalizationGather;
	ArForceByteSwapping = ArchiveToCopy.ArForceByteSwapping;
	ArSerializingDefaults = ArchiveToCopy.ArSerializingDefaults;
	ArIgnoreArchetypeRef = ArchiveToCopy.ArIgnoreArchetypeRef;
	ArNoDelta = ArchiveToCopy.ArNoDelta;
	ArIgnoreOuterRef = ArchiveToCopy.ArIgnoreOuterRef;
	ArIgnoreClassGeneratedByRef = ArchiveToCopy.ArIgnoreClassGeneratedByRef;
	ArIgnoreClassRef = ArchiveToCopy.ArIgnoreClassRef;
	ArAllowLazyLoading = ArchiveToCopy.ArAllowLazyLoading;
	ArIsObjectReferenceCollector = ArchiveToCopy.ArIsObjectReferenceCollector;
	ArIsModifyingWeakAndStrongReferences = ArchiveToCopy.ArIsModifyingWeakAndStrongReferences;
	ArIsCountingMemory = ArchiveToCopy.ArIsCountingMemory;
	ArPortFlags = ArchiveToCopy.ArPortFlags;
	ArShouldSkipBulkData = ArchiveToCopy.ArShouldSkipBulkData;
	ArMaxSerializeSize = ArchiveToCopy.ArMaxSerializeSize;
	ArIsFilterEditorOnly = ArchiveToCopy.ArIsFilterEditorOnly;
	ArIsSaveGame = ArchiveToCopy.ArIsSaveGame;
	ArIsNetArchive = ArchiveToCopy.ArIsNetArchive;
	ArCustomPropertyList = ArchiveToCopy.ArCustomPropertyList;
	ArUseCustomPropertyList = ArchiveToCopy.ArUseCustomPropertyList;
	//CookingTargetPlatform = ArchiveToCopy.CookingTargetPlatform;
	//SerializedProperty = ArchiveToCopy.SerializedProperty;
#if USE_STABLE_LOCALIZATION_KEYS
	SetBaseLocalizationNamespace(ArchiveToCopy.GetBaseLocalizationNamespace());
#endif // USE_STABLE_LOCALIZATION_KEYS
}
PRAGMA_ENABLE_DEPRECATION_WARNINGS

/**
* Returns the name of the Archive.  Useful for getting the name of the package a struct or object
* is in when a loading error occurs.
*
* This is overridden for the specific Archive Types
**/
FString FArchive::GetArchiveName() const
{
	return TEXT("FArchive");
}

#if WITH_EDITOR
FArchive::FScopeAddDebugData::FScopeAddDebugData(FArchive& InAr, const FName& DebugData) : Ar(InAr)
{
	Ar.PushDebugDataString(DebugData);
}

void FArchive::PushDebugDataString(const FName& DebugData)
{
}
#endif

#if WITH_EDITOR
void FArchive::SerializeBool(bool& D)
{
	// Serialize bool as if it were UBOOL (legacy, 32 bit int).
	uint32 OldUBoolValue;
#if DEVIRTUALIZE_FLinkerLoad_Serialize
	const uint8 * RESTRICT Src = this->ActiveFPLB->StartFastPathLoadBuffer;
	if (Src + sizeof(uint32) <= this->ActiveFPLB->EndFastPathLoadBuffer)
	{
#if PLATFORM_SUPPORTS_UNALIGNED_INT_LOADS
		OldUBoolValue = *(uint32* RESTRICT)Src;
#else
		static_assert(sizeof(uint32) == 4, "assuming sizeof(uint32) == 4");
		OldUBoolValue = Src[0] | Src[1] | Src[2] | Src[3];
#endif
		this->ActiveFPLB->StartFastPathLoadBuffer += 4;
	}
	else
#endif
	{
		OldUBoolValue = D ? 1 : 0;
		this->Serialize(&OldUBoolValue, sizeof(OldUBoolValue));
	}
	if (OldUBoolValue > 1)
	{
		UE_LOG(LogSerialization, Error, TEXT("Invalid boolean encountered while reading archive %s - stream is most likely corrupted."), *GetArchiveName());

		this->ArIsError = true;
	}
	D = !!OldUBoolValue;
}
#endif

void FArchive::ByteSwap(void* V, int32 Length)
{
	uint8* Ptr = (uint8*)V;
	int32 Top = Length - 1;
	int32 Bottom = 0;
	while (Bottom < Top)
	{
		Swap(Ptr[Top--], Ptr[Bottom++]);
	}
}

void FArchive::SerializeIntPacked(uint32& Value)
{
	if (IsLoading())
	{
		Value = 0;
		uint8 cnt = 0;
		uint8 more = 1;
		while (more)
		{
			uint8 NextByte;
			Serialize(&NextByte, 1);			// Read next byte

			more = NextByte & 1;				// Check 1 bit to see if theres more after this
			NextByte = NextByte >> 1;			// Shift to get actual 7 bit value
			Value += NextByte << (7 * cnt++);	// Add to total value
		}
	}
	else
	{
		TArray<uint8, TInlineAllocator<6>> PackedBytes;
		uint32 Remaining = Value;
		while (true)
		{
			uint8 nextByte = Remaining & 0x7f;		// Get next 7 bits to write
			Remaining = Remaining >> 7;				// Update remaining
			nextByte = nextByte << 1;				// Make room for 'more' bit
			if (Remaining > 0)
			{
				nextByte |= 1;						// set more bit
				PackedBytes.Add(nextByte);
			}
			else
			{
				PackedBytes.Add(nextByte);
				break;
			}
		}
		Serialize(PackedBytes.GetData(), PackedBytes.Num()); // Actually serialize the bytes we made
	}
}

void FArchive::LogfImpl(const TCHAR* Fmt, ...)
{
	// We need to use malloc here directly as GMalloc might not be safe, e.g. if called from within GMalloc!
	int32		BufferSize = 1024;
	TCHAR*	Buffer = NULL;
	int32		Result = -1;

	while (Result == -1)
	{
		FMemory::SystemFree(Buffer);
		Buffer = (TCHAR*)FMemory::SystemMalloc(BufferSize * sizeof(TCHAR));
		GET_VARARGS_RESULT(Buffer, BufferSize, BufferSize - 1, Fmt, Fmt, Result);
		BufferSize *= 2;
	};
	Buffer[Result] = 0;

	// Convert to ANSI and serialize as ANSI char.
	for (int32 i = 0; i < Result; i++)
	{
		ANSICHAR Char = CharCast<ANSICHAR>(Buffer[i]);
		Serialize(&Char, 1);
	}

	// Write out line terminator.
	for (int32 i = 0; LINE_TERMINATOR[i]; i++)
	{
		ANSICHAR Char = LINE_TERMINATOR[i];
		Serialize(&Char, 1);
	}

	// Free temporary buffers.
	FMemory::SystemFree(Buffer);
}

PRAGMA_DISABLE_DEPRECATION_WARNINGS
void FArchive::SetUE4Ver(int32 InVer)
{
	ArUE4Ver = InVer;
}

void FArchive::SetLicenseeUE4Ver(int32 InVer)
{
	ArLicenseeUE4Ver = InVer;
}

// void FArchive::SetEngineVer(const FEngineVersionBase& InVer)
// {
// 	ArEngineVer = InVer;
// }
// 
// void FArchive::SetEngineNetVer(const uint32 InEngineNetVer)
// {
// 	ArEngineNetVer = InEngineNetVer;
// }
// 
// void FArchive::SetGameNetVer(const uint32 InGameNetVer)
// {
// 	ArGameNetVer = InGameNetVer;
// }

void FArchive::SetIsLoading(bool bInIsLoading)
{
	ArIsLoading = bInIsLoading;
}

void FArchive::SetIsSaving(bool bInIsSaving)
{
	ArIsSaving = bInIsSaving;
}

void FArchive::SetIsTransacting(bool bInIsTransacting)
{
	ArIsTransacting = bInIsTransacting;
}

void FArchive::SetIsTextFormat(bool bInIsTextFormat)
{
	ArIsTextFormat = bInIsTextFormat;
}

void FArchive::SetWantBinaryPropertySerialization(bool bInWantBinaryPropertySerialization)
{
	ArWantBinaryPropertySerialization = bInWantBinaryPropertySerialization;
}

void FArchive::SetForceUnicode(bool bInForceUnicode)
{
	ArForceUnicode = bInForceUnicode;
}

void FArchive::SetIsPersistent(bool bInIsPersistent)
{
	ArIsPersistent = bInIsPersistent;
}
PRAGMA_ENABLE_DEPRECATION_WARNINGS