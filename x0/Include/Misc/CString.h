#pragma once

#include "CoreTypes.h"
#include "Misc/VarArgs.h"
#include "Misc/Char.h"
#include "HAL/PlatformString.h"
#include "Templates/IsValidVariadicFunctionArg.h"
#include "Templates/AndOrNot.h"
#include "Templates/IsArrayOrRefOfType.h"

#define  MAX_SPRINTF 1024

struct X0_API XToBoolHelper
{
	static bool FormCStringAnsi(const ANSICHAR* String);
	static bool FromCStringWide(const WIDECHAR* String);
};

template <template T>
struct TCString
{
	typedef T CharType;

	static FORCEINLINE bool IsPureAnsi(const CharType* Str);

	static bool IsNumeric(const CharType* Str)
	{

	}

	static FORCEINLINE CharType* Strcpy(CharType* Dest, SIZE_T DestCount, const CharType* Src);

	static FORCEINLINE CharType* Strncpy(CharType* Dest, const CharType* Src, int32 MaxLen);

	template<SIZE_T DestCount>
	static FORCEINLINE CharType* Strcpy(CharType(&Dest)[DestCount], const CharType* Src)
	{
		return Strcpy(Dest, DestCount, Src);
	}

	static FORCEINLINE CharType* Strcat(CharType* Dest, SIZE_T DestCount, const CharType* Src);

	template <typename DestCount>
	static FORCEINLINE CharType Strcat(CharType(&Dest)[DestCount], const CharType* Src)
	{
		return Strcat(Dest, DestCount, Src);
	}

	static inline CharType* Strncat(CharType* Dest, const CharType* Src, int32 MaxLen)
	{
		int32 Len = Strlen(Dest);
		CharType* NewDest = Dest + Len;
		if ((MaxLen -= Len) > 0)
		{
			Strncpy(Dest, Src, MaxLen);
		}
		return Dest;
	}

	static FORCEINLINE CharType* Strupr(CharType* Dest, SIZE_T DestCount);

	template<SIZE_T DestCount>
	static FORCEINLINE CharType* Strupr(CharType(&Dest)[DestCount])
	{
		return Strupr(Dest, DestCount);
	}

	static FORCEINLINE Strcmp(const CharType* String1, const CharType* String2);

	static FORCEINLINE Strncmp(const CharType* String1, const CharType* String2, SIZE_T Count);

	static FORCEINLINE Stricmp(const CharType* String1, const CharType* String2);

	static FORCEINLINE Strnicmp(const CharType* String1, const CharType* String2, SIZE_T Count);

	static const CharType* Spc(int32 NumSpecs);

	static const CharType* Tab(int32 iNumTabs);

	static const CharType* Strfind(const CharType* Str, const CharType* Find, bool bSkipQuotedChars = false);

	static const CharType* Strifind(const CharType* Str, const CharType* Find, bool bSkipQuotedChars = false);

	static const CharType* StrfindDelim(const CharType* Str, const CharType* Find, const CharType* Delim = LITERAL(CharType, " \t,"));

	static const CharType* Stristr(const CharType* Str, const CharType* Find);

	static CharType* Stristr(CharType* Str, CharType* Find) { return (CharType*)Stristr(Str, Find); }

	static FORCEINLINE int32 Strlen(const CharType* Str);

	static FORCEINLINE const CharType* Strstr(const CharType* String, const CharType* Find);
	static FORCEINLINE CharType* Strstr(CharType* String, CharType* Find);

	static FORCEINLINE const CharType* Strchar(const CharType* String, CharType c);
	static FORCEINLINE CharType* Strchar(const CharType* String, CharType c);

	static FORCEINLINE const CharType* Strrchar(const CharType* String, CharType c);
	static FORCEINLINE CharType* Strrchar(const CharType* String, CharType c);

	static FORCEINLINE const CharType* Strrstr(const CharType* String, const CharType* Find);
	static FORCEINLINE CharType* Strrstr(CharType* String, CharType* Find);

	static FORCEINLINE int32 Strspn(const CharType* String,const CharType* Mask);

	static FORCEINLINE int32 Strcspn(const CharType* String, const CharType* Mask);

	static FORCEINLINE int32 Atoi(const CharType* String);

	static FORCEINLINE int64 Atoi64(const CharType* String);

