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

	explicit Bounds2(const Point2<T> &p) : pMin(p), pMax(p) {}

	Bounds2(const Point2<T> &p1, const Point2<T> &p2) 
	{
		pMin = Vector2<T>(std::min(p1.x, p2.x), std::min(p1.y, p2.y));
		pMax = Vector2<T>(std::max(p1.x, p2.x), std::max(p1.y, p2.y));
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
		return (d.x * d.y);
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
private:
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
		pMin(std::min(p1.x, p2.x), std::min(p1.y, p2.y), std::min(p1.z, p2.z)),
		pMax(std::max(p1.x, p2.x), std::max(p1.y, p2.y), std::max(p1.z, p2.z)) 
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
	Vector3<T> Diagonal() const { return pMax - pMin; }
	T SurfaceArea() const 
	{
		Vector3<T> d = Diagonal();
		return 2 * (d.x * d.y + d.x * d.z + d.y * d.z);
	}
	T Volume() const 
	{
		Vector3<T> d = Diagonal();
		return d.x * d.y * d.z;
	}
private:
	Vector3<T> pMin, pMax;
};