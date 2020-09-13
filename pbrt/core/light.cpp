#include "light.h"
#include "scene.h"
#include "sampling.h"
//#include "stats.h"
#include "paramset.h"

Light::~Light()
{

}

Light::Light(int flags, const Transform & LightToWorld, 
	const MediumInterface & mediumInterface, int nSamples)
	:flags(flags),
	nSamples(std::max(1,nSamples)),
	mediumInterface(mediumInterface),
	LightToWorld(LightToWorld),
	WorldToLight(Inverse(LightToWorld))
{
}

void Light::Preprocess(const Scene& scene)
{

}

Spectrum Light::Le(const RayDifferential & r) const
{
	return Spectrum(0.f);
}

bool VisibilityTester::Unoccluded(const Scene &scene) const
{
	return !scene.IntersectP(p0.SpawnRayTo(p1));
}

Spectrum VisibilityTester::Tr(const Scene & scene, Sampler & sampler) const
{
	return Spectrum();
}

AreaLight::AreaLight(const Transform &LightToWorld, const MediumInterface &medium, int nSamples)
	:Light((int)LightFlags::Area,LightToWorld,medium,nSamples)
{

}
