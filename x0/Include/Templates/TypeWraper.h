#pragma once

template <typename T>
struct TTypeWraper;

template <typename T>
struct TUnwrapType
{
	typedef T Type;
};

template <typename T>
struct TUnwrapType<TTypeWraper<T>>
{
	typedef T Type;
};