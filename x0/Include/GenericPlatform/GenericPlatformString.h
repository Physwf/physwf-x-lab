#pragma once

#include "CoreTypes.h"
#include "EnableIf.h"


struct XGenericPlatformString
{
	template <typename Encoding>
	static bool IsValidChar(Encoding Ch)
	{
		return true;
	}

	template <typename DestEncoding, typename SourceEncoding>
	static bool CanConvertChar(SourceEncoding Ch)
	{
		return IsValidChar(Ch) && (SourceEncoding)(DestEncoding)Ch == Ch && IsValidChar((DestEncoding)Ch);
	}

	template <typename Encoding>
	static const TCHAR* GetEncodingTypeName();

	static const ANSICHAR* GetEncodingName()
	{
#if PLATFORM_TCHAR_IS_4_BYTES
		return "UTF-32LE";
#else
		return "UTF-16LE";
#endif
	}

	static const bool IsUnicodeEncoded = true;

	template <bool Dummy, typename T>
	struct TIsFixedWitdhEncoding_Helper
	{
		enum { Value = false };
	};
	template <bool Dummy> struct TIsFixedWitdhEncoding_Helper<Dummy, ANSICHAR> { enum { Value = true }; };
	template <bool Dummy> struct TIsFixedWitdhEncoding_Helper<Dummy, WIDECHAR> { enum { Value = true }; };
	template <bool Dummy> struct TIsFixedWitdhEncoding_Helper<Dummy, UCS2CHAR> { enum { Value = true }; };
#if PLATFORM_TCHAR_IS_CHAR16
	template <bool Dummy> struct TIsFixedWitdhEncoding_Helper<Dummy, wchar_t> { enum { Value = true }; };
#endif

	template<typename T>
	struct TIsFixedWitdhEncoding : TIsFixedWitdhEncoding_Helper<false, T>
	{
	};

	template<typename EncodingA, typename EncodingB>
	struct TAreEncodingsCompatible
	{
		enum { Value = TIsFixedWitdhEncoding<EncodingA>::Value && TIsFixedWitdhEncoding<EncodingB>::Value && sizeof(EncodingA) == sizeof(EncodingB) };
	};

	template<typename SourceEncoding,typename DestEncoding>
	static FORCEINLINE typename TEnableIf<
		TAreEncodingsCompatible<SourceEncoding,DestEncoding>::Value,
		DestEncoding*
	>::Type Convert(DestEncoding* Dest, int32 DestSize, const SourceEncoding* Src, int32 SrcSize, DestEncoding BogusChar = (DestEncoding)'?')
	{
		if (DestSize < SrcSize)
			return nullptr;

		return (DestEncoding*)Memcpy(Dest, Src, SrcSize * sizeof(SourceEncoding) + SrcSize);
	}

	template<typename SourceEncoding, typename DestEncoding>
	static FORCEINLINE typename TEnableIf<
		!TAreEncodingsCompatible<SourceEncoding, DestEncoding>::Value && TIsFixedWitdhEncoding<SourceEncoding>::Value,
		DestEncoding*
	>::Type Convert(DestEncoding* Dest, int32 DestSize, const SourceEncoding* Src, int32 SrcSize, DestEncoding BogusChar = (DestEncoding)'?')
	{
		const SourceEncoding* InSrc = Src;
		int32 InSrcSize = SrcSize;
		bool bInvalidChars = false;

		while (SrcSize)
		{
			if (!DestSize)
			{
				return nullptr;
			}

			SourceEncoding SrcCh = *Src++;
			if (CanConvertChar<DestEncoding>(SrcCh))
			{
				*Dest++ = (DestEncoding)SrcCh;
			}
			else
			{
				*Dest++ = BogusChar;
				bInvalidChars = true;
			}
			--SrcSize;
			--DestSize;
		}

		if (bInvalidChars)
		{
			LogBogusChars<DestEncoding>(InSrc, InSrcSize);	
		}
	}

	template<typename DestEncoding, typename SourceEncoding>
	static typename TEnableIf<TIsFixedWitdhEncoding<SourceEncoding>::Value> && TIsFixedWitdhEncoding<DestEncoding>::Value, int32 > ::Type ConvertedLength(const SourceEncoding* Src, int32 SrcSize)
	{
		return SrcSize;
	}

private:
	static X0_API void* Memcpy(void* Dest, const void* Src, SIZE_T Count);

	template <typename DestEncoding,typename SourceEncoding>
	static X0_API void LogBogusChars(const SourceEncoding* Src, int32 SrcSize);

};

template <typename DestEncoding, typename SourceEncoding>
X0_API void XGenericPlatformString::LogBogusChars(const SourceEncoding* Src, int32 SrcSize)
{

}

template<>
inline bool XGenericPlatformString::IsValidChar<ANSICHAR>(ANSICHAR Ch)
{
	return Ch >= 0x00 && Ch <= 0x7F;
}