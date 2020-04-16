#pragma once

/**
 * A functor which returns whatever is passed to it.  Mainly used for generic composition.
 */
struct FIdentityFunctor
{
	template <typename T>
	inline T&& operator()(T&& Val) const
	{
		return (T&&)Val;
	}
};