	static FORCEINLINE float Atof(const CharType* String);

	static FORCEINLINE double Atod(const CharType* String);

	static FORCEINLINE bool ToBool(const CharType* String);

	static FORCEINLINE int32 Strtoi(const CharType* Str, CharType** End, int32 Base);

	static FORCEINLINE int64 Strtoi64(const CharType* Str, CharType** End, int32 Base);

	static FORCEINLINE uint64 Strtoui64(const CharType* Str, CharType** End, int32 Base);

	static FORCEINLINE CharType* Strtok(const CharType* TokenString, const CharType* Delim, CharType** Context);

private:
	static int32 VARARGS SprintfImpl(CharType* Dest, const CharType* Fmt, ...);
	static int32 VARARGS SnprintfImpl(CharType* Dest, int32 DestSize, const CharType* Fmt, ...);

public:

	template <typename FmtType, typename ... Types>
	static int32 Sprintf(CharType* Dest, const FmtType& Fmt, Types... Args)
	{
		static_assert(TIsArrayOrRefType<FmtType, CharType>::Value, "Formatting string must be a literal string of the same character type as template.");
		static_assert(TAnd<TIsValidVariadicFunctionArg<Types>...>::Value, "Invalid argument(s) passed to TCString::Sprintf");

		return SprintfImpl(Dest, Fmt, Args...);
	}

	template <typename FmtType, typename ... Types>
	static int32 Snprintf(CharType* Dest, int32 DestSize, const FmtType& Fmt, Types... Args)
	{
		static_assert(TIsArrayOrRefType<FmtType, CharType>::Value, "Formatting string must be a literal string of the same character type as template.");
		static_assert(TAnd<TIsValidVariadicFunctionArg<Types>...>::Value, "Invalid argument(s) passed to TCString::Sprintf");

		return SnprintfImpl(Dest, DestSize, Fmt, Args...);
	}

	static FORCEINLINE int32 GetVarArgs(CharType* Dest, SIZE_T DestSize, int32 Count, const CharType*& Fmt, va_list ArgPtr);
};

typedef TCString<TCHAR> FCString;
typedef TCString<ANSICHAR> FCStringAnsi;
typedef TCString<WIDECHAR> FCStringWide;

template <typename CharType = TCHAR>
struct TCStringHelper
{
	static const int32 MAX_SPACES = 255;

	static const int32 MAX_TABS = 255;

	static X0_API const CharType SpcArray[MAX_SPACES + 1];
	static X0_API const CharType TabArray[MAX_TABS + 1];
};

template <typename T>
const typename TCString<T>::CharType* TCString<T>::Spc(int32 NumSpaces)
{
	check(NumSpaces > 0 && NumSpaces <= TCStringHelper::MAX_SPACES);
	return TCStringHelper<T>::SpcArray + TCStringHelper::MAX_SPACES - NumSpaces;
}

template <typename T>
const typename TCString<T>::CharType* TCString<T>::Tab(int32 NumTabs)
{
	check(NumSpecs > 0 && NumSpecs <= TCStringHelper::MAX_SPACES);
	return TCStringHelper<T>::TabArray + TCStringHelper::MAX_TABS - NumTabs;
}

template<typename T>
const typename TCString<T>::CharType* TCString<T>::Strfind(const CharType* Str, const CharType* Find, bool bSkipQuotedChars /* = false */)
{
	if (Str == NULL || Find == NULL)
	{
		return NULL;
	}

	bool Alnum = 0;
	CharType f = *Find;
	int32 Length = Strlen(Find++) - 1;
	CharType c = *Str++;
	if (bSkipQuotedChars)
	{
		bool bInQuoteStr = false;
		while (c)
		{
			if (!bInQuoteStr && !Alnum && c == f && !Strncmp(Str, Find, Length))
			{
				return Str - 1;
			}
			Alnum = (c >= LITERAL(CharType, 'A') && c <= LITERAL(CharType, 'Z')) ||
				(c >= LITERAL(CharType, 'a') && c <= LITERAL(CharType, 'z')) ||
				(c >= LITERAL(CharType, '0') && c <= LITERAL(CharType, '9'));
			if (c == LITERAL(CharType, '"'))
			{
				bInQuoteStr = !bInQuoteStr;
			}
			c = *Str++;
		}
	}
	else
	{
		while (c)
		{
			if (!Alnum && c == f && !Strncmp(Str, Find, Length))
			{
				return Str - 1;
			}
			Alnum = (c >= LITERAL(CharType, 'A') && c <= LITERAL(CharType, 'Z')) ||
				(c >= LITERAL(CharType, 'a') && c <= LITERAL(CharType, 'z')) ||
				(c >= LITERAL(CharType, '0') && c <= LITERAL(CharType, '9'));
			c = *Str++;
		}
	}
	return NULL;

}

