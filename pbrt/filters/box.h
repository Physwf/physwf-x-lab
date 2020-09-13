#pragma once

#include "filter.h"

class BoxFilter : public Filter
{
public:
	BoxFilter(const Vector2f& radius) : Filter(radius) {}

	virtual Float Evaluate(const Point2f &p) const override
	{
		return 1.0;
	}
};

BoxFilter* CreateBoxFilter(const ParamSet &ps);