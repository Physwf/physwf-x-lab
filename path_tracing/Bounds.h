#pragma once

#include "Vector.h"
#include <limits>

template <typename T>
class Bounds2
{
public:
	Bounds2()
	{
		T minNum = std::numeric_limits<T>::lowest();
		T maxNum = std::numeric_limits<T>::max();
		pMin = Vector2<T>(maxNum, maxNum);
		pMax = Vector2<T>(minNum, minNum);
	}

	explicit Bounds2(const Vector2<T> &p) : pMin(p), pMax(p) {}

	Bounds2(const Vector2<T> &p1, const Vector2<T> &p2)
	{
		pMin = Vector2<T>(std::min(p1.X, p2.X), std::min(p1.Y, p2.Y));
		pMax = Vector2<T>(std::max(p1.X, p2.X), std::max(p1.Y, p2.Y));
	}

	template <typename U>
	explicit operator Bounds2<U>() const 
	{
		return Bounds2<U>((Vector2<U>)pMin, (Vector2<U>)pMax);
	}

	Vector2<T> Diagonal() const { return pMax - pMin; }

	T Area() const 
	{
		Vector2<T> d = pMax - pMin;
		return (d.X * d.Y);
	}
	inline const Vector2<T> &operator[](int i) const
	{
		return (i == 0) ? pMin : pMax;
	}
	inline Vector2<T> &operator[](int i) 
	{
		return (i == 0) ? pMin : pMax;
	}
	bool operator==(const Bounds2<T> &b) const 
	{
		return b.pMin == pMin && b.pMax == pMax;
	}
	bool operator!=(const Bounds2<T> &b) const {
		return b.pMin != pMin || b.pMax != pMax;
	}

	Vector2<T> pMin, pMax;
};

template <typename T>
class Bounds3
{
public:
	Bounds3() 
	{
		T minNum = std::numeric_limits<T>::lowest();
		T maxNum = std::numeric_limits<T>::max();
		pMin = Vector3<T>(maxNum, maxNum, maxNum);
		pMax = Vector3<T>(minNum, minNum, minNum);
	}
	explicit Bounds3(const Vector3<T> &p) : pMin(p), pMax(p) {}
	Bounds3(const Vector3<T> &p1, const Vector3<T> &p2) : 
		pMin(std::min(p1.X, p2.Y), std::min(p1.Y, p2.Y), std::min(p1.Z, p2.Z)),
		pMax(std::max(p1.X, p2.Y), std::max(p1.Y, p2.Y), std::max(p1.Z, p2.Z))
	{}
	const Vector3<T> &operator[](int i) const
	{
		return (i == 0) ? pMin : pMax;
	}
	Vector3<T> &operator[](int i)
	{
		return (i == 0) ? pMin : pMax;
	}
	bool operator==(const Bounds3<T> &b) const {
		return b.pMin == pMin && b.pMax == pMax;
	}
	bool operator!=(const Bounds3<T> &b) const {
		return b.pMin != pMin || b.pMax != pMax;
	}
	bool IntersectP(const Ray& ray, float* hit0 = NULL, float* hit1 = NULL)
	{
		float t0 = 0, t1 = ray.tMax;
		for (int i = 0;i < 3; ++i)
		{
			float invRayDir = 1.f / ray.d[i];
			float tNear = (pMin[i] - ray.o[i]) * invRayDir;
			float tFar = (pMax[i] - ray.o[i]) * invRayDir;

			if (tNear > tFar) std::swap(tNear, tFar);

			t0 = tNear > t0 ? tNear : t0;
			t1 = tFar < t1 ? tFar : t1;
			if (t0 > t1) return false;
		}
		if (hit0) *hit0 = t0;
		if (hit1) *hit1 = t1;
		return true;
	}
	Vector3<T> Diagonal() const { return pMax - pMin; }
	T SurfaceArea() const 
	{
		Vector3<T> d = Diagonal();
		return 2 * (d.X * d.Y + d.X * d.Z + d.Y * d.Z);
	}
	T Volume() const 
	{
		Vector3<T> d = Diagonal();
		return d.X * d.Y * d.Z;
	}
	Vector3<T> pMin, pMax;
};

template <typename T>
Bounds3<T> Union(const Bounds3<T>& b, const Vector3<T>& p)
{
	Bounds3<T> ret;
	ret.pMin = Min(b.pMin, p);
	ret.pMax = Max(b.pMax, p);
	return ret;
}

template <typename T>
Bounds3<T> Union(const Bounds3<T>& b1, const Bounds3<T>& b2)
{
	Bounds3<T> ret;
	ret.pMin = Min(b1.pMin, b2.pMin);
	ret.pMax = Max(b1.pMax, b2.pMax);
	return ret;
}

template <typename T>
Bounds2<T> Intersect(const Bounds2<T> &b1, const Bounds2<T>& b2)
{
	Bounds2<T> ret;
	ret.pMin = Max(b1.pMin, b2.pMin);
	ret.pMax = Min(b1.pMax, b2.pMax);
	return ret;
}

template <typename T>
Bounds3<T> Intersect(const Bounds3<T> &b1, const Bounds3<T>& b2)
{
	Bounds3<T> ret;
	ret.pMin = Max(b1.pMin, b2.pMin);
	ret.pMax = Min(b1.pMax, b2.pMax);
	return ret;
}

typedef Bounds2<int> Bounds2i; 
typedef Bounds2<float> Bounds2f;
typedef Bounds3<int> Bounds3i;
typedef Bounds3<float> Bounds3f;

