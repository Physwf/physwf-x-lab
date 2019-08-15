#pragma once

namespace XIsTriviallyDestructible_Private
{
	template <typename T, bool bIsTriviallyDesctructible = __is_enum(T)>
	struct TImpl
	{
		enum { Value = true };
	};

	template <typename T>
	struct TImpl<T, false>
	{
		enum { Value = __has_trivial_destructor(T) };
	};
}

template <typename T>
struct TIsTriviallyDestructible
{
	enum { Value = XIsTriviallyDestructible_Private::TImpl<T>::Value };
};