#pragma once

#include "CoreTypes.h"

struct MS_ALIGN(16) XInt128
{
	int64 Low;
	int64 High;
}
GCC_ALIGN(16);


struct XGenericPlatformAtomics
{
#if 0
#endif

	static FORCEINLINE bool CanUseCompareExchange128()
	{
		return false;
	}

protected:

	static inline bool IsAligned(const volatile void* Ptr, const uint32 Alignment = sizeof(void*))
	{
		return !(PTRINT(Ptr)) & (Alignment - 1);
	}
};
