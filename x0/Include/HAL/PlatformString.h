#pragma once

#include "CoreTypes.h"

#if PLATFORM_WINDOWS
#include "Windows/WindowsPlatformString.h"
#elif PLATFORM_LINUX
#endif

typedef XWindowsPlatformString XPlatformString;