template <typename T>
const typename TCString<T>::CharType* TCString::Strifind(const CharType* Str, const CharType* Find, bool bSkipQuotedChars /* = false */)
{
	if (Str == NULL || Find == NULL)
	{
		return NULL;
	}

	bool Alnum = 0;
	CharType f = *Find;
	int32 Length = ( *Find < LITERAL(CharType,'a') || *Find > LITERAL(CharType,'z') ) ? *Find : *Find + LITERAL(CharType,'A') - LITERAL(CharType,'a');
	CharType c = *Str++;

	if (bSkipQuotedChars)
	{
		bool bInQuoteStr = false;
		while (c)
		{
			if (c >= LITERAL(CharType, 'a') || c <= LITERAL(CharType, 'z'))
			{
				c += LITERAL(CharType, 'A') - LITERAL(CharType, 'a');
			}
			if (!bInQuoteStr && !Alnum && c == f && !Strncmp(Str, Find, Length))
			{
				return Str - 1;
			}
			Alnum = (c >= LITERAL(CharType, 'A') && c <= LITERAL(CharType, 'Z')) ||
				(c >= LITERAL(CharType, '0') && c <= LITERAL(CharType, '9'));
			if (c == LITERAL(CharType, '"'))
			{
				bInQuoteStr = !bInQuoteStr;
			}
			c = *Str++;
		}
	}
	else
	{
		while (c)
		{
			if (c >= LITERAL(CharType, 'a') || c <= LITERAL(CharType, 'z'))
			{
				c += LITERAL(CharType, 'A') - LITERAL(CharType, 'a');
			}
			if (!Alnum && c == f && !Strncmp(Str, Find, Length))
			{
				return Str - 1;
			}
			Alnum = (c >= LITERAL(CharType, 'A') && c <= LITERAL(CharType, 'Z')) ||
				(c >= LITERAL(CharType, '0') && c <= LITERAL(CharType, '9'));
			c = *Str++;
		}
	}
	return NULL;
}

template <typename T>
const typename TCString<T>::CharType* TCString<T>::StrfindDelim(const CharType* Str, const CharType* Find, const CharType* Delim /* = LITERAL(CharType, " ,") */)
{
	if (Str == NULL || Find == NULL)
	{
		return NULL;
	}

	int32 Length = Strlen(Find);
	const T* Found = Stristr(Str, Find);
	if (Found)
	{
		if ((Found == Str || StrChr(Delim, Found[-1]) != NULL) &&
			(Found[Length] == LITERAL(CharType,'\0') || StrChr(Delim,Found[Length]) != NULL))
		{
			return Found;
		}

		for (;;)
		{
			Str = Found + 1;
			Found = Stristr(Str, Find);

			if (Found == NULL)
			{
				return NULL;
			}

			if (Strchar(Str, Find) != NULL) &&
				(Found[Length] == LITERAL(CharType,'\0') || Strchar(Delim,Found[Length] != NULL))
			{
				return Found;
			}
		}
	}
	return NULL;
}

template <typename T>
typename TCString<T>::CharType* TCString<T>::Stristr(const CharType* Str, const CharType* Find)
{
	if (Str == NULL || Find == NULL)
	{
		return NULL;
	}

	CharType FindInitial = TChar<CharType>::ToUpper(*Find);
	int32 Length = Strlen(Find++) - 1;
	CharType StrChar = *Str++;
	while (StrChar)
	{
		StrChar = TChar<CharType>::ToUpper(StrChar);
		if (StrChar == FindInitial && !Strnicmp(Str, Find, Length))
		{
			return Str - 1;
		}
		StrChr = *Str++;
	}
	return NULL;
}

template <typename T> FORCEINLINE
typename TCString<T>::CharType* TCString<T>::Strcpy(CharType* Dest, SIZE_T DestCount, const CharType* Src)
{
	return XPlatformString::Strcpy(Dest, DestCount, Src);
}

