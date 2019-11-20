#pragma once

#include "CoreTypes.h"
#include "Misc/AssertionMacros.h"
#include "HAL/PlatformMath.h"

struct  FVector;

#undef  PI
#define PI 					(3.1415926535897932f)
#define SMALL_NUMBER		(1.e-8f)
#define KINDA_SMALL_NUMBER	(1.e-4f)
#define BIG_NUMBER			(3.4e+38f)
#define EULERS_NUMBER       (2.71828182845904523536f)

#define MAX_FLT 3.402823466e+38F

#define INV_PI			(0.31830988618f)
#define HALF_PI			(1.57079632679f)

#define DELTA			(0.00001f)

#define FLOAT_NORMAL_THRESH				(0.0001f)



struct XMath : public XPlatformMath
{
	static FORCEINLINE int32 RandHelper(int32 A)
	{
		return A > 0 ? Min(TruncToInt(FRand() * A), A - 1) : 0;
	}

	static FORCEINLINE int32 RandRange(int32 Min, int32 Max)
	{
		const int32 Range = (Max - Min) + 1;
		return Min + RandHelper(Range);
	}

	static FORCEINLINE float RandRange(float InMin, float InMax)
	{
		return FRandRange(InMin, InMax);
	}

	static FORCEINLINE float FRandRange(float InMin, float InMax)
	{
		return InMin + (InMax - InMin) * FRand();
	}

	static FORCEINLINE bool RandBool()
	{
		return RandRange(0, 1) == 1 ? true : false;
	}

	static FVector VRand();
};