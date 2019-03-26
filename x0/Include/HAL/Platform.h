#pragma once

#if !defined(PLATFORM_WINDOWS)
	#define PLATFORM_WINDOWS 0
#endif
#if !defined(PLATFORM_LINUX)
	#define PLATFORM_LINUX 0
#endif

#include "GenericPlatform/GenericPlatform.h"

#if PLATFORM_WINDOWS
	#include "Windows/WindowsPlatform.h"
#elif PLATFORM_LINUX
	#include "Linux/LinuxPlatform.h"
#endif

typedef XPlatformTypes::uint8		uint8;
typedef XPlatformTypes::uint16		uint16;
typedef XPlatformTypes::uint32		uint32;
typedef XPlatformTypes::uint64		uint64;

typedef XPlatformTypes::int8		int8;
typedef XPlatformTypes::int16		int16;
typedef XPlatformTypes::int32		int32;
typedef XPlatformTypes::int64		int64;

typedef XPlatformTypes::ANSICHAR	ANSICHAR;
typedef XPlatformTypes::WIDECHAR	WIDECHAR;
typedef XPlatformTypes::TCHAR		TCHAR;

typedef XPlatformTypes::UPTRINT		UPTRINT;
typedef XPlatformTypes::PTRINT		PTRINT;
typedef XPlatformTypes::SIZE_T		SIZE_T;
typedef XPlatformTypes::SSIZE_T		SSIZE_T;



