#pragma once

#pragma once

#include "CoreTypes.h"
#include "GenericPlatform/GenericPlatformAtomics.h"

#if PLATFORM_WINDOWS
#include "Windows/WindowsPlatformAtomics.h"
#else
#error Unknown platform
#endif
