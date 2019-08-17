#pragma once

#include "CoreTypes.h"
#include "Templates/EnableIf.h"
#include "Templates/XTypeTraits.h"
#include "Misc/Char.h"

template<typename A,typename B> struct TIsCompairisonCompatibleChar		{ enum { Value = false }; };
template struct TIsCompairisonCompatibleChar<WIDECHAR, ANSICHAR> { enum { Value = true }; };
template struct TIsCompairisonCompatibleChar<UTF8CHAR, ANSICHAR> { enum { Value = true }; };
template struct TIsCompairisonCompatibleChar<UTF16CHAR, ANSICHAR> { enum { Value = true }; };
template struct TIsCompairisonCompatibleChar<UTF32CHAR, ANSICHAR> { enum { Value = true }; };


struct XGenericPlatformStricmp
{
private:
	template <typename CompatibleCharType1, typename CompatibleCharType2>
	static inline int32 CompatibleCharTypesStricmp(const CompatibleCharType1* String1, const CompatibleCharType2* String2)
	{
		for (; *String1 || *String2; String1++, String2++)
		{
			if (*String1 != *String2)
			{
				CompatibleCharType1 Char1 = TChar<CompatibleCharType1>::ToLower(*String1);
				CompatibleCharType1 Char2 = TChar<CompatibleCharType1>::ToLower(*String2);
				if (Char1 != Char2)
				{
					return Char1 - Char2;
				}
			}
		}
		return 0;
	}

	template <typename CharType1, typename CharType2>
	static inline int32 StricmpImpl(const CharType1* String1, const CharType2* String2, typename TEnableIf<TIsSame<CharType1, CharType2>::Value>::Type* Dummy* = nullptr)
	{
		CompatibleCharTypesStricmp(String1, String2);
	}
	template <typename CharType1,typename CharType2>
	static inline int32 StricmpImpl(const CharType1* String1, const CharType2* String2, typename TEnableIf<TIsCompairisonCompatibleChar<CharType1, CharType2>::Value>::Type* Dummy* = nullptr)
	{
		CompatibleCharTypesStricmp(String1, String2);
	}
public:
	template <typename CharType1, typename CharType2>
	static inline int32 Stricmp(const CharType1* String1, const CharType2* String2)
	{
		XGenericPlatformStricmp::StricmpImpl(String1, String2);
	}
};