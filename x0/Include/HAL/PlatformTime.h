#pragma once

#include "CoreTypes.h"

#if PLATFORM_WINDOWS
#include "Windows/WindowsPlatformTime.h"
#else
#error Unknown platform
#endif