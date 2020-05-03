#pragma once

#include "CoreTypes.h"
#include "HAL/PlatformMisc.h"
#include "Templates/AndOrNot.h"
#include "Templates/EnableIf.h"
#include "Templates/IsArrayOrRefOfType.h"
#include "Templates/IsValidVariadicFunctionArg.h"
#include "Misc/VarArgs.h"

#include <cassert>

#define check(expr)					{ assert(expr); }
#define checkf(expr,format,...)		{ assert(expr); }
#define checkSlow(expr)				{ assert(expr); }
#define ensure(expr)				{ assert(expr); }
#define verify(expr)				{ assert(expr); }
#define ensureMsgf(expr,format,...) { assert(expr); }