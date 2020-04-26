#include "CoreTypes.h"
#include "GenericPlatform/GenericPlatformProperties.h"

#if PLATFORM_WINDOWS
#include "Windows/WindowsPlatformProperties.h"
#define WITH_SERVER_CODE 0
typedef FWindowsPlatformProperties<WITH_EDITORONLY_DATA, UE_SERVER, !WITH_SERVER_CODE> FPlatformProperties;
#endif