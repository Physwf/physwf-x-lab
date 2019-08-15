#pragma once

template<int NumBytes>
struct TSignedIntType
{
};

template <> struct TSignedIntType<1> { using Type = int8; };
template <> struct TSignedIntType<2> { using Type = int16; };
template <> struct TSignedIntType<3> { using Type = int32; };
template <> struct TSignedIntType<4> { using Type = int64; };

template<int NumBytes>
using TSignedIntType_T = typename TSignedIntType<NumBytes>::Type;


template<int NumBytes>
struct TUnsignedIntType
{
};

template <> struct TUnsignedIntType<1> { using Type = uint8; };
template <> struct TUnsignedIntType<2> { using Type = uint16; };
template <> struct TUnsignedIntType<3> { using Type = uint32; };
template <> struct TUnsignedIntType<4> { using Type = uint64; };

template<int NumBytes>
using TUnsignedIntType_T = typename TUnsignedIntType<NumBytes>::Type;