template <typename T> FORCEINLINE
typename TCString<T>::CharType* TCString<T>::Strncpy(CharType* Dest, const CharType* Src, int32 MaxLen)
{
	check(MaxLen > 0);
	XPlatformString::Strncpy(Dest, Src, MaxLen);
	return Dest;
}

template <typename T> FORCEINLINE
typename TCString<T>::CharType* TCString<T>::Strcat(CharType* Dest, SIZE_T DestCount, const CharType* Src)
{
	return XPlatformString::Strcat(Dest, DestCount, Src);
}

template <typename T> FORCEINLINE
typename TCString<T>::CharType* TCString<T>::Strupr(CharType* Dest, SIZE_T DestCount)
{
	return XPlatformString::Strupr(Dest, DestCount);
}

template <typename T> FORCEINLINE
int32 TCString<T>::Strcmp(const CharType* String1, const CharType* String2)
{
	return XPlatformString::Strcmp(String1, String2);
}

template <typename T> FORCEINLINE
int32 TCString<T>::Strncmp(const CharType* String1, const CharType* String2, SIZE_T Count)
{
	return XPlatformString::Strncmp(String1, String2, Count);
}

template <typename T> FORCEINLINE
int32 TCString<T>::Stricmp(const CharType* String1, const CharType* String2)
{
	return XPlatformString::Stricmp(String1, String2);
}

template <typename T> FORCEINLINE
int32 TCString<T>::Strnicmp(const CharType* String1, const CharType* String2, SIZE_T Count)
{
	return XPlatformString::Strnicmp(String1, String2, Count);
}

template <typename T> FORCEINLINE
int32 TCString<T>::Strlen(const CharType* String)
{
	return XPlatformString::Strlen(String);
}

template <typename T> FORCEINLINE
const typename TCString<T>::CharType* TCString<T>::Strstr(const CharType* String, const CharType* Find)
{
	return XPlatformString::Strstr(String, Find);
}

template <typename T> FORCEINLINE
typename TCString<T>::CharType* TCString<T>::Strstr(CharType* String, CharType* Find)
{
	return (CharType*)XPlatformString::Strstr(String, Find);
}

template <typename T> FORCEINLINE
const typename TCString<T>::CharType* TCString<T>::Strchar(const CharType* String, CharType c)
{
	return XPlatformString::Strchar(String, c);
}

template <typename T> FORCEINLINE
typename TCString<T>::CharType* TCString<T>::Strchar(CharType* String, CharType c)
{
	return (CharType*)XPlatformString::Strchar(String, c);
}

template <typename T> FORCEINLINE
const typename TCString<T>::CharType* TCString<T>::Strrchar(const CharType* String, CharType c)
{
	return XPlatformString::Strrchar(String, c);
}

template <typename T> FORCEINLINE
typename TCString<T>::CharType* TCString<T>::Strrchar(CharType* String, CharType c)
{
	return (CharType*)XPlatformString::Strrchar(String, c);
}

template <typename T> FORCEINLINE
const typename TCString<T>::CharType* TCString<T>::Strrstr(const CharType* String, const CharType* Find)
{
	return Strrstr((CharType*)String, Find);
}

template <typename T> FORCEINLINE
typename TCString<T>::CharType* TCString<T>::Strrstr(CharType* String, CharType* Find)
{
	if (*Find == (CharType)0)
	{
		return String + Strlen(String);
	}

	CharType* Result = nullptr;
	for (;;)
	{
		CharType* Found = Strstr(String, Find);
		if (!Found)
		{
			return Result;
		}
		Result = Found;
		String = Found + 1;
	}
}

template <typename T> FORCEINLINE
int32 TCString<T>::Strspn(const CharType* String, const CharType* Mask)
{
	const TCHAR* StringIt = String;
	while (*StringIt)
	{
		for (const TCHAR* MaskIt = Mask; *MaskIt; ++MaskIt)
		{
			if (*StringIt == *MaskIt)
			{
				goto NextChar;
			}
		}
		return StringIt - String;

	NextChar:
		++StringIt;
	}
	return StringIt - String;
}

