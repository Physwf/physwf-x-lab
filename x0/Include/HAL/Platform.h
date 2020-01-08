#pragma once

#if !defined(PLATFORM_WINDOWS)
#define PLATFORM_WINDOWS 0
#endif

#if PLATFORM_WINDOWS
#include "Windows/WIndowsPlatform.h"
#else
#error Unknown platform
#endif


#ifndef PLATFORM_ENABLE_VECTORINTRINSICS
#define PLATFORM_ENABLE_VECTORINTRINSICS	0
#endif