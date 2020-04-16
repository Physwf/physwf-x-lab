#pragma once

#include "GenericPlatform/GenericPlatform.h"


struct FWindowsPlatformTypes : public FGenericPlatformTypes
{
#ifdef _WIN64
	typedef unsigned __int64	SIZE_T;
	typedef __int64				SSIZE_T;
#else
	typedef unsigned long		SIZE_T;
	typedef long				SSIZE_T;
#endif
};

typedef FWindowsPlatformTypes FPlatformTypes;

#define PLATFORM_LITTLE_ENDIAN								1

#define PLATFORM_ENABLE_VECTORINTRINSICS					1