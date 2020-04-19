#pragma once

#include "CoreTypes.h"

#if PLATFORM_WINDOWS
#include "Windows/WindowsCriticalSection.h"
#else
#error Unknown platform
#endif
