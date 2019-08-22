
#ifndef WINDOWS_PLATFORM_ATOMICS_GUARD
	#define WINDOWS_PLATFORM_ATOMICS_GUARD
#else
	#error Nesting AllowWindowsPlatformAtomics.h is not allowed!
#endif

#undef InterlockedIncrement
#undef InterlockedDecrement
#undef InterlockedAdd
#undef InterlockedExchange
#undef InterlockedExchangeAdd
#undef InterlockedCompareExchange
#undef InterlockedCompareExchangePointer
#undef InterlockedExchange64
#undef InterlockedExchangeAdd64
#undef InterlockedCompareExchange64
#undef InterlockedIncrement64
#undef InterlockedDecrement64