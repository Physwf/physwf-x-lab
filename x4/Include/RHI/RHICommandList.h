#pragma once

#include "CoreTypes.h"
#include "Misc/AssertionMacros.h"
#include "HAL/UnrealMemory.h"
#include "Templates/UnrealTemplate.h"
#include "Math/Color.h"
#include "Math/IntPoint.h"
#include "Math/IntRect.h"
#include "Math/Box2D.h"
#include "Math/PerspectiveMatrix.h"
#include "Math/TranslationMatrix.h"
#include "Math/ScaleMatrix.h"
#include "Math/Float16Color.h"
#include "HAL/ThreadSafeCounter.h"
//#include "GenericPlatform/GenericPlatformProcess.h"
//#include "Misc/MemStack.h"
//#include "Misc/App.h"
//#include "Stats/Stats.h"
//#include "HAL/IConsoleManager.h"
//#include "Async/TaskGraphInterfaces.h"
//#include "HAL/LowLevelMemTracker.h"


#include "X4.h"

class FApp;
class FBlendStateInitializerRHI;
class FGraphicsPipelineStateInitializer;
class FLastRenderTimeContainer;
class FReadSurfaceDataFlags;
class FRHICommandListBase;
class FRHIComputeShader;
class FRHIDepthRenderTargetView;
class FRHIRenderTargetView;
class FRHISetRenderTargetsInfo;
class IRHICommandContext;
class IRHIComputeContext;
struct FBoundShaderStateInput;
struct FDepthStencilStateInitializerRHI;
struct FRasterizerStateInitializerRHI;
struct FResolveParams;
struct FRHIResourceCreateInfo;
struct FRHIResourceInfo;
struct FRHIUniformBufferLayout;
struct FSamplerStateInitializerRHI;
struct FTextureMemoryStats;
enum class EAsyncComputeBudget;
enum class EClearDepthStencil;
enum class EResourceTransitionAccess;
enum class EResourceTransitionPipeline;
class FComputePipelineState;
class FGraphicsPipelineState;

struct X4_API FLockTracker
{
	struct FLockParams
	{
		void* RHIBuffer;
		void* Buffer;
		uint32 BufferSize;
		uint32 Offset;
		EResourceLockMode LockMode;

		FORCEINLINE_DEBUGGABLE FLockParams(void* InRHIBuffer, void* InBuffer, uint32 InOffset, uint32 InBufferSize, EResourceLockMode InLockMode)
			: RHIBuffer(InRHIBuffer)
			, Buffer(InBuffer)
			, BufferSize(InBufferSize)
			, Offset(InOffset)
			, LockMode(InLockMode)
		{
		}
	};
	TArray<FLockParams, TInlineAllocator<16> > OutstandingLocks;
	uint32 TotalMemoryOutstanding;

	FLockTracker()
	{
		TotalMemoryOutstanding = 0;
	}

	FORCEINLINE_DEBUGGABLE void Lock(void* RHIBuffer, void* Buffer, uint32 Offset, uint32 SizeRHI, EResourceLockMode LockMode)
	{
#if DO_CHECK
		for (auto& Parms : OutstandingLocks)
		{
			check(Parms.RHIBuffer != RHIBuffer);
		}
#endif
		OutstandingLocks.Add(FLockParams(RHIBuffer, Buffer, Offset, SizeRHI, LockMode));
		TotalMemoryOutstanding += SizeRHI;
	}
	FORCEINLINE_DEBUGGABLE FLockParams Unlock(void* RHIBuffer)
	{
		for (int32 Index = 0; Index < OutstandingLocks.Num(); Index++)
		{
			if (OutstandingLocks[Index].RHIBuffer == RHIBuffer)
			{
				FLockParams Result = OutstandingLocks[Index];
				OutstandingLocks.RemoveAtSwap(Index, 1, false);
				return Result;
			}
		}
		check(!"Mismatched RHI buffer locks.");
		return FLockParams(nullptr, nullptr, 0, 0, RLM_WriteOnly);
	}
};

enum class ECmdList
{
	EGfx,
	ECompute,
};

class IRHICommandContextContainer
{
public:
	virtual ~IRHICommandContextContainer()
	{
	}

	virtual IRHICommandContext* GetContext()
	{
		return nullptr;
	}

	virtual void SubmitAndFreeContextContainer(int32 Index, int32 Num)
	{
		check(0);
	}

	virtual void FinishContext()
	{
		check(0);
	}
};

struct FRHICommandListDebugContext
{
	FRHICommandListDebugContext()
	{
#if RHI_COMMAND_LIST_DEBUG_TRACES
		DebugStringStore[MaxDebugStoreSize] = 1337;
#endif
	}

