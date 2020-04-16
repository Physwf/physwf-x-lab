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

#define PLATFORM_DESKTOP					1
#if defined( _WIN64 )
#define PLATFORM_64BITS					1
#else
#define PLATFORM_64BITS					0
#endif

#define PLATFORM_LITTLE_ENDIAN								1

#define PLATFORM_ENABLE_VECTORINTRINSICS					1