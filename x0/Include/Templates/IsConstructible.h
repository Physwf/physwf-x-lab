#pragma once

template <typename T, typename... Args>
struct TIsConstructible
{
	enum { Value = { __Is_constructible(T,Args...)} };
};