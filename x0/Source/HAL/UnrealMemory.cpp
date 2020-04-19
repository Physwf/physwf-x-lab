#include "HAL/UnrealMemory.h"
#include "Math/XMathUtility.h"
//#include "Containers/Array.h"
//#include "Logging/LogMacros.h"
#include "HAL/ThreadSafeCounter.h"
//#include "Containers/LockFreeList.h"
//#include "Stats/Stats.h"
//#include "HAL/IConsoleManager.h"

#if USE_MALLOC_PROFILER && WITH_ENGINE && IS_MONOLITHIC
#include "Runtime/Engine/Public/MallocProfilerEx.h"
#endif

/*-----------------------------------------------------------------------------
Memory functions.
-----------------------------------------------------------------------------*/

//#include "ProfilingDebugging/MallocProfiler.h"
#include "HAL/MallocThreadSafeProxy.h"
//#include "HAL/MallocVerify.h"
//#include "HAL/MallocLeakDetectionProxy.h"
//#include "HAL/PlatformMallocCrash.h"
//#include "HAL/MallocPoisonProxy.h"


void* FUseSystemMallocForNew::operator new(size_t Size)
{
	return FMemory::SystemMalloc(Size);
}

void FUseSystemMallocForNew::operator delete(void* Ptr)
{
	FMemory::SystemFree(Ptr);
}

void* FUseSystemMallocForNew::operator new[](size_t Size)
{
	return FMemory::SystemMalloc(Size);
}

void FUseSystemMallocForNew::operator delete[](void* Ptr)
{
	FMemory::SystemFree(Ptr);
}

/** Helper function called on first allocation to create and initialize GMalloc */
int FMemory_GCreateMalloc_ThreadUnsafe()
{
#if !PLATFORM_MAC
	FPlatformMemoryStats Stats = FPlatformMemory::GetStats();
	uint64 ProgramSize = Stats.UsedPhysical;
#endif

	GMalloc = FPlatformMemory::BaseAllocator();
	// Setup malloc crash as soon as possible.
	//FPlatformMallocCrash::Get(GMalloc);

#if PLATFORM_HTML5 // remove this guard for other platforms that can use this check...
	if (!FPlatformProcess::SupportsMultithreading())
	{
		return 0;
	}
#endif

#if PLATFORM_USES_FIXED_GMalloc_CLASS
#if USE_MALLOC_PROFILER || MALLOC_VERIFY || MALLOC_LEAKDETECTION || UE_USE_MALLOC_FILL_BYTES
#error "Turn off PLATFORM_USES_FIXED_GMalloc_CLASS in order to use special allocator proxies"
#endif
	if (!GMalloc->IsInternallyThreadSafe())
	{
		UE_LOG(LogMemory, Fatal, TEXT("PLATFORM_USES_FIXED_GMalloc_CLASS only makes sense for allocators that are internally threadsafe."));
	}
#else
	// so now check to see if we are using a Mem Profiler which wraps the GMalloc
#if USE_MALLOC_PROFILER
#if WITH_ENGINE && IS_MONOLITHIC
	GMallocProfiler = new FMallocProfilerEx(GMalloc);
#else
	GMallocProfiler = new FMallocProfiler(GMalloc);
#endif
	GMallocProfiler->BeginProfiling();
	GMalloc = GMallocProfiler;
#endif

	// if the allocator is already thread safe, there is no need for the thread safe proxy
	if (!GMalloc->IsInternallyThreadSafe())
	{
		GMalloc = new FMallocThreadSafeProxy(GMalloc);
	}

#if MALLOC_VERIFY
	// Add the verifier
	GMalloc = new FMallocVerifyProxy(GMalloc);
#endif

#if MALLOC_LEAKDETECTION
	GMalloc = new FMallocLeakDetectionProxy(GMalloc);
#endif

	// poison memory allocations in Debug and Development non-editor/non-editoronly data builds
#if UE_USE_MALLOC_FILL_BYTES
	GMalloc = new FMallocPoisonProxy(GMalloc);
#endif

#endif

	// On Mac it's too early to log here in some cases. For example GMalloc may be created during initialization of a third party dylib on load, before CoreFoundation is initialized
#if !PLATFORM_DESKTOP
	// by this point, we assume we can log
	double SizeInMb = ProgramSize / (1024.0 * 1024.0);
	FPlatformMisc::LowLevelOutputDebugStringf(TEXT("Used memory before allocating anything was %.2fMB\n"), SizeInMb);
	UE_LOG(LogMemory, Display, TEXT("Used memory before allocating anything was %.2fMB"), SizeInMb);
#endif

	return 0;
}

