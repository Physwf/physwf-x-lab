#pragma once

#include <initializer_list>
#include "VectorHelper.h"
#include "Mathematics.h"

#include "DirectXMath.h"
using namespace DirectX;

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
	Vector2(T InX, T InY) : X(InX), Y(InY) {}

	template <typename U>
	explicit Vector2(const Vector2<U>& p)
	{
		X = T(p.X);
		Y = T(p.Y);
	}

	VectorBody(Vector2<T>)

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

	template <typename U>
	explicit Vector3(const Vector3<U>& p)
	{
		X = T(p.X);
		Y = T(p.Y);
	}

	XMFLOAT3 ToXMFloat3() const
	{
		return XMFLOAT3(X,Y,Z);
	}

	VectorBody(Vector3<T>)

	Vector3 Cross(const Vector3& rhs) const
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

	VectorBody(Vector4<T>)
};

template <typename T>
inline Vector2<T> operator*(float Value, const Vector2<T>& v)
{
	return v * Value;
}

template <typename T>
inline Vector3<T> operator*(float Value, const Vector3<T>& v)
{
	return v * Value;
}

template<typename T>
inline T Cross(const Vector2<T>& lhs, const Vector2<T>& rhs)
{
	return lhs.Cross(rhs);
}

template<typename T>
inline T Dot(const Vector2<T>& lhs, const Vector2<T>& rhs)
{
	return lhs.Dot(rhs);
}

template<typename T>
inline Vector3<T> Cross(const Vector3<T>& lhs,const Vector3<T>& rhs)
{
	return lhs.Cross(rhs);
}

template<typename T>
inline T Dot(const Vector3<T>& lhs, const Vector3<T>& rhs)
{
	return lhs.Dot(rhs);
}

template <typename T>
inline T AbsDot(const Vector3<T>& v1, const Vector3<T>& v2)
{
	return std::abs(Dot(v1, v2));
}

template<typename T>
inline Vector2<T> Normalize(const Vector2<T>& V)
{
	return V / V.Length();
}

template<typename T>
inline Vector3<T> Normalize(const Vector3<T>& V)
{
	return V / V.Length();
}

template <typename T>
inline Vector2<T> Min(const Vector2<T>& p1, const Vector2<T>& p2)
{
	return Vector2<T>(std::min(p1.X, p2.X), std::min(p1.Y, p2.Y));
}

template <typename T>
inline Vector3<T> Min(const Vector3<T>& p1, const Vector3<T>& p2)
{
	return Vector3<T>(std::min(p1.X, p2.X), std::min(p1.Y, p2.Y), std::min(p1.Z, p2.Z));
}

template <typename T>
inline Vector2<T> Max(const Vector2<T>& p1, const Vector2<T>& p2)
{
	return Vector2<T>(std::max(p1.X, p2.X), std::max(p1.Y, p2.Y));
}

template <typename T>
inline Vector3<T> Max(const Vector3<T>& p1, const Vector3<T>& p2)
{
	return Vector3<T>(std::max(p1.X, p2.X), std::max(p1.Y, p2.Y), std::max(p1.Z, p2.Z));
}

template <typename T>
inline Vector3<T> Floor(const Vector3<T>& p1)
{
	return Vector3<T>(std::floor(p1.X), std::floor(p1.Y), std::floor(p1.Z));
}

template <typename T>
inline Vector2<T> Floor(const Vector2<T>& p1)
{
	return Vector2<T>(std::floor(p1.X), std::floor(p1.Y));
}

template <typename T>
inline Vector3<T> Ceil(const Vector3<T>& p1)
{
	return Vector3<T>(std::ceil(p1.X), std::ceil(p1.Y), std::ceil(p1.Z));
}

template <typename T>
inline Vector2<T> Ceil(const Vector2<T>& p1)
{
	return Vector2<T>(std::ceil(p1.X), std::ceil(p1.Y));
}

template<typename T>
inline T Distance(const Vector3<T>& p1, const Vector3<T>& p2)
{
	return (p1 - p2).Length();
}
template <typename T>
inline float DistanceSquared(const Vector3<T>& p1, const Vector3<T>& p2)
{
	return (p1 - p2).LengthSquared();
}

