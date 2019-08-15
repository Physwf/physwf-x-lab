#pragma once

struct XIdentityFunctor
{
	template <typename T>
	T&& operator()(T&& Val) const
	{
		return (T&&)Val;
	}
};