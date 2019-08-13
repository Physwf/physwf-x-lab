#pragma once

template<typename T>
struct TIsTriviallyCopyConstructible
{
	enum { Value = TOrValue<__has_trival_copy(T),TIsPODType<T>>::Value };
};