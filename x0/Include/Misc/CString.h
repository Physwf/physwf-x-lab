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
struct TString
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
};