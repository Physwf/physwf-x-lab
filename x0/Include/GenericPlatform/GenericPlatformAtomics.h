#pragma once

#include "CoreTypes.h"

struct alignas(16) FInt128
{
	int64 Low;
	int64 High;
};

struct FGenericPlatformAtomics
{
	static inline bool CanUseCompareExchange128()
	{
		return false;
	}

protected:
	/**
	 * Checks if a pointer is aligned and can be used with atomic functions.
	 *
	 * @param Ptr - The pointer to check.
	 *
	 * @return true if the pointer is aligned, false otherwise.
	 */
	static inline bool IsAligned(const volatile void* Ptr, const uint32 Alignment = sizeof(void*))
	{
		return !(PTRINT(Ptr) & (Alignment - 1));
	}
};