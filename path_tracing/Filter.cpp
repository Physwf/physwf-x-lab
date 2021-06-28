#include "Filter.h"

float GaussianFilter::Evaluate(const Vector2f& p) const
{
	return Gaussian(p.X, expX) * Gaussian(p.Y, expY);
}

