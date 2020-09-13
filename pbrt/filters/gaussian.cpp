#include "gaussian.h"
#include "paramset.h"

Float GaussianFilter::Evaluate(const Point2f &p) const
{
	return Gaussian(p.x, expX) * Gaussian(p.y, expY);
}

GaussianFilter * CreateGaussianFilter(const ParamSet &ps)
{
	Float xw = ps.FindOneFloat("xwidth", 2.f);
	Float yw =  ps.FindOneFloat("ywidth", 2.f);
	Float alpha = ps.FindOneFloat("alpha", 2.f);
	return new GaussianFilter(Vector2f(xw, yw), alpha);
}
