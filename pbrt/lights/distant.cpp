#include "distant.h"
#include "paramset.h"
#include "sampling.h"

DistantLight::DistantLight(const Transform& LightToWorld, const Spectrum& L, const Vector3f &w)
	:Light((int)LightFlags::DeltaDirection, LightToWorld, MediumInterface()),
	L(L),
	wLight(Normalize(LightToWorld(wLight))) {}

Spectrum DistantLight::Sample_Li(const Interaction& ref, const Point2f& u, Vector3f *wi, Float* pdf, VisibilityTester* vis) const
{
	*wi = wLight;
	*pdf = 1;
	Point3f pOutside = ref.p + wLight * (2 * worldRadius);
	*vis = VisibilityTester(ref, Interaction(pOutside, ref.time, mediumInterface));
	return L;
}

Spectrum DistantLight::Power() const
{
	return L * Pi * worldRadius *worldRadius;
}

Float DistantLight::Pdf_Li(const Interaction& ref, const Vector3f& wi) const
{
	return 0;
}

Spectrum DistantLight::Sample_Le(const Point2f &u1, const Point2f& u2, Float time, Ray* ray, Normal3f *nLight, Float *pdfPos, Float *pdfDir) const
{
	Vector3f v1, v2;
	CoordinateSystem(wLight, &v1, &v2);
	Point2f cd = ConcentricSampleDisk(u1);
	Point3f pDisk = worldCenter + worldRadius * (cd.x*v1, cd.y*v2);
	*ray = Ray(pDisk + worldRadius * wLight, -wLight, Infinity, time);
	*nLight = (Normal3f)ray->d;
	*pdfPos = 1 / (Pi * worldRadius * worldRadius);
	*pdfDir = 1;
	return L;
}

void DistantLight::Pdf_Le(const Ray &ray, const Normal3f &nLight, Float *pdfPos, Float *pdfDir) const
{
	*pdfPos = 1 / (Pi * worldRadius * worldRadius);
	*pdfDir = 0;
}

std::shared_ptr<DistantLight> CreateDistantLight(const Transform& light2world,const ParamSet &paramSet)
{
	Spectrum L = paramSet.FindOneSpectrum("L", Spectrum(1.0));
	Spectrum sc = paramSet.FindOneSpectrum("scale", Spectrum(1.0));
	Point3f from = paramSet.FindOnePoint3f("from", Point3f(0, 0, 0));
	Point3f to = paramSet.FindOnePoint3f("to", Point3f(0, 0, 1));
	Vector3f dir = from - to;
	return std::make_shared<DistantLight>(light2world, L * sc, dir);
}
