#pragma once

struct XMath
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
		// see http://codinggorilla.domemtech.com/?p=81 or http://en.wikipedia.org/wiki/Binary_logarithm but modified to return 0 for a input value of 0
		// 686ms on test data
		uint32 pos = 0;
		if (Value >= 1 << 16) { Value >>= 16; pos += 16; }
		if (Value >= 1 << 8) { Value >>= 8; pos += 8; }
		if (Value >= 1 << 4) { Value >>= 4; pos += 4; }
		if (Value >= 1 << 2) { Value >>= 2; pos += 2; }
		if (Value >= 1 << 1) { pos += 1; }
		return (Value == 0) ? 0 : pos;

	}
};