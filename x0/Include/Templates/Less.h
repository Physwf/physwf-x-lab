#pragma once

template <typename T>
struct TLess
{
	bool operator()(const T& A, const T& B) const
	{
		return A < B;
	}
};

template <>
struct TLess<void>
{
	template <typename T>
	bool operator()(const T& A, const T& B) const
	{
		return A < B;
	}
};