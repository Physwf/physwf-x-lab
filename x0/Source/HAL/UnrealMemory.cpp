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
//#include "HAL/MallocThreadSafeProxy.h"
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