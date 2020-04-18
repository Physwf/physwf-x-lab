#pragma once

#include "CoreTypes.h"
//#include "Misc/OutputDevice.h"
#include "HAL/PlatformAtomics.h"
//#include "Misc/Exec.h"
#include "Templates/Atomic.h"

#include "X0.h"

enum
{
	// Default allocator alignment. If the default is specified, the allocator applies to engine rules.
	// Blocks >= 16 bytes will be 16-byte-aligned, Blocks < 16 will be 8-byte aligned. If the allocator does
	// not support allocation alignment, the alignment will be ignored.
	DEFAULT_ALIGNMENT = 0,

	// Minimum allocator alignment
	MIN_ALIGNMENT = 8,
};

/** The global memory allocator. */
X0_API extern class FMalloc* GMalloc;
X0_API extern class FMalloc** GFixedMallocLocationPtr;

/** Global FMallocProfiler variable to allow multiple malloc profilers to communicate. */
//MALLOC_PROFILER(CORE_API extern class FMallocProfiler* GMallocProfiler; )

struct FGenericMemoryStats;

class X0_API FUseSystemMallocForNew
{
public:
	void* operator new(size_t Size);

	void operator delete(void* ptr);

	void* operator new[](size_t);

	void operator delete[](void* Ptr);
};

class X0_API FMalloc :
	public FUseSystemMallocForNew
	//public FExec
{
public:
	virtual void* Malloc(SIZE_T Count, uint32 Alignment = DEFAULT_ALIGNMENT) = 0;

	virtual void* Realloc(void* Original, SIZE_T Count, uint32 Alignment = DEFAULT_ALIGNMENT) = 0;

	virtual void Free(void* Original) = 0;

	virtual SIZE_T QuantizeSize(SIZE_T Count, uint32 Alignment)
	{
		return Count;
	}

	virtual bool GetAllocationSize(void* Original, SIZE_T& SizeOut)
	{
		return false;
	}

	virtual void Trim()
	{
	}

	virtual void SetupTLSCachesOnCurrentThread()
	{
	}

	virtual void ClearAndDisableTLSCachesOnCurrentThread()
	{
	}

	virtual void InitializeStatsMetadata();

	virtual void UpdateStats();

	virtual void GetAllocatorStats(FGenericMemoryStats& out_Stats);

	virtual bool IsInternallyThreadSafe() const
	{
		return false;
	}

	virtual bool ValidateHeap()
	{
		return(true);
	}

	virtual const TCHAR* GetDescriptiveName()
	{
		return TEXT("Unspecified allocator");
	}

protected:
	friend struct FCurrentFrameCalls;

	inline void IncrementTotalMallocCalls()
	{
		++TotalMallocCalls;
	}

	inline void IncrementTotalFreeCalls()
	{
		++TotalFreeCalls;
	}

	inline void IncrementTotalReallocCalls()
	{
		++TotalReallocCalls;
	}

	/** Total number of calls Malloc, if implemented by derived class. */
	static TAtomic<uint32> TotalMallocCalls;
	/** Total number of calls Malloc, if implemented by derived class. */
	static TAtomic<uint32> TotalFreeCalls;
	/** Total number of calls Malloc, if implemented by derived class. */
	static TAtomic<uint32> TotalReallocCalls;

#if !UE_BUILD_SHIPPING
public:
	/** Limits the maximum single allocation, to this many bytes, for debugging */
	static TAtomic<uint64> MaxSingleAlloc;
#endif
};