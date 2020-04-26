#pragma once
/*
* Data Models--------------------------------------------------
	C / C++ base type	LP64(Unix / Linux)	LLP64(Windows)	ILP32
	int, short, char	¡Ü32					¡Ü32				¡Ü32
	Long				64					32				32
	void * (pointer)	64					64				32
	long long			64					64				64
	SP Float			32					32				32
	DP Float			64					64				64
*/

#include "Misc/Build.h"
#include "HAL/Platform.h"
//#include "ProfilingDebugging/UMemoryDefines.h"
#include "Misc/CoreMiscDefines.h"
//#include "Misc/CoreDefines.h"