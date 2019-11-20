#include "GenericPlatform/GenericPlatformMath.h"

static int32 GSRandSeed;

X0_API void XGenericPlatformMath::SRandInit(int32 Seed)
{
	GSRandSeed = Seed;
}

X0_API int32 XGenericPlatformMath::GetRandSeed()
{
	return GSRandSeed;
}

X0_API float XGenericPlatformMath::SRand()
{
	GSRandSeed = (GSRandSeed * 196314165) + 907633515;
	union { float f; int32 i; } Result;
	union { float f; int32 i; } Temp;
	const float SRandTemp = 1.0f;
	Temp.f = SRandTemp;
	Result.i = (Temp.i & 0xff800000) | (GSRandSeed & 0x007fffff);
	return XPlatformMath::Fractional(Result.f);
}
