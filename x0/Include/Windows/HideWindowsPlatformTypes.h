
#ifndef WINDOWS_PLATFORM_TYPES_GUARD
#define WINDOWS_PLATFORM_TYPES_GUARD
#else 
#error Nesting AllowWindowsPlatformTypes.h is not allowed!
#endif

#undef INT
#undef UINT
#undef DWORD
#undef FLOAT

#ifdef TRUE
	#undef TRUE
#endif

#ifdef FALSE
	#undef FALSE
#endif

#pragma warning( pop )