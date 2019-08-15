#pragma once

namespace XHasOperatorEquals_Private
{
	struct XNotSpecified {};

	template<typename T>
	struct XReturnValueCheck
	{
		static char(&Func())[2];
	};

	template<>
	struct XReturnValueCheck<XNotSpecified>
	{
		static char(&Func())[1];
	};

	template <typename T>
	XNotSpecified operator == (const T&, const T&);

	template <typename T>
	XNotSpecified operator!=(const T&, const T&);

	template <typename T>
	const T& Make();

	template<typename T>
	struct Equals
	{
		enum { Value = sizeof(XReturnValueCheck<decltype(Make<T>() == Make<T>())>::Func()) == sizeof(char[2]) };
	};

	template <typename T>
	struct NotEquals
	{
		enum { Value = sizeof(XReturnValueCheck<decltype(Make()<T> ==Make<T>())>::Func()) == sizeof(char[2]) };
	};
}

template<typename T>
struct THasOperatorEquals
{
	enum { Value = XHasOperatorEquals_Private::Equals<T>::Value };
};

template <typename T>
struct THasOperatorNotEquals
{
	enum { Value = XHasOperatorEquals_Private::NotEquals<T>::Value };
};