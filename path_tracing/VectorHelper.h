#pragma once

template<typename T, int Count>
class VectorHelper
{
public:
	inline void Add(const T* lhs, const T* rhs, T* Result)
	{
		for (int i = 0; i < Count; ++i)
		{
			Result[i] = lhs[i] + rhs[i];
		}
	}
	inline void Subtract(const T* lhs, const T* rhs, T* Result)
	{
		for (int i = 0; i < Count; ++i)
		{
			Result[i] = lhs[i] - rhs[i];
		}
	}
	inline void Minus(const T* V)
	{
		for (int i = 0; i < Count; ++i)
		{
			Result[i] = -V[i];
		}
	}
	inline T Dot(const T* lhs, const T* rhs)
	{
		T Result = 0;
		for (int i = 0; i < Count; ++i)
		{
			Result += lhs[i] * rhs[i];
		}
		return Result;
	}
	inline T Cross2(const T* lhs, const T* rhs)
	{
		return lhs[0] * rhs[1] - lhs[1] - rhs[0];
	}
	inline void Cross3(const T* lhs, const T* rhs, T* Result)
	{
		Result[0] = lhs[1] * rhs[2] - lhs[2] * rhs[1];
		Result[1] = lhs[0] * rhs[2] - lhs[2] * rhs[0];
		Result[2] = lhs[0] * rhs[1] - lhs[1] * rhs[0];
	}
	template<typename U>
	inline void Multiply(const T* lhs, U value, T* Result)
	{
		for (int i = 0; i < Count; ++i)
		{
			Result[i] = lhs[i] * (T)value;
		}
	}
	inline void Multiply(const T* lhs, const T* rhs, T* Result)
	{
		for (int i = 0; i < Count; ++i)
		{
			Result[i] = lhs[i] * rhs[i];
		}
	}
	template<typename U>
	inline void Devide(const T* lhs, U value, T* Result)
	{
		for (int i = 0; i < Count; ++i)
		{
			Result[i] = lhs[i] / (T)value;
		}
	}
	inline void Devide(const T* lhs, const T* rhs, T* Result)
	{
		for (int i = 0; i < Count; ++i)
		{
			Result[i] = lhs[i] / rhs[i];
		}
	}
	inline bool Equal(const T* lhs, const T* rhs)
	{
		for (int i = 0; i < Count; ++i)
		{
			if (lhs[i] != rhs[i]) return false;
		}
		return true;
	}
	inline bool IsNearlyEqual(const T* lhs, const T* rhs)
	{
		for (int i = 0; i < Count; ++i)
		{
			if (!Math::IsNearlyEqual(lhs[i], rhs[i])) return false;
		}
		return true;
	}
};

#define VectorBody(Type)									\
Type operator+(const Type& rhs) const						\
{															\
	Type Result;											\
	Add(Elements, rhs.Elements, Result.Elements);			\
	return Result;											\
}															\
Type operator-(const Type& rhs)	const						\
{															\
	Type Result;											\
	Subtract(Elements, rhs.Elements, Result.Elements);		\
	return Result;											\
}															\
Type operator-() const										\
{															\
	Type Result;											\
	Minus(Result.Elements);									\
	return Result;											\
}															\
Type operator*(const Type& rhs)	 const						\
{															\
	Type Result;											\
	Multiply(Elements, rhs.Elements, Result.Elements);		\
	return Result;											\
}															\
template<typename U>										\
Type operator*(U value)	const								\
{															\
	Vector3 Result;											\
	Multiply<U>(Elements, value, Result.Elements);			\
	return Result;											\
}															\
Type operator/(const Type& rhs)	const						\
{															\
	Type Result;											\
	Devide(Elements, rhs.Elements, Result.Elements);		\
	return Result;											\
}															\
template <typename U>										\
Type operator/(U value)	const								\
{															\
	Type Result;											\
	Devide<U>(Elements, value, Result.Elements);			\
	return Result;											\
}															\
T Dot(const Type& rhs)	const								\
{															\
	return Dot(Elements, rhs.Elements);						\
}															\
T LengthSquared()	const									\
{															\
	return Dot(Elements, Elements);							\
}															\
T Length()	const											\
{															\
	return std::sqrt(LengthSquared());						\
}															\
bool operator==(const Type& rhs) const						\
{															\
	return Equal(Elements, rhs.Elements);					\
}															\
bool operator!=(const Type& rhs) const						\
{															\
	return !Equal(Elements, rhs.Elements);					\
}															\
bool IsNearlyEqual(const Type& rhs) const					\
{															\
	return IsNearlyEqual(Elements, rhs.Elements);			\
}															\
															\
T operator[](int i) const									\
{															\
	return Elements[i];										\
}															