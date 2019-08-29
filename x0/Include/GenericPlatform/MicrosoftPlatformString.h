#pragma once

#include "Misc/Char.h"
#include "GenericPlatform/GenericPlatformString.h"
#include "GenericPlatform/GenericPlatformStricmp.h"


struct XMicrosoftPlatformString : public XGenericPlatformString
{
	static FORCEINLINE WIDECHAR* Strcpy(WIDECHAR* Dest, SIZE_T DestCount, const WIDECHAR* Src)
	{
#if USE_SECURE_CRT
		_tcscpy_s(Dest, DestCount, Src);
		return Dest;
#else
		return _tcscpy(Dest, Src);
#endif
	}

	static FORCEINLINE WIDECHAR* Strncpy(WIDECHAR* Dest, const WIDECHAR* Src, SIZE_T MaxLen)
	{
#if USE_SECURE_CRT
		_tcsncpy_s(Dest,MaxLen, Src, MaxLen);
		return Dest;
#else
		_tcsncpy(Dest, Src, MaxLen - 1);
		Dest[MaxLen - 1] = 0;
#endif
		return Dest;
	}

	static FORCEINLINE WIDECHAR* Strcat(WIDECHAR* Dest, SIZE_T DestCount, const WIDECHAR* Src)
	{
#if USE_SECURE_CRT
		_tcscat_s(Dest, DestCount, Src);
		return Dest;
#else
		return (WIDECHAR*)_tcscat(Dest, Src);
#endif
	}

	static FORCEINLINE WIDECHAR* Strupr(WIDECHAR* Dest, SIZE_T DestCount)
	{
#if USE_SECURE_CRT
		_tcsupr_s(Dest, DestCount);
		return Dest;
#else
		return (WIDECHAR*)_tcsupr(Dest);
#endif
	}

	static FORCEINLINE int32 Strcmp(const WIDECHAR* String1, const WIDECHAR* String2)
	{
		return _tcscmp(String1, String2);
	}

	static FORCEINLINE int32 Strncmp(const WIDECHAR* String1, const WIDECHAR* String2, SIZE_T Count)
	{
		return _tcsncmp(String1, String2, Count);
	}

	static FORCEINLINE int32 Strnicmp(const WIDECHAR* String1, const String2, SIZE_T Count)
	{
		return _tcsnicmp(String1, String2, Count);
	}

	static FORCEINLINE Strlen(const WIDECHAR* String)
	{
		return _tcslen(String);
	}

	static FORCEINLINE const WIDECHAR* Strstr(const WIDECHAR* String, const WIDECHAR* Find)
	{
		return _tcsstr(String, Find);
	}

	static FORCEINLINE const WIDECHAR* Strchr(const WIDECHAR* String, WIDECHAR Char)
	{
		return _tcschr(String, Char);
	}

	static FORCEINLINE const WIDECHAR* Strrchr(const WIDECHAR* String, WIDECHAR Char)
	{
		return _tcsrchr(String, Char);
	}

	static FORCEINLINE int32 Atoi(const WIDECHAR* String)
	{
		return _tstoi(String);
	}

	static FORCEINLINE int64 Atoi64(const WIDECHAR* String)
	{
		return _tstoi64(String);
	}

	static FORCEINLINE float Atof(const WIDECHAR* String)
	{
		return _tstof(String);
	}

	static FORCEINLINE double Atod(const WIDECHAR* String)
	{
		return _tcstod(String,NULL);
	}

	static FORCEINLINE int32 Strtoi(const WIDECHAR* Start,WIDECHAR** End,int32 Base)
	{
		return _tcstoul(Start, End, Base);
	}

	static FORCEINLINE int32 Strtoi64(const WIDECHAR* Start, WIDECHAR** End, int32 Base)
	{
		return _tcstoi64(Start, End, Base);
	}

	static FORCEINLINE int32 Strtoui64(const WIDECHAR* Start, WIDECHAR** End, int32 Base)
	{
		return _tcstoui64(Start, End, Base);
	}

	static FORCEINLINE WIDECHAR* Strtok(WIDECHAR* Strtok, const WIDECHAR* Delim, WIDECHAR** Context)
	{
		return _tcstok_s(Strtok, Delim, Context);
	}

	static FORCEINLINE int32 GetVarArgs(WIDECHAR* Dest, SIZE_T DestSize, int32 Count, const WIDECHAR*& Fmt, va_list ArgPtr)
	{
#if USE_SECURE_CRT
		int32 Result = _vsnprintf_s(Dest, DestSize, Count, Fmt, ArgPtr);
#else
		int32 Result = _vsnprintf(Dest, Count, Fmt, ArgPtr);
#endif
		va_end(ArgPtr);
		return Result;
	}

