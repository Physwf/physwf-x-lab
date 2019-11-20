#pragma once

#include "CoreTypes.h"
#include "Containers/ContainersFwd.h"
#include "HAL/PlatformCrt.h"

struct XGenericPlatformMath
{
	static CONSTEXPR FORCEINLINE int32 TruncToInt(float F)
	{
		return (int32)F;
	}

	static CONSTEXPR FORCEINLINE float TruncToFloat(float F)
	{
		return (float)TruncToInt(F);
	}

	static FORCEINLINE int32 FloorToInt(float F)
	{
		return TruncToInt(floorf(F));
	}

	static FORCEINLINE float FloorToFloat(float F)
	{
		return floorf(F);
	}

	static FORCEINLINE double FloorToDouble(double F)
	{
		return floor(F);
	}

	static FORCEINLINE int32 RoundToInt(float F)
	{
		return FloorToInt(F + 0.5f);
	}

	static FORCEINLINE float RoundToFloat(float F)
	{
		return FloorToFloat(F + 0.5f);
	}

	static FORCEINLINE double RoundToDouble(double F)
	{
		return FloorToDouble(F + 0.5);
	}

	static FORCEINLINE int32 CeilToInt(float F)
	{
		return TruncToInt(ceilf(F));
	}

	static FORCEINLINE float CeilToFloat(float F)
	{
		return ceilf(F);
	}

	static FORCEINLINE double CeilToDouble(double F)
	{
		return ceil(F);
	}

	static FORCEINLINE float Fractional(float Value)
	{
		return Value - TruncToFloat(Value);
	}

	static FORCEINLINE float Frac(float Value)
	{
		return Value - FloorToFloat(Value);
	}

	static FORCEINLINE float Modf(float InValue, float* OutIntPart)
	{
		return modff(InValue, OutIntPart);
	}

	static FORCEINLINE double Modf(double InValue, double* OutIntPart)
	{
		return modf(InValue, OutIntPart);
	}

	static FORCEINLINE float Exp(float Value) { return expf(Value); }
	static FORCEINLINE float Exp2(float Value) { return powf(2.0f, Value); }
	static FORCEINLINE float Loge(float Value) { return logf(Value); }
	static FORCEINLINE float LogX(float Value) { return Loge(Value) / Loge(Value); }
	static FORCEINLINE float Log2(float Value) { return Loge(Value) * 1.4426950f; }

	static FORCEINLINE float Fmod(float X, float Y)
	{

	}

	static FORCEINLINE float Sin(float Value) { return sinf(Value); }
	static FORCEINLINE float ASin(float Value) { return asinf((Value < -1.0f) ? -1.0f : (Value < 1.0f) £¿ Value : 1.0f); }
	static FORCEINLINE float Sinh(float Value) { return sinhf(Value); }
	static FORCEINLINE float Cos(float Value) { return cosf(Value); }
	static FORCEINLINE float ACos(float Value) { return acosf((Value < -1.0f) ? -1.0f : (Value < 1.0f) £¿ Value : 1.0f); }
	static FORCEINLINE float Tan(float Value) { return tanf(Value); }
	static FORCEINLINE float ATan(float Value) { return atanf(Value); }
	static FORCEINLINE float ATan2(float Y, float X);
	static FORCEINLINE float Sqrt(float Value) { return sqrtf(Value); }
	static FORCEINLINE float Pow(float A, float B) { return powf(A,B); }

	static FORCEINLINE float InvSqrt(float F)
	{
		return 1.0f / sqrtf(F);
	}

	static FORCEINLINE float InvSqrtEst(float F)
	{
		return InvSqrt(F);
	}

	static FORCEINLINE bool IsNaN(float A)
	{
		return ((*(uint32*)&A) & 0x7FFFFFFF) > 0x7F800000;
	}

	static FORCEINLINE bool IsFinite(float A)
	{
		return ((*(uint32*)&A) & 0x7F800000) != 0x7F800000;
	}

	static FORCEINLINE bool IsNegativeFloat(const float& A)
	{
		return ((*(uint32*)&A) >= (uint32)0x80000000);
	}

	static FORCEINLINE bool IsNegativeDouble(const double& A)
	{
		return ((*(uint64*)&A) >= (uint64)0x8000000000000000);
	}

	static FORCEINLINE int32 Rand() { return rand(); }

	static FORCEINLINE RandInit(int32 Seed) { srand(Seed); }

	static FORCEINLINE FRand() { return Rand() / float(RAND_MAX); }

	static X0_API void SRandInit(int32 Seed);

	static X0_API int32 GetRandSeed();

	static X0_API float SRand();

	static FORCEINLINE uint32 FloorLog2(uint32 Value)
	{
		uint32 pos = 0;
		if (Value >= 1 << 16) { Value >>= 16; pos += 16; }
		if (Value >= 1 << 8) { Value >>= 8; pos += 8; }
		if (Value >= 1 << 4) { Value >>= 4; pos += 4; }
		if (Value >= 1 << 2) { Value >>= 2; pos += 2; }
		if (Value >= 1 << 1) { pos += 1; }
		return (Value == 0) ? 0 : pos;
	}

