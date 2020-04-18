#include "GenericPlatform/GenericPlatformMemory.h"
//#include "HAL/LowLevelMemTracker.h"
#include "HAL/PlatformMemory.h"
#include "Misc/AssertionMacros.h"
#include "Math/XMathUtility.h"
//#include "Containers/StringConv.h"
//#include "UObject/NameTypes.h"
//#include "Logging/LogMacros.h"
//#include "Stats/Stats.h"
//#include "Containers/Ticker.h"
//#include "Misc/FeedbackContext.h"
//#include "Async/Async.h"
#include "HAL/MallocAnsi.h"
//#include "GenericPlatform/GenericPlatformMemoryPoolStats.h"
#include "HAL/MemoryMisc.h"

FGenericPlatformMemoryStats::FGenericPlatformMemoryStats()
	: FGenericPlatformMemoryConstants(FPlatformMemory::GetConstants())
	, AvailablePhysical(0)
	, AvailableVirtual(0)
	, UsedPhysical(0)
	, PeakUsedPhysical(0)
	, UsedVirtual(0)
	, PeakUsedVirtual(0)
{}

bool FGenericPlatformMemory::bIsOOM = false;
uint64 FGenericPlatformMemory::OOMAllocationSize = 0;
uint32 FGenericPlatformMemory::OOMAllocationAlignment = 0;
FGenericPlatformMemory::EMemoryAllocatorToUse FGenericPlatformMemory::AllocatorToUse = Platform;
void* FGenericPlatformMemory::BackupOOMMemoryPool = nullptr;

void FGenericPlatformMemory::Init()
{
	SetupMemoryPools();
}

/*CA_NO_RETURN*/ void FGenericPlatformMemory::OnOutOfMemory(uint64 Size, uint32 Alignment)
{

}

void FGenericPlatformMemory::SetupMemoryPools()
{
	if (FPlatformMemory::GetBackMemoryPoolSize() > 0)
	{
		//LLM_PLATFORM_SCOPE(ELLMTag::BackupOOMMemoryPoolPlatform);
		//LLM_SCOPE(ELLMTag::BackupOOMMemoryPool);

		BackupOOMMemoryPool = FPlatformMemory::BinnedAllocFromOS(FPlatformMemory::GetBackMemoryPoolSize());

		//LLM(FLowLevelMemTracker::Get().OnLowLevelAlloc(ELLMTracker::Default, BackupOOMMemoryPool, FPlatformMemory::GetBackMemoryPoolSize()));
	}
}

//#include "Misc/CoreDelegates.h"
//#include "Misc/ConfigCacheIni.h"
//#include "Misc/CommandLine.h"
//#include "Misc/MessageDialog.h"

void FGenericPlatformMemory::MemswapGreaterThan8(void* Ptr1, void* Ptr2, SIZE_T Size)
{
	union PtrUnion
	{
		void*   PtrVoid;
		uint8*  Ptr8;
		uint16* Ptr16;
		uint32* Ptr32;
		uint64* Ptr64;
		UPTRINT PtrUint;
	};

	PtrUnion Union1 = { Ptr1 };
	PtrUnion Union2 = { Ptr2 };

	checkf(Union1.PtrVoid && Union2.PtrVoid, TEXT("Pointers must be non-null: %p, %p"), Union1.PtrVoid, Union2.PtrVoid);

	// We may skip up to 7 bytes below, so better make sure that we're swapping more than that
	// (8 is a common case that we also want to inline before we this call, so skip that too)
	check(Size > 8);

	if (Union1.PtrUint & 1)
	{
		Valswap(*Union1.Ptr8++, *Union2.Ptr8++);
		Size -= 1;
	}
	if (Union1.PtrUint & 2)
	{
		Valswap(*Union1.Ptr16++, *Union2.Ptr16++);
		Size -= 2;
	}
	if (Union1.PtrUint & 4)
	{
		Valswap(*Union1.Ptr32++, *Union2.Ptr32++);
		Size -= 4;
	}

	uint32 CommonAlignment = FMath::Min(FMath::CountTrailingZeros(Union1.PtrUint - Union2.PtrUint), 3u);
	switch (CommonAlignment)
	{
	default:
		for (; Size >= 8; Size -= 8)
		{
			Valswap(*Union1.Ptr64++, *Union2.Ptr64++);
		}

	case 2:
		for (; Size >= 4; Size -= 4)
		{
			Valswap(*Union1.Ptr32++, *Union2.Ptr32++);
		}

	case 1:
		for (; Size >= 2; Size -= 2)
		{
			Valswap(*Union1.Ptr16++, *Union2.Ptr16++);
		}

	case 0:
		for (; Size >= 1; Size -= 1)
		{
			Valswap(*Union1.Ptr8++, *Union2.Ptr8++);
		}
	}
}
