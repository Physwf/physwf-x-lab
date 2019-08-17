#pragma once

#include "X0.h"

struct X0_API XWindowsPlatformTypes : public XGenericPlatformTypes
{
#ifdef _WIN64
	typedef	unsigned __int64 SIZE_T;
	typedef __int64			SSIZE_T;
#else
	typedef unsigned long	SIZE_T;
	typedef long			SSIZE_T;
#endif
};

typedef XWindowsPlatformTypes XPlatformTypes;


#define PLATFORM_LITTLE_ENDIAN						1
#define PLATFORM_SUPPORTS_UNALIGNED_INT_LOAD		1


#define PLATFORM_SUPPORTS_PRAGMA_PACK				1


#define PLATFORM_HAS_BSD_TIME						1
#define PLATFORM_USE_PTHREAD						1

#define PLATFORM_HAS_BSD_SOCKET_FEATURE_WINSOCKS	1
#define PLATFORM_SUPPORTS_TBB						1


#define VARARGS		__cdecl							
#define CDECL		__cdecl
#define STDCALL		__stdcall
#define FORCEINLINE	__forceinline
#define FORCENOINLINE	__declspec(noinline)
#define FUNCTION_NO_RETURN_START	__declspec(noreturn)
#define FUNCTION_NO_NULL_RETURN_START	_Ret_notnull_


#if !defined(__clang__)
#define PRAGMA_DISABLE_OPTIMIZATION_ACTUAL	__pragma(optimize("",off))
#define PRAGMA_ENABLE_OPTIMIZATION_ACTUAL	__pragma(optimize("",on))
#elif defined(_MSC_VER)
#define PRAGMA_DISABLE_OPTIMIZATION_ACTUAL	__pragma(clang optimize off)
#define PRAGMA_ENABLE_OPTIMIZATION_ACTUAL	__pragma(clang optimize on)
#endif


#if defined(__clang__) || _MSC_VER >= 1900
#define CONSTEXPR constexpr
#else
#define CONSTEXPR
#endif

#define ABSTRACT abstract

#define LINE_TERMINATOR	TEXT("\r\n")
#define LINE_TERMINATOR_ANSI "\r\n"

#if defined(__clang__)
	#define GCC_PACK(n) __attribute__((packed,aligned(n)))
	#define GCC_ALIGN(n) __attribute__((aligned(n)))
	#if defined(_MSC_VER)
		#define MS_ALIGN(n) __declspec(align(n))
	#endif
#else
	#define MS_ALIGN(n) __declspec(align(n))
#endif

#define MSVC_PRAGMA(Pragma) __pragma(Pragma)

#define PLATFORM_CACHE_LINE_SIZE	128

#define DLLEXPORT __declspec(dllexport)
#define DLLIMPORT __declspec(dllimport)

#ifndef USE_SECURE_CRT
#define USE_SECURE_CRT 0
#endif