#include "CoreTypes.h"
#include "GenericPlatform/GenericPlatformProperties.h"

#if PLATFORM_WINDOWS
#include "Windows/WindowsPlatformProperties.h"
#define UE_SERVER 0
#define WITH_SERVER_CODE 0
typedef FWindowsPlatformProperties<false, UE_SERVER, !WITH_SERVER_CODE> FPlatformProperties;
#endif