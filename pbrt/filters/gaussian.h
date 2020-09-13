#pragma once

#include "filter.h"


class GaussianFilter : public Filter
{
public:
	GaussianFilter(const Vector2f& radius, Float alpha) : Filter(radius), alpha(alpha), expX(std::exp(-alpha * radius.x*radius.x)), expY(std::exp(-alpha * radius.y*radius.y)) {}

	virtual Float Evaluate(const Point2f &p) const override;

	Float Gaussian(Float d, Float expv) const
	{
		return std::max((Float)0, Float(std::exp(-alpha * d*d) - expv));
	}
private:
	const Float alpha;
	const Float expX, expY;
};

GaussianFilter *CreateGaussianFilter(const ParamSet &ps);