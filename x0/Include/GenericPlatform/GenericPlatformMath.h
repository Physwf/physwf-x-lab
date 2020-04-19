#pragma once

#include "CoreTypes.h"
#include "Containers/ContainersFwd.h"
#include "HAL/PlatformCrt.h"

struct FGenericPlatformMath
{
	static constexpr inline int32 TruncToInt(float F)
	{
		return (int32)F;
	}

	static constexpr inline float TruncToFloat(float F)
	{
		return (float)TruncToInt(F);
	}

	static inline int32 FloorToInt(float F)
	{
		return TruncToInt(floorf(F));
	}

	static inline float FloorToFloat(float F)
	{
		return floorf(F);
	}

	static inline double FloorToDouble(double F)
	{
		return floor(F);
	}

	static inline int32 RoundToInt(float F)
	{
		return FloorToInt(F + 0.5f);
	}

	static inline float RoundToFloat(float F)
	{
		return FloorToFloat(F + 0.5f);
	}

	static inline double RoundToDouble(double F)
	{
		return FloorToDouble(F + 0.5);
	}

	static inline int32 CeilToInt(float F)
	{
		return TruncToInt(ceilf(F));
	}

	static inline float CeilToFloat(float F)
	{
		return ceilf(F);
	}

	static inline double CeilToDouble(double F)
	{
		return ceil(F);
	}

	static inline float Fractional(float Value)
	{
		return Value - TruncToFloat(Value);
	}

	static inline float Frac(float Value)
	{
		return Value - FloorToFloat(Value);
	}

	static inline float Modf(const float InValue, float* OutIntPart)
	{
		return modff(InValue, OutIntPart);
	}

	static inline double Modf(const double InValue, double* OutIntPart)
	{
		return modf(InValue, OutIntPart);
	}

	// Returns e^Value
	static inline float Exp(float Value) { return expf(Value); }
	// Returns 2^Value
	static inline float Exp2(float Value) { return powf(2.f, Value); /*exp2f(Value);*/ }
	static inline float Loge(float Value) { return logf(Value); }
	static inline float LogX(float Base, float Value) { return Loge(Value) / Loge(Base); }
	// 1.0 / Loge(2) = 1.4426950f
	static inline float Log2(float Value) { return Loge(Value) * 1.4426950f; }

	static inline float Fmod(float X, float Y)
	{
		if (fabsf(Y) <= 1.e-8f)
		{
			//FmodReportError(X, Y);
			return 0.f;
		}
		const float Quotient = TruncToFloat(X / Y);
		float IntPortion = Y * Quotient;

		// Rounding and imprecision could cause IntPortion to exceed X and cause the result to be outside the expected range.
		// For example Fmod(55.8, 9.3) would result in a very small negative value!
		if (fabsf(IntPortion) > fabsf(X))
		{
			IntPortion = X;
		}

		const float Result = X - IntPortion;
		return Result;
	}

	static inline float Sin(float Value) { return sinf(Value); }
	static inline float Asin(float Value) { return asinf((Value < -1.f) ? -1.f : ((Value < 1.f) ? Value : 1.f)); }
	static inline float Cos(float Value) { return cosf(Value); }
	static inline float Acos(float Value) { return acosf((Value < -1.f) ? -1.f : ((Value < 1.f) ? Value : 1.f)); }
	static inline float Tan(float Value) { return tanf(Value); }
	static inline float Atan(float Value) { return atanf(Value); }
	static float Atan2(float Y, float X);
	static inline float Sqrt(float Value) { return sqrtf(Value); }
	static inline float Pow(float A, float B) { return powf(A, B); }

	/** Computes a fully accurate inverse square root */
	static inline float InvSqrt(float F)
	{
		return 1.0f / sqrtf(F);
	}

	/** Computes a faster but less accurate inverse square root */
	static inline float InvSqrtEst(float F)
	{
		return InvSqrt(F);
	}

	static inline bool IsNaN(float A)
	{
		return ((*(uint32*)&A) & 0x7FFFFFFF) > 0x7F800000;
	}
	static inline bool IsFinite(float A)
	{
		return ((*(uint32*)&A) & 0x7F800000) != 0x7F800000;
	}
	static inline bool IsNegativeFloat(const float& A)
	{
		return ((*(uint32*)&A) >= (uint32)0x80000000); // Detects sign bit.
	}

	static inline bool IsNegativeDouble(const double& A)
	{
		return ((*(uint64*)&A) >= (uint64)0x8000000000000000); // Detects sign bit.
	}

	/** Returns a random integer between 0 and RAND_MAX, inclusive */
	static inline int32 Rand() { return rand(); }

	/** Seeds global random number functions Rand() and FRand() */
	static inline void RandInit(int32 Seed) { srand(Seed); }

	/** Returns a random float between 0 and 1, inclusive. */
	static inline float FRand() { return Rand() / (float)RAND_MAX; }

	/** Seeds future calls to SRand() */
	static void SRandInit(int32 Seed);

