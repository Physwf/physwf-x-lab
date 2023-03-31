#include "BodyScattering.h"

LinearColor TabulatedBSSRDF::Sr(float d) const
{
	LinearColor Sr(0.f);
	return Sr.Clamp();
}

