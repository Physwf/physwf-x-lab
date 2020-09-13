#pragma once

#include "filter.h"

class TriangleFilter : public Filter
{
public:
	TriangleFilter(const Vector2f& radius) : Filter(radius) {}

	virtual Float Evaluate(const Point2f &p) const override;
};

TriangleFilter *CreateTriangleFilter(const ParamSet &ps);