	/** Returns the current seed for SRand(). */
	static int32 GetRandSeed();

	/** Returns a seeded random float in the range [0,1), using the seed from SRandInit(). */
	static float SRand();

	static inline uint32 FloorLog2(uint32 Value)
	{
		uint32 pos = 0;
		if (Value >= 1 << 16) { Value >>= 16; pos += 16; }
		if (Value >= 1 << 8) { Value >>= 8; pos += 8; }
		if (Value >= 1 << 4) { Value >>= 4; pos += 4; }
		if (Value >= 1 << 2) { Value >>= 2; pos += 2; }
		if (Value >= 1 << 1) { pos += 1; }
		return (Value == 0) ? 0 : pos;
	}

	/**
	 * Computes the base 2 logarithm for a 64-bit value that is greater than 0.
	 * The result is rounded down to the nearest integer.
	 *
	 * @param Value		The value to compute the log of
	 * @return			Log2 of Value. 0 if Value is 0.
	 */
	static inline uint64 FloorLog2_64(uint64 Value)
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

	/**
	 * Counts the number of leading zeros in the bit representation of the value
	 *
	 * @param Value the value to determine the number of leading zeros for
	 *
	 * @return the number of zeros before the first "on" bit
	 */
	static inline uint32 CountLeadingZeros(uint32 Value)
	{
		if (Value == 0) return 32;
		return 31 - FloorLog2(Value);
	}

	/**
	 * Counts the number of leading zeros in the bit representation of the 64-bit value
	 *
	 * @param Value the value to determine the number of leading zeros for
	 *
	 * @return the number of zeros before the first "on" bit
	 */
	static inline uint64 CountLeadingZeros64(uint64 Value)
	{
		if (Value == 0) return 64;
		return 63 - FloorLog2_64(Value);
	}

	/**
	 * Counts the number of trailing zeros in the bit representation of the value
	 *
	 * @param Value the value to determine the number of trailing zeros for
	 *
	 * @return the number of zeros after the last "on" bit
	 */
	static inline uint32 CountTrailingZeros(uint32 Value)
	{
		if (Value == 0)
		{
			return 32;
		}
		uint32 Result = 0;
		while ((Value & 1) == 0)
		{
			Value >>= 1;
			++Result;
		}
		return Result;
	}

	/**
	 * Returns smallest N such that (1<<N)>=Arg.
	 * Note: CeilLogTwo(0)=0 because (1<<0)=1 >= 0.
	 */
	static inline uint32 CeilLogTwo(uint32 Arg)
	{
		int32 Bitmask = ((int32)(CountLeadingZeros(Arg) << 26)) >> 31;
		return (32 - CountLeadingZeros(Arg - 1)) & (~Bitmask);
	}

	static inline uint64 CeilLogTwo64(uint64 Arg)
	{
		int64 Bitmask = ((int64)(CountLeadingZeros64(Arg) << 57)) >> 63;
		return (64 - CountLeadingZeros64(Arg - 1)) & (~Bitmask);
	}

	/** @return Rounds the given number up to the next highest power of two. */
	static inline uint32 RoundUpToPowerOfTwo(uint32 Arg)
	{
		return 1 << CeilLogTwo(Arg);
	}

	static inline uint64 RoundUpToPowerOfTwo64(uint64 V)
	{
		return uint64(1) << CeilLogTwo64(V);
	}

	static constexpr inline float FloatSelect(float Comparand, float ValueGEZero, float ValueLTZero)
	{
		return Comparand >= 0.f ? ValueGEZero : ValueLTZero;
	}

	static constexpr inline double FloatSelect(double Comparand, double ValueGEZero, double ValueLTZero)
	{
		return Comparand >= 0.f ? ValueGEZero : ValueLTZero;
	}

	/** Computes absolute value in a generic way */
	template< class T >
	static constexpr inline T Abs(const T A)
	{
		return (A >= (T)0) ? A : -A;
	}

	/** Returns 1, 0, or -1 depending on relation of T to 0 */
	template< class T >
	static constexpr inline T Sign(const T A)
	{
		return (A > (T)0) ? (T)1 : ((A < (T)0) ? (T)-1 : (T)0);
	}

	/** Returns higher value in a generic way */
	template< class T >
	static constexpr inline T Max(const T A, const T B)
	{
		return (A >= B) ? A : B;
	}

	/** Returns lower value in a generic way */
	template< class T >
	static constexpr inline T Min(const T A, const T B)
	{
		return (A <= B) ? A : B;
	}

	static inline int32 CountBits(uint64 Bits)
	{
		// https://en.wikipedia.org/wiki/Hamming_weight
		Bits -= (Bits >> 1) & 0x5555555555555555ull;
		Bits = (Bits & 0x3333333333333333ull) + ((Bits >> 2) & 0x3333333333333333ull);
		Bits = (Bits + (Bits >> 4)) & 0x0f0f0f0f0f0f0f0full;
		return (Bits * 0x0101010101010101) >> 56;
	}
};