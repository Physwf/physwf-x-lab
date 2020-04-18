#pragma once

#include "CoreTypes.h"

struct FGenericPlatformTLS
{
	static inline bool IsValidTlsSlot(uint32 SlotIndex)
	{
		return SlotIndex != 0xFFFFFFFF;
	}
};