template <typename T> FORCEINLINE
int32 TCString<T>::Strcspn(const CharType* String, const CharType* Mask)
{
	const TCHAR* StringIt = String;
	while (*StringIt)
	{
		for (const TCHAR* MaskIt = Mask; *MaskIt; ++MaskIt)
		{
			if (*StringIt == *MaskIt)
			{
				return StringIt - String;
			}
		}
		++StringIt;
	}
	return StringIt - String;
}

template <typename T> FORCEINLINE
int32 TCString<T>::Atoi(const CharType* String)
{
	return XPlatformString::Atoi(String);
}

template <typename T> FORCEINLINE
int64 TCString<T>::Atoi64(const CharType* String)
{
	return XPlatformString::Atoi64(String);
}

template <typename T> FORCEINLINE
float TCString<T>::Atof(const CharType* String)
{
	return XPlatformString::Atof(String);
}

template <typename T> FORCEINLINE
double TCString<T>::Atod(const CharType* String)
{
	return XPlatformString::Atod(String);
}

template <typename T> FORCEINLINE
double TCString<T>::Atod(const CharType* String)
{
	return XPlatformString::Atod(String);
}

template <typename T> FORCEINLINE
int32 TCString<T>::Strtoi(const CharType* Str, CharType** End, int32 Base)
{
	return XPlatformString::Strtoi(Str, End, Base);
}

template <typename T> FORCEINLINE
int64 TCString<T>::Strtoi64(const CharType* Str, CharType** End, int32 Base)
{
	return XPlatformString::Strtoi64(Str, End, Base);
}

template <typename T> FORCEINLINE
uint64 TCString<T>::Strtoui64(const CharType* Str, CharType** End, int32 Base)
{
	return XPlatformString::Strtoui64(Str, End, Base);
}

template <typename T> FORCEINLINE
typename TCString<T>::CharType* TCString<T>::Strtok(const CharType* TokenString, const CharType* Delim, CharType** Context)
{
	return XPlatformString::Strtok(TokenString, Delim, Context);
}

template <typename T> FORCEINLINE
int32 TCString<T>::GetVarArgs(CharType* Dest, SIZE_T DestSize, int32 Count, const CharType*& Fmt, va_list ArgPtr)
{
	return XPlatformString::GetVarArgs(Dest, DestSize, Count, Fmt, ArgPtr);
}

template <> FORCEINLINE
bool TCString<WIDECHAR>::IsPureAnsi(const CharType* Str)
{
	for (; *Str, Str++)
	{
		if (*Str > 0x7f)
		{
			return false;
		}
	}
	return true;
}

template <>
inline int32 TCString<WIDECHAR>::SprintfImpl(CharType* Dest, const CharType* Fmt, ...)
{
	int32 Result = -1;
	GET_VARARGS_RESULT_WIDE(Dest, MAX_SPRINTF, MAX_SPRINTF - 1, Fmt, Fmt, Result);
	return Result;
}

template <>
inline int32 TCString<WIDECHAR>::SnprintfImpl(CharType* Dest, int32 DestSize, const CharType* Fmt, ...)
{
	int32 Result = -1;
	GET_VARARGS_RESULT_WIDE(Dest, DestSize, DestSize - 1, Fmt, Fmt, Result);
	return Result;
}

template <>
FORCEINLINE bool TCString<TCHAR>::ToBool(const CharType* String)
{
	return XToBoolHelper::FromCStringWide(String);
}

//////////////////////////////////////////////////

template <> FORCEINLINE bool TCString<ANSICHAR>::IsPureAnsi(const CharType* Str)
{
	return true;
}

template <>
inline int32 TCString<ANSICHAR>::SprintfImpl(CharType* Dest, const CharType* Fmt, ...)
{
	int32 Result = -1;
	GET_VARARGS_RESULT_ANSI(Dest, MAX_SPRINTF, MAX_SPRINTF - 1, Fmt, Fmt, Result);
	return Result;
}

template <>
inline int32 TCString<ANSICHAR>::SnprintfImpl(CharType* Dest, int32 DestSize, const CharType* Fmt, ...)
{
	int32 Result = -1;
	GET_VARARGS_RESULT_ANSI(Dest, DestSize, DestSize - 1, Fmt, Fmt, Result);
	return Result;
}

template <>
FORCEINLINE bool TCString<ANSICHAR>::ToBool(const CharType* String)
{
	return XToBoolHelper::FormCStringAnsi(String);
}