template <typename T>
inline Vector2<T> Lerp(float t, const Vector2<T>& v0, const Vector2<T>& v1)
{
	return (1.0f - t) * v0 + t * v1;
}

template <typename T>
inline Vector3<T> Lerp(float t, const Vector3<T>& v0, const Vector3<T>& v1)
{
	return (1.0f - t) * v0 + t * v1;
}
template <typename T>
inline Vector3<T> Faceforward(const Vector3<T>& n, const Vector3<T>& v)
{
	return (Dot(n, v) < 0.f) ? -n : n;
}


typedef Vector2<int> Vector2i; 
typedef Vector3<int> Vector3i; 
typedef Vector4<int> Vector4i;
typedef Vector2<float> Vector2f;
typedef Vector3<float> Vector3f;
typedef Vector4<float> Vector4f;


inline float CosTheta(const Vector3f& v) { return v.Z; }
inline float AbsCosTheta(const Vector3f& v) { return std::abs(v.Z); }
inline float Cos2Theta(const Vector3f& v) { return v.Z * v.Z; }
inline float Sin2Theta(const Vector3f& v) { return std::max(0.f,1.0f - Cos2Theta(v)); }
inline float SinTheta(const Vector3f& v) { return std::sqrtf(Sin2Theta(v)); }
inline float TanTheta(const Vector3f& v) { return SinTheta(v) / CosTheta(v); }
inline float Tan2Theta(const Vector3f& v) { return Sin2Theta(v) / Cos2Theta(v); }
inline float CosPhi(const Vector3f& v) 
{
	float sinTheta = SinTheta(v);
	return sinTheta == 0 ? 1 : Math::Clamp(v.X / sinTheta, -1.0f, 1.0f);
}
inline float SinPhi(const Vector3f& v)
{
	float sinTheta = SinTheta(v);
	return sinTheta == 0 ? 0 : Math::Clamp(v.Y / sinTheta, -1.0f, 1.0f);
}
inline float Cos2Phi(const Vector3f& v)
{
	float cosPhi = CosPhi(v);
	return cosPhi * cosPhi;
}
inline float Sin2Phi(const Vector3f& v)
{
	float sinPhi = SinPhi(v);
	return sinPhi * sinPhi;
}
inline float CosDeltaPhi(const Vector3f& va, const Vector3f& vb)
{
	//ignore z, and calculate dot()
	return Math::Clamp((va.X * vb.X + va.Y * vb.Y) / std::sqrt((va.X * va.X + va.Y * va.Y) * (vb.X * vb.X + vb.Y * vb.Y)), -1.0f, 1.0f);
}
inline Vector3f Reflect(const Vector3f& wo, const Vector3f& n)
{
	return (-wo + 2.f * Dot(wo, n) * n);
}
inline bool Refract(const Vector3f& wi, const Vector3f& n,float etaT, Vector3f* wt)
{

}
inline bool SameHemisphere(const Vector3f& w, const Vector3f& wp) 
{
	return w.Z * wp.Z > 0;
}
template <typename T>
inline void CoordinateSystem(const Vector3<T>& v1, Vector3<T>* v2, Vector3<T>* v3)
{
	if (std::abs(v1.X) > std::abs(v1.Y))
	{
		*v2 = Vector3<T>(-v1.Z, 0, v1.X) / std::sqrt(v1.X * v1.X + v1.Z * v1.Z);
	}
	else
	{
		*v2 = Vector3<T>(0, v1.Z, -v1.Y) / std::sqrt(v1.Y * v1.Y + v1.Z * v1.Z);
	}
	*v3 = Cross(v1, *v2);
}
inline Vector3f SphericalDirection(float sinTheta, float cosTheta, float phi)
{
	return Vector3f(sinTheta * std::cos(phi), sinTheta * std::sin(phi), cosTheta);
}
inline Vector3f SphericalDirection(float sinTheta, float cosTheta, float phi, const Vector3f& x, const Vector3f& y, const Vector3f& z)
{
	return sinTheta * std::cos(phi) * x + sinTheta * std::sin(phi) * y + cosTheta * z;
}
float SphericalTheta(const Vector3f& v);
float SphericalPhi(const Vector3f& v);

