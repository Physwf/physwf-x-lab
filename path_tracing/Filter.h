#pragma once

#include "Vector.h"

class Filter
{
public:
	virtual ~Filter();

	Filter(const Vector2f& Inradius) :radius(Inradius), invradius(1.0f/ radius.X, 1.0f / radius.Y) {}

	virtual float Evaluate(const Vector2f& p) const = 0;

	const Vector2f radius, invradius;
};