#pragma once

#include <initializer_list>
#include "VectorHelper.h"

template <typename T>
class Vector2 : private VectorHelper<T, 2>
{
public:
	union
	{
		struct { T X, Y; };
		T Elements[2];
	};

	Vector2() :X(0), Y(0) {}
	Vector2(T InX, T InY, T InZ) : X(InX), Y(InY) {}

	VectorBody(Vector2)

	T Cross(const Vector2& rhs)
	{
		return Cross2(Elements, rhs.Elements);
	}
};

template <typename T>
class Vector3 : private VectorHelper<T,3>
{
public:
	union
	{
		struct { T X, Y, Z; };
		T Elements[3];
	};

	Vector3():X(0),Y(0),Z(0){}
	Vector3(T InX, T InY, T InZ) : X(InX), Y(InY), Z(InZ){}

	VectorBody(Vector3)

	Vector3 Cross(const Vector3& rhs)
	{
		Vector3 Result;
		Cross3(Elements, rhs.Elements, Result.Elements);
		return Result;
	}
};

template <typename T>
class Vector4 : private VectorHelper<T, 4>
{
public:
	union
	{
		struct { T X, Y, Z, W; };
		T Elements[4];
	};

	Vector4() :X(0), Y(0), Z(0), W(0) {}
	Vector4(T InX, T InY, T InZ) : X(InX), Y(InY), Z(InZ) {}

	VectorBody(Vector4)
};

template<typename T>
T Cross(const Vector2<T>& lhs, const Vector2<T>& rhs)
{
	return lhs.Cross(rhs);
}

template<typename T>
T Dot(const Vector2<T>& lhs, const Vector2<T>& rhs)
{
	return lhs.Dot(rhs);
}

template<typename T>
Vector3<T> Cross(const Vector3<T>& lhs,const Vector3<T>& rhs)
{
	return lhs.Cross(rhs);
}

template<typename T>
T Dot(const Vector3<T>& lhs, const Vector3<T>& rhs)
{
	return lhs.Dot(rhs);
}

typedef Vector2<int> Vector2i; 
typedef Vector3<int> Vector3i; 
typedef Vector4<int> Vector4i;
typedef Vector2<float> Vector2f;
typedef Vector3<float> Vector3f;
typedef Vector4<float> Vector4f;
