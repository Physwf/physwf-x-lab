#include "Light.h"
#include "Sampling.h"
#include "Scene.h"

bool VisibilityTester::Unoccluded(const Scene& scene) const
{
	return !scene.IntersectP(p0.SpawnRayTo(p1));
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

std::unique_ptr<LightDistribution> CreateLightDistribution(const std::string& name, const Scene& scene)
{
	if (name == "uniform" || scene.lights.size() == 1)
		return std::unique_ptr<LightDistribution>{ new UniformLightDistribution(scene) };
	return nullptr;
}

LinearColor DisffuseAreaLight::L(const Interaction& intr, const Vector3f& w) const
{
	return (bTwoSide || Dot(intr.n, w) > 0) ? Lemit :LinearColor(0.f);
}

LinearColor DisffuseAreaLight::Power() const
{
	return (bTwoSide ? 2 : 1) * Lemit * area * PI;
}

LinearColor DisffuseAreaLight::Sample_Li(const Interaction& ref, const Vector2f& u, Vector3f* wi, float* pdf, VisibilityTester* vis) const
{
	Interaction pShape = shape->Sample(ref, u, pdf);
	if (*pdf == 0 || (pShape.p - ref.p).LengthSquared() == 0)
	{
		*pdf = 0;
		return 0.f;
	}
	*wi = Normalize(pShape.p - ref.p);
	*vis = VisibilityTester(ref, pShape);
	return L(pShape, -*wi);
}

float DisffuseAreaLight::Pdf_Li(const Interaction& ref, const Vector3f& wi) const
{
	return shape->Pdf(ref, wi);
}

LinearColor DisffuseAreaLight::Sample_Le(const Vector2f& u1, const Vector2f& u2, Ray* ray, Vector3f* nLight, float* pdfPos, float* pdfDir) const
{
	Interaction pShape = shape->Sample(u1, pdfPos);
	*nLight = pShape.n;
	Vector3f w;
	if (bTwoSide)
	{
		Vector2f u = u2;
		if (u[0] < .5f)
		{
			u[0] = std::min(u[0] * 2, OneMinusEpsilon);
			w = CosineSampleHemisphere(u);
		}
		else
		{
			u[0] = std::min((u[0] - 0.5f) * 2, OneMinusEpsilon);
			w = CosineSampleHemisphere(u);
			w.Z *= -1;
		}
		*pdfDir = 0.5f * CosineHemispherePdf(std::abs(w.Z));
	}
	else
	{
		w = CosineSampleHemisphere(u2);
		*pdfDir = CosineHemispherePdf(w.Z);
	}
	Vector3f v1, v2, n(pShape.n);
	CoordinateSystem(n, &v1, &v2);
	w = w.X * v1 + w.Y * v2 + w.Z * n;
	*ray = pShape.SpawnRay(w);
	return L(pShape, w);
}

void DisffuseAreaLight::Pdf_Le(const Ray& ray, const Vector3f& n, float* pdfPos, float* pdfDir) const
{
	Interaction it(ray.o, n, Vector3f(n));
	*pdfPos = shape->Pdf(it);
	*pdfDir = bTwoSide ? (.5 * CosineHemispherePdf(AbsDot(n, ray.d))) : CosineHemispherePdf(Dot(n, ray.d));
}
