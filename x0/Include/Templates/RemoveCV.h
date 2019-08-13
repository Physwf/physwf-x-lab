#pragma once

#include "CoreTypes.h"

template<typename T> struct TRemoveCV					{ typedef T Type; };
template<typename T> struct TRemoveCV<const T>			{ typedef T Type; };
template<typename T> struct TRemoveCV<volatile T>		{ typedef T Type; };
template<typename T> struct TRemoveCV<const volatile T> { typedef T Type; };
