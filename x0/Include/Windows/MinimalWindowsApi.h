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
#define WINDOWS_PF_COMPARE_EXCHANGE128 14

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
	extern "C" __declspec(dllimport) DWORD WINAPI SetCriticalSectionSpinCount(LPCRITICAL_SECTION lpCriticalSection, DWORD dwSpinCount);
	extern "C" __declspec(dllimport) BOOL WINAPI TryEnterCriticalSection(LPCRITICAL_SECTION lpCritalSection);
	extern "C" __declspec(dllimport) void WINAPI EnterCriticalSection(LPCRITICAL_SECTION lpCriticalSection);
	extern "C" __declspec(dllimport) void WINAPI LeaveCriticalSection(LPCRITICAL_SECTION lpCriticalSection);
	extern "C" __declspec(dllimport) void WINAPI DeleteCriticalSection(LPCRITICAL_SECTION lpCriticalSection);

	extern "C" __declspec(dllimport) void WINAPI InitializeSRWLock(PSRWLOCK SRWLock);
	extern "C" __declspec(dllimport) void WINAPI AcquireSRWLockShared(PSRWLOCK SRWLock);
	extern "C" __declspec(dllimport) void WINAPI ReleaseSRWLockShared(PSRWLOCK SRWLock);
	extern "C" __declspec(dllimport) void WINAPI AcquireSRWLockExclusive(PSRWLOCK SRWLock);
	extern "C" __declspec(dllimport) void WINAPI ReleaseSRWLockExclusive(PSRWLOCK SRWLock);

	// I/O
	extern "C" __declspec(dllimport) BOOL WINAPI ConnectNamedPipe(HANDLE hNamedPipe, LPOVERLAPPED lpOverlaped);
	extern "C" __declspec(dllimport) BOOL WINAPI GetOverlappedResult(HANDLE hFile, LPOVERLAPPED lpOverlaped, LPDWORD lpNumberOfBytesTransferred, BOOL bWait);
	extern "C" __declspec(dllimport) BOOL WINAPI WriteFile(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfByteWritten, LPOVERLAPPED lpOverlaped);
	extern "C" __declspec(dllimport) BOOL WINAPI ReadFile(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfByteRead, LPOVERLAPPED lpOverlaped);
	
	//Timing 
	extern "C" __declspec(dllimport) BOOL WINAPI QueryPerformanceCounter(LPLARGE_INTEGER Cycles);

	//Thread local storage 
	extern "C" __declspec(dllimport) DWORD WINAPI GetCurrentThreadId();
	extern "C" __declspec(dllimport) DWORD WINAPI TlsAlloc();
	extern "C" __declspec(dllimport) LPVOID WINAPI TlsGetValue(DWORD dwTlsIndex);
	extern "C" __declspec(dllimport) BOOL WINAPI TlsSetValue(DWORD dwTlsIndex, LPVOID lpTlsValue);
	extern "C" __declspec(dllimport) BOOL WINAPI TlsFree(DWORD dwTlsIndex);

	//System
	extern "C" __declspec(dllimport) BOOL WINAPI IsProcessorFeaturePresent(DWORD dwProcessorFeature);
	

	struct CRITICAL_SECTION { void* Opaque1[1]; long Opaque2[2]; void* Opaque3[3]; };
	struct OVERLAPPED { void *Opaque[3]; unsigned long Opaque2[2]; };
	union LPLARGE_INTEGER { struct { DWORD LowPart; LONG HightPart; }; LONGLONG QuadPart; };

	FORCEINLINE BOOL WINAPI ConnectNamedPipe(HANDLE hNamedPipe, OVERLAPPED* lpOverlaped)
	{
		return ConnectNamedPipe(hNamedPipe, (LPOVERLAPPED)lpOverlaped);
	}
	FORCEINLINE BOOL WINAPI GetOverlappedResult(HANDLE hFile, OVERLAPPED* lpOverlaped, LPDWORD lpNumberOfBytesTransferred, BOOL bWait)
	{
		return GetOverlappedResult(hFile, (LPOVERLAPPED)lpOverlaped, lpNumberOfBytesTransferred, bWait);
	}
	FORCEINLINE BOOL WINAPI WriteFile(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfByteWritten, OVERLAPPED* lpOverlaped)
	{
		return WriteFile(hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfByteWritten, (LPOVERLAPPED)lpOverlaped);
	}
	FORCEINLINE BOOL WINAPI ReadFile(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfByteRead, OVERLAPPED* lpOverlaped)
	{
		return ReadFile(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfByteRead, (LPOVERLAPPED)lpOverlaped);
	}

	FORCEINLINE void WINAPI InitializeCriticalSection(CRITICAL_SECTION* lpCriticalSection)
	{
		InitializeCriticalSection((LPCRITICAL_SECTION)lpCriticalSection);
	}

	FORCEINLINE BOOL WINAPI InitializeCriticalSectionAndSpinCount(CRITICAL_SECTION* lpCriticalSection, DWORD dwSpinCount)
	{
		return InitializeCriticalSectionAndSpinCount((LPCRITICAL_SECTION)lpCriticalSection, dwSpinCount);
	}

	FORCEINLINE WINAPI SetCriticalSectionSpinCount(CRITICAL_SECTION* lpCriticalSection, DWORD dwSpinCount)
	{
		SetCriticalSectionSpinCount((LPCRITICAL_SECTION)lpCriticalSection, dwSpinCount);
	}

	FORCEINLINE WINAPI TryEnterCriticalSection(CRITICAL_SECTION* lpCritalSection)
	{
		TryEnterCriticalSection((LPCRITICAL_SECTION)lpCritalSection);
	}

	FORCEINLINE void WINAPI EnterCriticalSection(CRITICAL_SECTION* lpCriticalSection)
	{
		EnterCriticalSection((LPCRITICAL_SECTION)lpCriticalSection);
	}

	FORCEINLINE void WINAPI LeaveCriticalSection(CRITICAL_SECTION* lpCriticalSection)
	{
		LeaveCriticalSection((LPCRITICAL_SECTION)lpCritalSection);
	}

	FORCEINLINE void WINAPI DeleteCriticalSection(CRITICAL_SECTION* lpCriticalSection)
	{
		DeleteCriticalSection((LPCRITICAL_SECTION)lpCriticalSection);
	}

	//Timing 
	FORCEINLINE BOOL WINAPI QueryPerformanceCounter(LPLARGE_INTEGER* Cycles)
	{
		return QueryPerformanceCounter((LPLARGE_INTEGER)Cycles);
	}
}