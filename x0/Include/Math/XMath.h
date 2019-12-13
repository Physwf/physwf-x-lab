#pragma once

#include "HAL/PlatformMath.h"

struct XVector;

#undef  PI
#define PI 					(3.1415926535897932f)
#define SMALL_NUMBER		(1.e-8f)
#define KINDA_SMALL_NUMBER	(1.e-4f)
#define BIG_NUMBER			(3.4e+38f)
#define EULERS_NUMBER       (2.71828182845904523536f)

// Copied from float.h
#define MAX_FLT 3.402823466e+38F

// Aux constants.
#define INV_PI			(0.31830988618f)
#define HALF_PI			(1.57079632679f)

// Magic numbers for numerical precision.
#define DELTA			(0.00001f)

struct XMath : public XPlatformMath
{
	static inline int32 RandHelper(int32 A)
	{
		return A > 0 ? Min(TruncToInt(FRand() * A), A - 1) : 0;
	}

	static inline int32 RandRange(int32 Min, int32 Max)
	{
		const int32 Range = (Max - Min) + 1;
		return Min + RandHelper(Range);
	}

	static inline float RandRange(float InMin, float InMax)
	{
		return FRandRange(InMin, InMax);
	}

	static inline float FRandRange(float InMin, float InMax)
	{
		return InMin + (InMax - InMin) * FRand();
	}

	static inline bool RandBool()
	{
		return (RandRange(0, 1) == 1) ? true : false;
	}

	template <typename T>
	static inline bool IsPowerOfTwo(T Value)
	{
		return ((Value & (Value - 1)) == (T)0);
	}


	// Math Operations

	/** Returns highest of 3 values */
	template< class T >
	static inline T Max3(const T A, const T B, const T C)
	{
		return Max(Max(A, B), C);
	}

	/** Returns lowest of 3 values */
	template< class T >
	static inline T Min3(const T A, const T B, const T C)
	{
		return Min(Min(A, B), C);
	}

	/** Multiples value by itself */
	template< class T >
	static inline T Square(const T A)
	{
		return A * A;
	}

	/** Clamps X to be between Min and Max, inclusive */
	template< class T >
	static inline T Clamp(const T X, const T Min, const T Max)
	{
		return X < Min ? Min : X < Max ? X : Max;
	}

	static inline float Log2(float Value)
	{
		// Cached value for fast conversions
		static const float LogToLog2 = 1.f / Loge(2.f);
		// Do the platform specific log and convert using the cached value
		return Loge(Value) * LogToLog2;
	}

	template<class T>
	static inline auto RadiansToDegrees(T const& RadVal) -> decltype(RadVal * (180.f / PI))
	{
		return RadVal * (180.f / PI);
	}

	/**
	 * Converts degrees to radians.
	 * @param	DegVal			Value in degrees.
	 * @return					Value in radians.
	 */
	template<class T>
	static inline auto DegreesToRadians(T const& DegVal) -> decltype(DegVal * (PI / 180.f))
	{
		return DegVal * (PI / 180.f);
	}

	template< class T, class U >
	static inline T Lerp(const T& A, const T& B, const U& Alpha)
	{
		return (T)(A + Alpha * (B - A));
	}
};
