#pragma once

#include "CoreTypes.h"

#ifndef STRICT
#define STRICT
#endif

struct HINSTANCE__;
struct HWND__;
struct HKEY__;
struct HDC__;
struct HICON__;

struct tagPROCESSENTRY32W;
struct _EXCEPTION_POINTERS;
struct _OVERLAPPED;
struct _RTL_CRITICAL_SECTION;
union _LARGE_INTEGER;

#define WINDOWS_MAX_PATH 260
#define WIDDOWS_PF_COMPARE_EXHANGE 14

#define WINAPI __stdcall

namespace Windows
{
	typedef int32 BOOL;
	typedef unsigned long DWORD;
	typedef DWORD* LPDWORD;
	typedef long LONG;
	typedef long* LPLONG;
	typedef int64 LONGLONG;
	typedef void* LPVOID;
	typedef const void* LPCVOID;
	typedef const wchar_t* LPCTSTR;

	typedef void* HANDLE;
	typedef HINSTANCE__* HINSTANCE;
	typedef HINSTANCE HMODULE;
	typedef HWND__** HWND;
	typedef HKEY__* HKEY;
	typedef HDC__* HDC;
	typedef HICON__* HICON;
	typedef HICON__* HCURSOR;

	typedef tagPROCESSENTRY32W PROCESSENTRY32;
	typedef _EXCEPTION_POINTERS* LPEXCEPTION_POINTERS;
	typedef _RTL_CRITICAL_SECTION* LPCRITICAL_SECTION;
	typedef _OVERLAPPED* LPOVERLAPPED;
	typedef _LARGE_INTEGER* LPLARGE_INTEGER;

	typedef struct _RTL_SRWLOCK
	{
		void* Ptr;
	} RTL_SRWLOCK, *PRTL_SRWLOCK;

	typedef RTL_SRWLOCK SRWLOCK, *PSRWLOCK;

	//Modules
	extern "C" __declspec(dllimport) HMODULE WINAPI LoadLibraryW(LPCTSTR lpFileName);
	extern "C" __declspec(dllimport) BOOL WINAPI FreeLibrary(HMODULE hModule);

	// Critical sections
	extern "C" __declspec(dllimport) void WINAPI InitializeCriticalSection(LPCRITICAL_SECTION lpCriticalSection);
	extern "C" __declspec(dllimport) BOOL WINAPI InitializeCriticalSectionAndSpinCount(LPCRITICAL_SECTION lpCriticalSection, DWORD dwSpinCount);
}