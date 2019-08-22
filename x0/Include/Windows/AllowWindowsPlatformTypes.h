
#include "Windows/WindowsHWraper.h"

#ifndef WINDOWS_PLATFORM_TYPES_GUARD
	#define WINDOWS_PLATFORM_TYPES_GUARD
#else 
	#error Nesting AllowWindowsPlatformTypes.h is not allowed!
#endif

#define INT ::INT
#define UINT ::UINT
#define DWORD ::DWORD
#define FLOAT ::FLOAT

#define TRUE 1
#define FALSE 0

