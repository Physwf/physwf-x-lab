#pragma once

#include "CoreTypes.h"
#include "GenericPlatform/GenericPlatformMemory.h"

#if PLATFORM_WINDOWS
#include "Windows/WindowsPlatformMemory.h"
#else
#error Unknown Platform
#endif