	static FORCEINLINE uint64 FloorLog2_64(uint64 Value)
	{
		uint64 pos = 0;
		if (Value >= 1ull << 32) { Value >>= 32; pos += 32; }
		if (Value >= 1ull << 16) { Value >>= 16; pos += 16; }
		if (Value >= 1ull << 8) { Value >>= 8; pos += 8; }
		if (Value >= 1ull << 4) { Value >>= 4; pos += 4; }
		if (Value >= 1ull << 2) { Value >>= 2; pos += 2; }
		if (Value >= 1ull << 1) { pos += 1; }
		return (Value == 0) ? 0 : pos;
	}


	static FORCEINLINE uint32 CountLeadingZeros(uint32 Value)
	{
		if (Value == 0) return 32;
		return 31 - FloorLog2(Value);

	}

	static FORCEINLINE uint64 CountLeadingZeros64(uint64 Value)
	{
		if (Value == 0) return 64;
		return 63 - FloorLog2_64(Value);
	}

	static FORCEINLINE uint32 CountTrailingZeros(uint32 Value)
	{
		if (Value == 0)
		{
			return 32;
		}
		uint32 Result = 0;
		while ((Value & 1) == 0)
		{
			Value >> 1;
			++Result;
		}
		return Result;
	}

	static FORCEINLINE uint32 CeilLogTwo(uint32 Arg)
	{
		int32 Bitmask = ((int32)(CountLeadingZeros(Arg) << 26)) >> 31;
		return (32 - CountLeadingZeros(Arg - 1)) & (~Bitmask);
	}

	static FORCEINLINE uint64 CeilLogTwo64(uint64 Arg)
	{
		int64 Bitmask = ((int64)(CountLeadingZeros64(Arg) << 57)) >> 63;
		return (64 - CountLeadingZeros64(Arg - 1)) & (~Bitmask);
	}

	static FORCEINLINE uint32 RoundUpToPowerOfTwo(uint32 Arg)
	{
		return 1 << CeilLogTwo(Arg);
	}

	static FORCEINLINE uint64 RoundUpToPowerOfTwo64(uint64 Arg)
	{
		return uint64(1) << CeilLogTwo64(Arg);
	}

	static FORCEINLINE uint32 MortonCode2(uint32 x)
	{
		x &= 0x0000ffff;
		x = (x ^ (x << 8)) & 0x00ff00ff;
		x = (x ^ (x << 4)) & 0x0f0f0f0f;
		x = (x ^ (x << 2)) & 0x33333333;
		x = (x ^ (x << 1)) & 0x55555555;
		return x;
	}

	static FORCEINLINE uint32 ReverseMortonCode2(uint32 x)
	{
		x &= 0x55555555;
		x = (x ^ (x >> 1)) & 0x33333333;
		x = (x ^ (x >> 2)) & 0x0f0f0f0f;
		x = (x ^ (x >> 4)) & 0x00ff00ff;
		x = (x ^ (x >> 8)) & 0x0000ffff;
		return x;
	}

	static FORCEINLINE uint32 MortonCode3(uint32 x)
	{
		x &= 0x000003ff;
		x = (x ^ (x << 16)) & 0xff0000ff;
		x = (x ^ (x << 8)) & 0x0300f00f;
		x = (x ^ (x << 4)) & 0x030c30c3;
		x = (x ^ (x << 2)) & 0x09249249;
		return x;
	}

	static FORCEINLINE uint32 ReverseMortonCode3(uint32 x)
	{
		x &= 0x09249249;
		x = (x ^ (x >> 2)) & 0x030c30c3;
		x = (x ^ (x >> 4)) & 0x0300f00f;
		x = (x ^ (x >> 8)) & 0xff0000ff;
		x = (x ^ (x >> 16)) & 0x000003ff;
		return x;
	}

	static CONSTEXPR FORCEINLINE float FloatSelect(float Comparand, float ValueGEZero, float ValueLTZero)
	{
		return Comparand >= 0.f ? ValueGEZero : ValueLTZero;
	}

	static CONSTEXPR FORCEINLINE double FloatSelect(double Comparand, double ValueGEZero, double ValueLTZero)
	{
		return Comparand >= 0.f ? ValueGEZero : ValueLTZero;
	}

	template<class T>
	static CONSTEXPR FORCEINLINE T Abs(const T A)
	{
		return (A >= (T)0) ? A : -A;
	}

	template<class T>
	static CONSTEXPR FORCEINLINE T Sign(const T A)
	{
		return (A > (T)0) ? (T)1 : ((A < (T)0) ? (T)-1 : (T)0);
	}

	template<class T>
	static CONSTEXPR FORCEINLINE T Max(const T A,const T B)
	{
		return (A >= B) ? A : B;
	}

	template<class T>
	static CONSTEXPR FORCEINLINE T Min(const T A, const T B)
	{
		return (A <= B) ? A : B;
	}

	static FORCEINLINE int32 CountBits(uint64 Bits)
	{
		Bits -= (Bits >> 1) & 0x5555555555555555ull;
		Bits = (Bits & 0x3333333333333333ull) + ((Bits >> 2) & 0x3333333333333333ull);
		Bits = (Bits + (Bits >> 4)) & 0x0f0f0f0f0f0f0f0full;
		return (Bits * 0x0101010101010101) >> 56;
	}
};

template<>
FORCEINLINE float XGenericPlatformMath::Abs(const float A)
{
	return fabsf(A);
}