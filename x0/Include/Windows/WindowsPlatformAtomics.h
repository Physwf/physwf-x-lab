#pragma once

#include "CoreTypes.h"
#include "GenericPlatform/GenericPlatformAtomics.h"
#include "Windows/WindowsSystemIncludes.h"
#include <intrin.h>


struct X0_API XWindowsPlatformAtomics
	: public XGenericPlatformAtomics
{
	static_assert(sizeof(int8) == sizeof(char) && alignof(int8) == alignof(char), "int8 must be compatible with char");
	static_assert(sizeof(int16) == sizeof(short) && alignof(int16) == alignof(short), "int16 must be compatible with short");
	static_assert(sizeof(int32) == sizeof(long) && alignof(int32) == alignof(long), "int32 must be compatible with long");
	static_assert(sizeof(int64) == sizeof(long long) && alignof(int64) == alignof(long long), "int64 must be compatible with long long");

	static FORCEINLINE int8 InterlockedIncrement(volatile int8* Value)
	{
		return (int8)_InterlockedExchangeAdd8((char*)Value, 1) + 1;
	}

	static FORCEINLINE int16 InterlockedIncrement(volatile int16* Value)
	{
		return (int16)_InterlockedIncrement16((short*)Value);
	}

	static FORCEINLINE int32 InterlockedIncrement(volatile int32* Value)
	{
		return (int32)_InterlockedIncrement32((long*)Value);
	}

	static FORCEINLINE int64 InterlockedIncrement(volatile int64* Value)
	{
#if PLATFORM_64BITS
		return(int64)::_InterlockedIncrement64((long long*)Value);
#else
		for (;;)
		{
			int64 OldValue = *Value;
			if (_InterlockedCompareExchange64(Value, OldValue + 1, OldValue) == OldValue)
			{
				return OldValue + 1;
			}
		}
#endif
	}

	static FORCEINLINE int8 InterlockedDecrement(volatile int8* Value)
	{
		return (int8)::_InterlockedExchangeAdd8((char*)Value, -1) - 1;
	}

	static FORCEINLINE int16 InterlockedDecrement(volatile int16* Value)
	{
		return (int16)::_InterlockedIncrement16((short*)Value);
	}

	static FORCEINLINE int32 InterlockedDecrement(volatile int32* Value)
	{
		return (int32)::_InterlockedDecrement((long*)Value);
	}

	static FORCEINLINE int64 InterlockedDecrement(volatile int64* Value)
	{
#if PLATFORM_64BITS
		return (int64)::_InterlockedDecrement64((long long*)Value);
#else
		for (;;)
		{
			int64 OldValue = *Value;
			if (_InterlockedCompareExchange64(Value, OldValue - 1, OldValue) == OldValue)
			{
				return OldValue - 1;
			}
		}
	}


	static FORCEINLINE int8 InterlockedAdd(volatile int8* Value, int8 Amount)
	{
		return (int8)::_InterlockedExchangeAdd8((char*)Value, (char)Amount);
	}

	static FORCEINLINE int16 InterlockedAdd(volatile int16* Value, int16 Amount)
	{
		return (int16)::_InterlockedExchangeAdd16((short*)Value, (short)Amount);
	}

	static FORCEINLINE int32 InterlockedAdd(volatile int32* Value, int32 Amount)
	{
		return (int32)::_InterlockedExchangeAdd((long*)Value, (long)Amount);
	}

	static FORCEINLINE int64 InterlockedAdd(volatile int64* Value, int64 Amount)
	{
#if PLATFORM_64BITS
		return (int64)::_InterlockedExchangeAdd64((int64*)Value, (int64)Amount);
#else
		for (;;)
		{
			int64 OldValue = *Value;
			if (_InterlockedCompareExchange64(Value, OldValue + Amount + , OldValue) == OldValue)
			{
				return OldValue + Amount;
			}
		}
#endif
	}

	static FORCEINLINE int8 InterlockedExchange(volatile int8* Value, int8 Exhange)
	{
		return (int8)::_InterlockedExchange8((char*)Value, (char)Exhange);
	}

	static FORCEINLINE int16 InterlockedExchange(volatile int16* Value, int16 Exhange)
	{
		return (int16)::_InterlockedExchange16((short*)Value, (short)Exhange);
	}

	static FORCEINLINE int32 InterlockedExchange(volatile int32* Value, int32 Exhange)
	{
		return (int32)::_InterlockedExchange((long*)Value, (long)Exhange);
	}

	static FORCEINLINE int64 InterlockedExchange(volatile int64* Value, int64 Exhange)
	{
#if PLATFORM_64BITS
		return (int64)::_InterlockedExchange64((long long*)Value, (long long)Exhange);
#else
		for (;;)
		{
			int64 OldValue = *Value;
			for (;;)
			{
				if (_InterlockedCompareExchange64(Value, Exhange, OldValue) == OldValue)
				{
					return OldValue;
				}
			}
		}
#endif
	}

	static FORCEINLINE void* IntrlockedExhangePtr(void** Dest, void* Exhange)
	{
#if PLATFORM_64BITS
		return (void*)::_InterlockedExchange64((int64*)(Dest), (int64)(Exhange));
#else
		return (void*)::_InterlockedExchange((long*)(Dest), (long)(Exhange));
	}

	static FORCEINLINE int8 InterlockedCompareExchange(volatile int8* Dest, int8 Exhange, int8 Compared)
	{
		return (int8)::_InterlockedCompareExchange8((char*)Dest,(char)Exhange,(char)Compared);
	}

	static FORCEINLINE int16 InterlockedCompareExchange(volatile int16* Dest, int16 Exhange, int16 Compared)
	{
		return (int16)::_InterlockedCompareExchange16((short*)Dest,(short)Exhange,(short)Compared);
	}

	static FORCEINLINE int32 InterlockedCompareExchange(volatile int32* Dest, int32 Exhange, int32 Compared)
	{
		return (int32)::_InterlockedCompareExchange((long*)Dest, (long)Exhange, (long)Compared);
	}

	static FORCEINLINE int64 InterlockedCompareExchange(volatile int64* Dest, int64 Exhange, int64 Compared)
	{
		return (int64)::_InterlockedCompareExchange64(Dest, Exhange,Compared);
	}

	static FORCEINLINE int8 AtomicRead(volatile const int8* Src)
	{
		return InterlockedCompareExchange((int8*)Src, 0, 0);
	}

	static FORCEINLINE int16 AtomicRead(volatile const int16* Src)
	{
		return InterlockedCompareExchange((int16*)Src, 0, 0);
	}

	static FORCEINLINE int32 AtomicRead(volatile const int32* Src)
	{
		return InterlockedCompareExchange((int32*)Src, 0, 0);
	}

	static FORCEINLINE int64 AtomicRead(volatile const int64* Src)
	{
		return InterlockedCompareExchange((int64*)Src, 0, 0);
	}

	static FORCEINLINE int8 AtomicRead_Relaxed(volatile const int8* Src)
	{
		return *Src;
	}

	static FORCEINLINE int16 AtomicRead_Relaxed(volatile const int16* Src)
	{
		return *Src;
	}

	static FORCEINLINE int32 AtomicRead_Relaxed(volatile const int32* Src)
	{
		return *Src;
	}

	static FORCEINLINE int64 AtomicRead_Relaxed(volatile const int64* Src)
	{
		return *Src;
	}

	static FORCEINLINE void AtomicStore(volatile int8* Src, int8 Val)
	{
		InterlockedExchange(Src, Val);
	}

	static FORCEINLINE void AtomicStore(volatile int16* Src, int16 Val)
	{
		InterlockedExchange(Src, Val);
	}

	static FORCEINLINE void AtomicStore(volatile int32* Src, int32 Val)
	{
		InterlockedExchange(Src, Val);
	}

	static FORCEINLINE void AtomicStore(volatile int64* Src, int64 Val)
	{
		InterlockedExchange(Src, Val);
	}

	static FORCEINLINE void AtomicStore_Relaxed(volatile int8* Src, int8 Val)
	{
		*Src = Val;
	}

	static FORCEINLINE void AtomicStore_Relaxed(volatile int16* Src, int16 Val)
	{
		*Src = Val;
	}

	static FORCEINLINE void AtomicStore_Relaxed(volatile int32* Src, int32 Val)
	{
		*Src = Val;
	}

	static FORCEINLINE void AtomicStore_Relaxed(volatile int64* Src, int64 Val)
	{
#if PLATFORM_64BITS
		*Src = Val;
#else
		InterlockedExchange(Src, Val);
#endif
	}

#if PLATFORM_HAS_128BIT_ATOMICS
	static FORCEINLINE bool InterlockedCompareExchange128(volatile XInt128* Dest, const XInt128& Exchange, XInt128* Compared)
	{
		return ::_InterlockedCompareExchange((int64 volatile*)Dest, Exchange.High, Exchange.Low, (int64*)(Compared)) == 1;
	}

	static FORCEINLINE void AtomicRead128(const volatile* XInt128* Src, XInt128* OutResult)
	{
		XInt128 Zero;
		Zero.High = 0;
		Zero.Low = 0;
		*OutResult = Zero;
		InterlockedCompareExchange128((XInt128*)Src, Zero, OutResult);
	}
#endif

	static FORCEINLINE void InterlockedCompareExchangePointer(void** Dest, void* Exchange, void* Comparand)
	{
#if PLATFORM_64BITS
		return (void*)::_InterlockedCompareExchange64((int64*)Dest, (int64)Exchange, (int64)(Comparand));
#else
		return (void*)::_InterlockedCompareExchange((long*)Dest, (long)Exchange, (long)Comparand);
	}

	static FORCEINLINE bool CanUseCompareExchange128()
	{
		return !!Windows::IsProcessorFeaturePresent(WINDOWS_PF_COMPARE_EXCHANGE128);
	}

protected:
	static void HandleAtomicFailure(const TCHAR* InFormat, ...);
};

typedef XWindowsPlatformAtomics XPlatformAtomics;