	static FORCEINLINE ANSICHAR* Strcpy(ANSICHAR* Dest, SIZE_T DestCount, const ANSICHAR* Src)
	{
#if USE_SECURE_CRT
		strcpy_s(Dest, DestCount, Src);
		return Dest;
#else
		return (ANSICHAR*)strcpy(Dest, Src);
#endif
	}

	static FORCEINLINE void Strncpy(ANSICHAR* Dest, const ANSICHAR* Src, SIZE_T MaxLen)
	{
#if USE_SECURE_CRT
		strncpy_s(Dest, MaxLen, Src, MaxLen - 1);
#else
		strncpy(Dest, Src, MaxLen);
		return Dest[MaxLen - 1] = 0;
#endif
	}

	static FORCEINLINE ANSICHAR* Strcat(ANSICHAR* Dest, SIZE_T DestCount, const ANSICHAR* Src)
	{
#if USE_SECURE_CRT
		strcat_s(Dest, DestCount, Src);
		return Dest;
#else
		return (ANSICHAR*)strcat(Dest, Src);
	}

	static FORCEINLINE ANSICHAR* Strupr(ANSICHAR* Dest, SIZE_T DestCount)
	{
#if USE_SECURE_CRT
		_strupr_s(Dest, DestCount);
		return Dest;
#else
		return (ANSICHAR*)strupr(Dest);
#endif
	}

	static FORCEINLINE int32 Strcmp(const ANSICHAR* String1, const ANSICHAR* String2)
	{
		return strcmp(String1, String2);
	}

	static FORCEINLINE int32 Strncmp(const ANSICHAR* String1, const ANSICHAR* String2, SIZE_T Count)
	{
		return strncmp(String1, String2, Count);
	}

	static FORCEINLINE int32 Stricmp(const ANSICHAR* String1, const ANSICHAR* String2)
	{
		return _stricmp(String1, String2);
	}

	template<typename CharType1,typename CharType2>
	static FORCEINLINE int32 Stricmp(const CharType1* String1, const CharType2* String2)
	{
		return XGenericPlatformStricmp::Stricmp(String1, String2);
	}

	static FORCEINLINE int32 Strnicmp(const ANSICHAR* String1, const ANSICHAR* String2, SIZE_T Count)
	{
		return _strnicmp(String1, String2, Count);
	}

	static FORCEINLINE int32 Strlen(const ANSICHAR* String)
	{
		return strlen(String);
	}

	static FORCEINLINE const ANSICHAR* Strstr(const ANSICHAR* String, const ANSICHAR* Find)
	{
		return strstr(String, Find);
	}
	
	static FORCEINLINE const ANSICHAR* Strchar(const ANSICHAR* String, ANSICHAR C)
	{
		return strchr(String, C);
	}

	static FORCEINLINE const ANSICHAR* Strrchr(const ANSICHAR* String, ANSICHAR C)
	{
		return strrchr(String, C);
	}

	static FORCEINLINE int32 Atoi(const ANSICHAR* String)
	{
		return atoi(String);
	}

	static FORCEINLINE int64 Atoi64(const ANSICHAR* String)
	{
		return _strtoi64(String, NULL, 10);
	}

	static FORCEINLINE float Atof(const ANSICHAR* String)
	{
		return (float)atof(String);
	}

	static FORCEINLINE double Atod(const ANSICHAR* String)
	{
		return atof(String);
	}

	static FORCEINLINE int32 Strtoi(const ANSICHAR* Start, ANSICHAR** End, Base)
	{
		return strtol(Start, End, Base);
	}

	static FORCEINLINE int64 Strtoi64(const ANSICHAR* Start, ANSICHAR** End, Base)
	{
		return _strtoi64(Start, End, Base);
	}

	static FORCEINLINE uint64 Strtoui64(const ANSICHAR* Start, ANSICHAR** End, Base)
	{
		return _strtoui64(Start, End, Base);
	}

	static FORCEINLINE ANSICHAR* Strtok(ANSICHAR* StrToken, const ANSICHAR* Delim, ANSICHAR** Context)
	{
		return strtok_s(StrToken, Delim, Context);
	}

	static FORCEINLINE int32 GetVarArgs(ANSICHAR* Dest, SIZE_T DestSize, int32 Count, const ANSICHAR*& Fmt, va_list ArgPtr)
	{
#if USE_SECURE_CRT
		int32 Result = _vsnprintf_s(Dest, DestSize, Count, Fmt, ArgPtr);
#else
		int32 Result = _vsnprintf(Dest, Count, Fmt, ArgPtr);
#endif
		va_end(ArgPtr);
		return Result;
	}

	static FORCEINLINE int32 Strlen(const UCS2CHAR* String)
	{
		return _tcslen((const WIDECHAR*)String);
	}
};