void FMemory::GCreateMalloc()
{
	// On some platforms (e.g. Mac) GMalloc can be created on multiple threads at once.
	// This admittedly clumsy construct ensures both thread-safe creation and prevents multiple calls into it.
	// The call will not be optimized away in Shipping because the function has side effects (touches global vars).
	static int ThreadSafeCreationResult = FMemory_GCreateMalloc_ThreadUnsafe();
}

void* FMemory::MallocExternal(SIZE_T Count, uint32 Alignment)
{
	if (!GMalloc)
	{
		GCreateMalloc();
		CA_ASSUME(GMalloc != NULL);	// Don't want to assert, but suppress static analysis warnings about potentially NULL GMalloc
	}
	return GMalloc->Malloc(Count, Alignment);
}

void* FMemory::ReallocExternal(void* Original, SIZE_T Count, uint32 Alignment)
{
	if (!GMalloc)
	{
		GCreateMalloc();
		CA_ASSUME(GMalloc != NULL);	// Don't want to assert, but suppress static analysis warnings about potentially NULL GMalloc
	}
	return GMalloc->Realloc(Original, Count, Alignment);
}

void FMemory::FreeExternal(void* Original)
{
	if (!GMalloc)
	{
		GCreateMalloc();
		CA_ASSUME(GMalloc != NULL);	// Don't want to assert, but suppress static analysis warnings about potentially NULL GMalloc
	}
	if (Original)
	{
		GMalloc->Free(Original);
	}
}

SIZE_T FMemory::GetAllocSizeExternal(void* Original)
{
	if (!GMalloc)
	{
		GCreateMalloc();
		CA_ASSUME(GMalloc != NULL);	// Don't want to assert, but suppress static analysis warnings about potentially NULL GMalloc
	}
	SIZE_T Size = 0;
	return GMalloc->GetAllocationSize(Original, Size) ? Size : 0;
}

SIZE_T FMemory::QuantizeSizeExternal(SIZE_T Count, uint32 Alignment)
{
	if (!GMalloc)
	{
		GCreateMalloc();
		CA_ASSUME(GMalloc != NULL);	// Don't want to assert, but suppress static analysis warnings about potentially NULL GMalloc
	}
	return GMalloc->QuantizeSize(Count, Alignment);
}


void FMemory::Trim()
{
	if (!GMalloc)
	{
		GCreateMalloc();
		CA_ASSUME(GMalloc != NULL);	// Don't want to assert, but suppress static analysis warnings about potentially NULL GMalloc
	}
	//QUICK_SCOPE_CYCLE_COUNTER(STAT_FMemory_Trim);
	GMalloc->Trim();
}

void FMemory::SetupTLSCachesOnCurrentThread()
{
	if (!GMalloc)
	{
		GCreateMalloc();
		CA_ASSUME(GMalloc != NULL);	// Don't want to assert, but suppress static analysis warnings about potentially NULL GMalloc
	}
	GMalloc->SetupTLSCachesOnCurrentThread();
}

void FMemory::ClearAndDisableTLSCachesOnCurrentThread()
{
	if (GMalloc)
	{
		GMalloc->ClearAndDisableTLSCachesOnCurrentThread();
	}
}

void* FMemory::GPUMalloc(SIZE_T Count, uint32 Alignment /* = DEFAULT_ALIGNMENT */)
{
	return FPlatformMemory::GPUMalloc(Count, Alignment);
}

void* FMemory::GPURealloc(void* Original, SIZE_T Count, uint32 Alignment /* = DEFAULT_ALIGNMENT */)
{
	return FPlatformMemory::GPURealloc(Original, Count, Alignment);
}

void FMemory::GPUFree(void* Original)
{
	return FPlatformMemory::GPUFree(Original);
}

#if !INLINE_FMEMORY_OPERATION && !PLATFORM_USES_FIXED_GMalloc_CLASS
#include "HAL/FMemory.inl"
#endif
