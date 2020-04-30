#include "RenderCore/RenderCore.h"
//#include "HAL/IConsoleManager.h"
#include "RenderCore/UniformBuffer.h"
#include "Modules/ModuleManager.h"

#include "X4.h"

/** Whether to pause the global realtime clock for the rendering thread (read and write only on main thread). */
bool GPauseRenderingRealtimeClock;

/** Global realtime clock for the rendering thread. */
FTimer GRenderingRealtimeClock;

FInputLatencyTimer GInputLatencyTimer(2.0f);

/** Potentially starts the timer on the gamethread, based on the UpdateFrequency. */
void FInputLatencyTimer::GameThreadTick()
{
#if STATS
	if (FThreadStats::IsCollectingData())
	{
		if (!bInitialized)
		{
			LastCaptureTime = FPlatformTime::Seconds();
			bInitialized = true;
		}
		float CurrentTimeInSeconds = FPlatformTime::Seconds();
		if ((CurrentTimeInSeconds - LastCaptureTime) > UpdateFrequency)
		{
			LastCaptureTime = CurrentTimeInSeconds;
			StartTime = FPlatformTime::Cycles();
			GameThreadTrigger = true;
		}
	}
#endif
}

TLinkedList<FUniformBufferStruct*>*& FUniformBufferStruct::GetStructList()
{
	static TLinkedList<FUniformBufferStruct*>* GUniformStructList = NULL;
	return GUniformStructList;
}

TMap<FString, FUniformBufferStruct*>& FUniformBufferStruct::GetNameStructMap()
{
	static 	TMap<FString, FUniformBufferStruct*> GlobalNameStructMap;
	return GlobalNameStructMap;
}

FUniformBufferStruct* FindUniformBufferStructByName(const TCHAR* StructName)
{
	FString FindByName(StructName);
	FUniformBufferStruct* FoundStruct = FUniformBufferStruct::GetNameStructMap().FindRef(FindByName);
	return FoundStruct;
}

FUniformBufferStruct* FindUniformBufferStructByFName(FString StructName)
{
	return FUniformBufferStruct::GetNameStructMap().FindRef(StructName);
}

// Can be optimized to avoid the virtual function call but it's compiled out for final release anyway
X4_API int32 GetCVarForceLOD()
{
	int32 Ret = -1;

#if EXPOSE_FORCE_LOD
	{
		Ret = CVarForceLOD.GetValueOnRenderThread();
	}
#endif // EXPOSE_FORCE_LOD

	return Ret;
}

X4_API int32 GetCVarForceLODShadow()
{
	int32 Ret = -1;

#if EXPOSE_FORCE_LOD
	{
		Ret = CVarForceLODShadow.GetValueOnRenderThread();
	}
#endif // EXPOSE_FORCE_LOD

	return Ret;
}

X4_API bool IsHDREnabled()
{
// 	static const auto CVarHDROutputEnabled = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.HDR.EnableHDROutput"));
// 	if (CVarHDROutputEnabled)
// 	{
// 		if (CVarHDROutputEnabled->GetValueOnAnyThread() != 0)
// 		{
// 			return true;
// 		}
// 	}
	return false;
}

class FUniformBufferMemberAndOffset
{
public:
	FUniformBufferMemberAndOffset(const FUniformBufferStruct::FMember& InMember, int32 InStructOffset) :
		Member(InMember),
		StructOffset(InStructOffset)
	{}

	FUniformBufferStruct::FMember Member;
	int32 StructOffset;
};

FUniformBufferStruct::FUniformBufferStruct(const FString& InLayoutName, const TCHAR* InStructTypeName, const TCHAR* InShaderVariableName, ConstructUniformBufferParameterType InConstructRef, uint32 InSize, const TArray<FMember>& InMembers, bool bRegisterForAutoBinding)
	: StructTypeName(InStructTypeName)
	, ShaderVariableName(InShaderVariableName)
	, ConstructUniformBufferParameterRef(InConstructRef)
	, Size(InSize)
	, bLayoutInitialized(false)
	, Layout(InLayoutName)
	, Members(InMembers)
	, GlobalListLink(this)
{
	if (bRegisterForAutoBinding)
	{
		GlobalListLink.LinkHead(GetStructList());
		FString StrutTypeFName(StructTypeName);
		// Verify that during FName creation there's no case conversion
		checkSlow(FCString::Strcmp(StructTypeName, *StrutTypeFName) == 0);
		GetNameStructMap().Add(FString(StructTypeName), this);
	}
	else
	{
		// We cannot initialize the layout during global initialization, since we have to walk nested struct members.
		// Structs created during global initialization will have bRegisterForAutoBinding==false, and are initialized during startup.
		// Structs created at runtime with bRegisterForAutoBinding==true can be initialized now.
		InitializeLayout();
	}
}

