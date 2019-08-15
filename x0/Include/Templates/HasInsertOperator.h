#pragma once

namespace XHasInserterOperator_Private
{
	struct FNotSpecified {};

	template <typename T>
	struct FreturnValueCheck
	{
		static char(&Func())[2];
	};

	template <>
	struct FReturnValueCheck<FNotSpecified>
	{
		static char(&Func())[1];
	};

	template <typename Dest,typename T>

	template <typename T>
	T&& Make();

	template <typename Dest, typename T, typename decltype(Make<Dest&>() << Make<T&>())>
	struct THasInserterOperator
	{
		enum { Value = true };
	};

	template <typename Dest, typename T>
	struct THasInserterOperator<Dest, T, FNotSpecified>
	{
		enum { Value = false };
	};
}

template <typename Dest, typename T>
struct THasInserterOperator
{
	enum { Value = XHasInserterOperator_Private::THasInserterOperator<Dest,T>::Value };
}