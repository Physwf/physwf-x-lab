#pragma once

#include <cmath>

#include "Vector.h"

class Filter
{
public:
	virtual ~Filter() {};

	Filter(const Vector2f& Inradius) :radius(Inradius), invradius(1.0f/ radius.X, 1.0f / radius.Y) {}

	virtual float Evaluate(const Vector2f& p) const = 0;

	const Vector2f radius, invradius;
};

class GaussianFilter : public Filter
{
public:
	GaussianFilter(const Vector2f& radius, float Inalpha)
		: Filter(radius)
		, alpha(Inalpha)
		, expX(std::exp(-alpha * radius.X * radius.X))
		, expY(std::exp(-alpha * radius.Y * radius.Y))
	{}

	float Evaluate(const Vector2f& p) const;
private:
	const float alpha;
	const float expX, expY;

	float Gaussian(float d, float expv) const
	{
		return std::max(float(0), float(std::exp(-alpha*d*d) - expv));
	}
};