	void PushMarker(const TCHAR* Marker)
	{
#if RHI_COMMAND_LIST_DEBUG_TRACES
		//allocate a new slot for the stack of pointers
		//and preserve the top of the stack in case we reach the limit
		if (++DebugMarkerStackIndex >= MaxDebugMarkerStackDepth)
		{
			for (uint32 i = 1; i < MaxDebugMarkerStackDepth; i++)
			{
				DebugMarkerStack[i - 1] = DebugMarkerStack[i];
				DebugMarkerSizes[i - 1] = DebugMarkerSizes[i];
			}
			DebugMarkerStackIndex = MaxDebugMarkerStackDepth - 1;
		}

		//try and copy the sting into the debugstore on the stack
		TCHAR* Offset = &DebugStringStore[DebugStoreOffset];
		uint32 MaxLength = MaxDebugStoreSize - DebugStoreOffset;
		uint32 Length = TryCopyString(Offset, Marker, MaxLength) + 1;

		//if we reached the end reset to the start and try again
		if (Length >= MaxLength)
		{
			DebugStoreOffset = 0;
			Offset = &DebugStringStore[DebugStoreOffset];
			MaxLength = MaxDebugStoreSize;
			Length = TryCopyString(Offset, Marker, MaxLength) + 1;

			//if the sting was bigger than the size of the store just terminate what we have
			if (Length >= MaxDebugStoreSize)
			{
				DebugStringStore[MaxDebugStoreSize - 1] = TEXT('\0');
			}
		}

		//add the string to the stack
		DebugMarkerStack[DebugMarkerStackIndex] = Offset;
		DebugStoreOffset += Length;
		DebugMarkerSizes[DebugMarkerStackIndex] = Length;

		check(DebugStringStore[MaxDebugStoreSize] == 1337);
#endif
	}

	void PopMarker()
	{
#if RHI_COMMAND_LIST_DEBUG_TRACES
		//clean out the debug stack if we have valid data
		if (DebugMarkerStackIndex >= 0 && DebugMarkerStackIndex < MaxDebugMarkerStackDepth)
		{
			DebugMarkerStack[DebugMarkerStackIndex] = nullptr;
			//also free the data in the store to postpone wrapping as much as possibler
			DebugStoreOffset -= DebugMarkerSizes[DebugMarkerStackIndex];

			//in case we already wrapped in the past just assume we start allover again
			if (DebugStoreOffset >= MaxDebugStoreSize)
			{
				DebugStoreOffset = 0;
			}
		}

		//pop the stack pointer
		if (--DebugMarkerStackIndex == (~0u) - 1)
		{
			//in case we wrapped in the past just restart
			DebugMarkerStackIndex = ~0u;
		}
#endif
	}

#if RHI_COMMAND_LIST_DEBUG_TRACES
private:

	//Tries to copy a string and early exits if it hits the limit. 
	//Returns the size of the string or the limit when reached.
	uint32 TryCopyString(TCHAR* Dest, const TCHAR* Source, uint32 MaxLength)
	{
		uint32 Length = 0;
		while (Source[Length] != TEXT('\0') && Length < MaxLength)
		{
			Dest[Length] = Source[Length];
			Length++;
		}

		if (Length < MaxLength)
		{
			Dest[Length] = TEXT('\0');
		}
		return Length;
	}

	uint32 DebugStoreOffset = 0;
	static constexpr int MaxDebugStoreSize = 1023;
	TCHAR DebugStringStore[MaxDebugStoreSize + 1];

	uint32 DebugMarkerStackIndex = ~0u;
	static constexpr int MaxDebugMarkerStackDepth = 32;
	const TCHAR* DebugMarkerStack[MaxDebugMarkerStackDepth] = {};
	uint32 DebugMarkerSizes[MaxDebugMarkerStackDepth] = {};
#endif
};

struct FRHICommandBase
{
	FRHICommandBase* Next = nullptr;
	virtual void ExecuteAndDestruct(FRHICommandListBase& CmdList, FRHICommandListDebugContext& DebugContext) = 0;
};

// Thread-safe allocator for GPU fences used in deferred command list execution
// Fences are stored in a ringbuffer
class X4_API FRHICommandListFenceAllocator
{
public:
	static const int MAX_FENCE_INDICES = 4096;
	FRHICommandListFenceAllocator()
	{
		CurrentFenceIndex = 0;
		for (int i = 0; i < MAX_FENCE_INDICES; i++)
		{
			FenceIDs[i] = 0xffffffffffffffffull;
			FenceFrameNumber[i] = 0xffffffff;
		}
	}

	uint32 AllocFenceIndex()
	{
		//check(IsInRenderingThread());
		uint32 FenceIndex = (FPlatformAtomics::InterlockedIncrement(&CurrentFenceIndex) - 1) % MAX_FENCE_INDICES;
		check(FenceFrameNumber[FenceIndex] != GFrameNumberRenderThread);
		FenceFrameNumber[FenceIndex] = GFrameNumberRenderThread;

		return FenceIndex;
	}

	volatile uint64& GetFenceID(int32 FenceIndex)
	{
		check(FenceIndex < MAX_FENCE_INDICES);
		return FenceIDs[FenceIndex];
	}

private:
	volatile int32 CurrentFenceIndex;
	uint64 FenceIDs[MAX_FENCE_INDICES];
	uint32 FenceFrameNumber[MAX_FENCE_INDICES];
};

extern X4_API FRHICommandListFenceAllocator GRHIFenceAllocator;

class X4_API FRHICommandListBase : public FNoncopyable
{
public:
private:
	FRHICommandBase* Root;
	FRHICommandBase** CommandLink;
	bool bExecuting;
	uint32 NumCommands;
	uint32 UID;
	IRHICommandContext* Context;
	IRHIComputeContext* ComputeContext;

};