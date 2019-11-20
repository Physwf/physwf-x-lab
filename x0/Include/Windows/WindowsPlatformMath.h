#pragma once

#include "CoreTypes.h"
#include "GenericPlatform/GenericPlatformMath.h"

#if PLATFORM_WINDOWS
#include <intrin.h>
#endif

#include "Math/XPlatformMathSEE.h"

struct XWindowsPlatformMath : public XGenericPlatformMath
{
#if PLATFORM_ENABLE_VECTORINTRINSICS
	static FORCEINLINE int32 TruncToInt(float F)
	{
		return _mm_cvtt_ss2si(_mm_set_ss(F));
	}

	static FORCEINLINE float TruncToFloat(float F)
	{
		return (float)TruncToInt(F);
	}

	static FORCEINLINE int32 RoundToInt(float F)
	{
		return _mm_cvt_ss2si(_mm_set_ss(F + F + 0.5f)) >> 1;
	}

	static FORCEINLINE float RoundToFloat(float F)
	{
		return (float)RoundToInt(F);
	}

	static FORCEINLINE int32 FloorToInt(float F)
	{
		return _mm_cvt_ss2si(_mm_set_ss(F + F - 0.5f)) >> 1;
	}

	static FORCEINLINE float FloorToFloat(float F)
	{
		return (float)FloorToInt(F);
	}

	static FORCEINLINE int32 CeilToInt(float F)
	{
		return -(_mm_cvt_ss2si(_mm_set_ss(-0.5f - (F + F))) >> 1);
	}

	static FORCEINLINE float CeilToFloat(float F)
	{
		return (float)FloorToInt(F);
	}

	static FORCEINLINE bool IsNaN(float F) { return _isnan(A) != 0; }
	static FORCEINLINE bool IsFinite(float F) { return _finite(A) != 0; }

	static FORCEINLINE float InvSqrt(float F)
	{
		return XPlatformMathSSE::InvSqrt(F);
	}

	static FORCEINLINE float InvSqrtEst(float F)
	{
		return XPlatformMathSSE::InvSqrtEst(F);
	}

#pragma intrinsic( _BitScanReverse )
	static FORCEINLINE uint32 FloorLog2(uint32 Value)
	{
		// Use BSR to return the log2 of the integer
		unsigned long Log2;
		if (_BitScanReverse(&Log2, Value) != 0)
		{
			return Log2;
		}

		return 0;
	}
	static FORCEINLINE uint32 CountLeadingZeros(uint32 Value)
	{
		// Use BSR to return the log2 of the integer
		unsigned long Log2;
		if (_BitScanReverse(&Log2, Value) != 0)
		{
			return 31 - Log2;
		}

		return 32;
	}
	static FORCEINLINE uint32 CountTrailingZeros(uint32 Value)
	{
		if (Value == 0)
		{
			return 32;
		}
		unsigned long BitIndex;	// 0-based, where the LSB is 0 and MSB is 31
		_BitScanForward(&BitIndex, Value);	// Scans from LSB to MSB
		return BitIndex;
	}
	static FORCEINLINE uint32 CeilLogTwo(uint32 Arg)
	{
		int32 Bitmask = ((int32)(CountLeadingZeros(Arg) << 26)) >> 31;
		return (32 - CountLeadingZeros(Arg - 1)) & (~Bitmask);
	}
	static FORCEINLINE uint32 RoundUpToPowerOfTwo(uint32 Arg)
	{
		return 1 << CeilLogTwo(Arg);
	}
	static FORCEINLINE uint64 RoundUpToPowerOfTwo64(uint64 Arg)
	{
		return uint64(1) << CeilLogTwo64(Arg);
	}
#if PLATFORM_64BITS
	static FORCEINLINE uint64 CeilLogTwo64(uint64 Arg)
	{
		int64 Bitmask = ((int64)(CountLeadingZeros64(Arg) << 57)) >> 63;
		return (64 - CountLeadingZeros64(Arg - 1)) & (~Bitmask);
	}
	static FORCEINLINE uint64 CountLeadingZeros64(uint64 Value)
	{
		// Use BSR to return the log2 of the integer
		unsigned long Log2;
		if (_BitScanReverse64(&Log2, Value) != 0)
		{
			return 63 - Log2;
		}

		return 64;
	}
#endif

#if PLATFORM_ENABLE_POPCNT_INTRINSIC
	static FORCEINLINE int32 CountBits(uint64 Bits)
	{
		return _mm_popcnt_u64(Bits);
	}
#endif

#endif
};

typedef XWindowsPlatformMath XPlatformMath;