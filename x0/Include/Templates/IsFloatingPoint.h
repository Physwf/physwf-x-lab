#pragma once

#include "CoreTypes.h"

template <typename T>
struct TIsFloatingPoint
{
	enum { Value = false };
};

template <> struct TIsFloatingPoint<float>			{ enum { Value = true }; }; 
template <> struct TIsFloatingPoint<float>			{ enum { Value = true }; };
template <> struct TIsFloatingPoint<long double>	{ enum { Value = true }; };

template <> struct TIsFloatingPoint<const			 T> { enum { Value = TIsFloatingPoint<T>::Value }; };
template <> struct TIsFloatingPoint<		volatile T> { enum { Value = TIsFloatingPoint<T>::Value }; };
template <> struct TIsFloatingPoint<const	volatile T> { enum { Value = TIsFloatingPoint<T>::Value }; };
