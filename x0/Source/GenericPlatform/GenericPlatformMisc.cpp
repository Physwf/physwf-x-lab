#include "GenericPlatform/GenericPlatformMisc.h"
#include "Misc/AssertionMacros.h"
//#include "HAL/PlatformFilemanager.h"
#include "Math/XMathUtility.h"
#include "HAL/UnrealMemory.h"
#include "Containers/Array.h"
#include "Containers/UnrealString.h"
#include "Logging/LogMacros.h"
#include "CoreGlobals.h"
//#include "Misc/Parse.h"
//#include "Misc/CommandLine.h"
//#include "Misc/Paths.h"
//#include "Internationalization/Text.h"
//#include "Internationalization/Internationalization.h"
//#include "Misc/Guid.h"
#include "Math/Color.h"
//#include "GenericPlatform/GenericPlatformCompression.h"
//#include "Misc/ConfigCacheIni.h"
//#include "Misc/App.h"
//#include "GenericPlatform/GenericPlatformChunkInstall.h"
//#include "HAL/FileManagerGeneric.h"
//#include "Misc/VarargsHelper.h"
//#include "Misc/SecureHash.h"
//#include "HAL/ExceptionHandling.h"
//#include "GenericPlatform/GenericPlatformCrashContext.h"
//#include "GenericPlatform/GenericPlatformDriver.h"
//#include "ProfilingDebugging/ExternalProfiler.h"
#//include "HAL/LowLevelMemTracker.h"
#include "Templates/Function.h"

//#include "Misc/UProjectInfo.h"
//#include "Internationalization/Culture.h"

#if UE_ENABLE_ICU
THIRD_PARTY_INCLUDES_START
#include <unicode/locid.h>
THIRD_PARTY_INCLUDES_END
#endif

#if !UE_BUILD_SHIPPING
bool FGenericPlatformMisc::bShouldPromptForRemoteDebugging = false;
bool FGenericPlatformMisc::bPromptForRemoteDebugOnEnsure = false;
#endif	