#pragma once

#include "CoreTypes.h"
//#include "Misc/Exec.h"
#include "Containers/Array.h"
#include "Containers/UnrealString.h"
#include "Containers/Map.h"
#include "Templates/Function.h"
#include "Math/IntPoint.h"
//#include "UObject/NameTypes.h"
#include "CoreGlobals.h"
#include "HAL/ThreadSingleton.h"

#include "X0.h"
/**
 * Check to see if this executable is running as dedicated server
 * Editor can run as dedicated with -server
 */
FORCEINLINE bool IsRunningDedicatedServer()
{
// 	if (FPlatformProperties::IsServerOnly())
// 	{
// 		return true;
// 	}
// 
// 	if (FPlatformProperties::IsGameOnly())
// 	{
// 		return false;
// 	}
// 
// #if UE_EDITOR
// 	extern CORE_API int32 StaticDedicatedServerCheck();
// 	return (StaticDedicatedServerCheck() == 1);
// #else
// 	return false;
// #endif
	return false;
}

bool X0_API StringHasBadDashes(const TCHAR* Str);