#include "GenericPlatform/GenericPlatformString.h"
#include "HAL/XMemeory.h"
#include "Misc/Char.h"
#include "Containers/XString.h"

template<> const TCHAR* XGenericPlatformString::GetEncodingTypeName<ANSICHAR>() { return TEXT("ANSICHAR"); }
template<> const TCHAR* XGenericPlatformString::GetEncodingTypeName<WIDECHAR>() { return TEXT("WIDECHAR"); }
template<> const TCHAR* XGenericPlatformString::GetEncodingTypeName<UCS2CHAR>() { return TEXT("UCS2CHAR"); }

X0_API void* XGenericPlatformString::Memcpy(void* Dest, const void* Src, SIZE_T Count)
{
	return XMemory::Memcpy(Dest, Src, Count);
}


template <typename DestEncoding, typename SourceEncoding>
X0_API void XGenericPlatformString::LogBogusChars(const SourceEncoding* Src, int32 SrcSize)
{

}