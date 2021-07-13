#include "Light.h"
#include "Sampling.h"
#include "Scene.h"

bool VisibilityTester::Unoccluded(const Scene& scene) const
{
	return scene.IntersectP(p0.SpawnRayTo(p1));
}

Light::Light(int flags, const Transform& LightToWorld, int nSamples)
	:flags(flags),
	nSamples(std::max(1,nSamples)),
	LightToWorld(LightToWorld),
	WorldToLight(Inverse(LightToWorld))
{

}

LinearColor Light::Le(const Ray& r) const
{
	return LinearColor(0.f);
}

AreaLight::AreaLight(const Transform& LightToWorld, int nSamples)
	: Light((int)LightFlags::Area, LightToWorld, nSamples) 
{
}

LinearColor PointLight::Sample_Li(const Interaction& ref, const Vector2f& u, Vector3f* wi, float* pdf, VisibilityTester* vis) const
{
	*wi = Normalize(pLight - ref.p);
	*pdf = 1.f;
	*vis = VisibilityTester(ref,Interaction(pLight));
	return I / DistanceSquared(pLight , ref.p);
}

LinearColor PointLight::Power() const
{
	return 4 * PI * I;
}

float PointLight::Pdf_Li(const Interaction& ref, const Vector3f& wi) const
{
	return 0;
}

LinearColor PointLight::Sample_Le(const Vector2f& u1, const Vector2f& u2, Ray* ray, Vector3f* nLight, float* pdfPos, float* pdfDir) const
{
	*ray = Ray(pLight, UniformSampleSphere(u1));
	*nLight = ray->d;
	*pdfPos = 1;
	*pdfDir = UniformSpherePdf();
	return I;
}

void PointLight::Pdf_Le(const Ray& ray, const Vector3f& nLight, float* pdfPos, float* pdfDir) const
{
	*pdfPos = 0;
	*pdfDir = UniformSpherePdf();
}

UniformLightDistribution::UniformLightDistribution(const Scene& scene)
{
	std::vector<float> prob(scene.lights.size(), 1.f);
	distrib.reset(new Distribution1D(prob.data(),(int)prob.size()));
}

const Distribution1D* UniformLightDistribution::Lookup(const Vector3f& p) const
{
	return distrib.get();
}
