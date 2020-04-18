#pragma once

#include "CoreTypes.h"
#include "GenericPlatform/GenericPlatformTLS.h"
#include "Windows/WindowsSystemIncludes.h"

#include "X0.h"

struct X0_API FWindowsPlatformTLS
	: public FGenericPlatformTLS
{
	static inline uint32 GetCurrentThreadId(void)
	{
		return Windows::GetCurrentThreadId();
	}

	static inline uint32 AllocTlsSlot(void)
	{
		return Windows::TlsAlloc();
	}

	static inline void SetTlsValue(uint32 SlotIndex, void* Value)
	{
		Windows::TlsSetValue(SlotIndex, Value);
	}

	static inline void* GetTlsValue(uint32 SlotIndex)
	{
		return Windows::TlsGetValue(SlotIndex);
	}

	static inline void FreeTlsSlot(uint32 SlotIndex)
	{
		Windows::TlsFree(SlotIndex);
	}
};

typedef FWindowsPlatformTLS FPlatformTLS;