void FUniformBufferStruct::InitializeStructs()
{
	for (TLinkedList<FUniformBufferStruct*>::TIterator StructIt(FUniformBufferStruct::GetStructList()); StructIt; StructIt.Next())
	{
		StructIt->InitializeLayout();
	}
}

void FUniformBufferStruct::InitializeLayout()
{
	check(!bLayoutInitialized);
	Layout.ConstantBufferSize = Size;

	TArray<FUniformBufferMemberAndOffset> MemberStack;
	MemberStack.Reserve(Members.Num());

	for (int32 MemberIndex = 0; MemberIndex < Members.Num(); MemberIndex++)
	{
		MemberStack.Push(FUniformBufferMemberAndOffset(Members[MemberIndex], 0));
	}

	for (int32 i = 0; i < MemberStack.Num(); ++i)
	{
		const FMember& CurrentMember = MemberStack[i].Member;
		bool bIsResource = IsUniformBufferResourceType(CurrentMember.GetBaseType());

		if (bIsResource)
		{
			Layout.Resources.Add(CurrentMember.GetBaseType());
			const uint32 AbsoluteMemberOffset = CurrentMember.GetOffset() + MemberStack[i].StructOffset;
			check(AbsoluteMemberOffset < (1u << (Layout.ResourceOffsets.GetTypeSize() * 8)));
			Layout.ResourceOffsets.Add(AbsoluteMemberOffset);
		}

		const FUniformBufferStruct* MemberStruct = CurrentMember.GetStruct();

		if (MemberStruct)
		{
			int32 AbsoluteStructOffset = CurrentMember.GetOffset() + MemberStack[i].StructOffset;

			for (int32 StructMemberIndex = 0; StructMemberIndex < MemberStruct->Members.Num(); StructMemberIndex++)
			{
				FMember StructMember = MemberStruct->Members[StructMemberIndex];
				MemberStack.Insert(FUniformBufferMemberAndOffset(StructMember, AbsoluteStructOffset), i + 1 + StructMemberIndex);
			}
		}
	}

	Layout.ComputeHash();

	bLayoutInitialized = true;
}

void FUniformBufferStruct::GetNestedStructs(TArray<const FUniformBufferStruct*>& OutNestedStructs) const
{
	for (int32 i = 0; i < Members.Num(); ++i)
	{
		const FMember& CurrentMember = Members[i];

		const FUniformBufferStruct* MemberStruct = CurrentMember.GetStruct();

		if (MemberStruct)
		{
			OutNestedStructs.Add(MemberStruct);
			MemberStruct->GetNestedStructs(OutNestedStructs);
		}
	}
}

void FUniformBufferStruct::AddResourceTableEntries(TMap<FString, FResourceTableEntry>& ResourceTableMap, TMap<FString, uint32>& ResourceTableLayoutHashes) const
{
	uint16 ResourceIndex = 0;
	FString Prefix = FString::Printf(TEXT("%s_"), ShaderVariableName);
	AddResourceTableEntriesRecursive(ShaderVariableName, *Prefix, ResourceIndex, ResourceTableMap);
	ResourceTableLayoutHashes.Add(ShaderVariableName, GetLayout().GetHash());
}

void FUniformBufferStruct::AddResourceTableEntriesRecursive(const TCHAR* UniformBufferName, const TCHAR* Prefix, uint16& ResourceIndex, TMap<FString, FResourceTableEntry>& ResourceTableMap) const
{
	for (int32 MemberIndex = 0; MemberIndex < Members.Num(); ++MemberIndex)
	{
		const FMember& Member = Members[MemberIndex];
		if (IsUniformBufferResourceType(Member.GetBaseType()))
		{
			FResourceTableEntry& Entry = ResourceTableMap.FindOrAdd(FString::Printf(TEXT("%s%s"), Prefix, Member.GetName()));
			if (Entry.UniformBufferName.IsEmpty())
			{
				Entry.UniformBufferName = UniformBufferName;
				Entry.Type = Member.GetBaseType();
				Entry.ResourceIndex = ResourceIndex++;
			}
		}
		else if (Member.GetBaseType() == UBMT_STRUCT)
		{
			check(Member.GetStruct());
			FString MemberPrefix = FString::Printf(TEXT("%s%s_"), Prefix, Member.GetName());
			Member.GetStruct()->AddResourceTableEntriesRecursive(UniformBufferName, *MemberPrefix, ResourceIndex, ResourceTableMap);
		}
	}
}