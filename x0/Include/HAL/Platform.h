#pragma once

#if !defined(PLATFORM_WINDOWS)
#define PLATFORM_WINDOWS 0
#endif

#if PLATFORM_WINDOWS
#include "Windows/WIndowsPlatform.h"
#else
#error Unknown platform
#endif