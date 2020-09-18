#include "diffuse.h"
#include "paramset.h"
#include "sampling.h"
//#include "shapes/triangle.h"

DiffuseAreaLight::DiffuseAreaLight(const Transform& LightToWorld, 
	const MediumInterface& mediumInterface, 
	const Spectrum& Le, 
	int nSamples, 
	const std::shared_ptr<Shape> &shape, 
	bool twoSide /*= false*/)
	:AreaLight(LightToWorld,mediumInterface,nSamples),
	Lemit(Le),
	shape(shape),
	twoSided(twoSide),
	area(shape->Area())
{

}


Spectrum DiffuseAreaLight::Power() const
{
	return (twoSided ? 2 : 1) * Lemit * area * Pi;
}

Spectrum DiffuseAreaLight::Sample_Li(const Interaction& ref,const Point2f& u, Vector3f *wi, Float* pdf, VisibilityTester *vis) const
{
	Interaction pShape = shape->Sample(ref, u, pdf);
	pShape.mediumInterface = mediumInterface;
	*wi = Normalize(pShape.p - ref.p);
	*pdf = shape->Pdf(ref, *wi);
	*vis = VisibilityTester(ref, pShape);
	return L(pShape, -*wi);
}

Float DiffuseAreaLight::Pdf_Li(const Interaction& ref, const Vector3f& wi) const
{
	return shape->Pdf(ref, wi);
}

Spectrum DiffuseAreaLight::Sample_Le(const Point2f &u1, const Point2f& u2, Float time, Ray* ray, Normal3f *nLight, Float *pdfPos, Float *pdfDir) const
{
	Interaction pShape = shape->Sample(u1);
	pShape.mediumInterface = mediumInterface;
	*pdfPos = shape->Pdf(pShape);
	*nLight = pShape.n;

	Vector3f w = CosineSampleHemisphere(u2);
	*pdfDir = CosineSampleHemisphere(w.z);
	Vector3f v1, v2, n(pShape.n);
	CoordinateSystem(n, &v1, &v2);
	w = w.x * v1 + w.y * v2 + w.z * n;
	*ray = pShape.SpawnRay(w);
	return L(pShape,w);
}

void DiffuseAreaLight::Pdf_Le(const Ray &ray, const Normal3f &nLight, Float *pdfPos, Float *pdfDir) const
{
	Interaction it(ray.o, n, Vector3f(), Vector3f(n), ray.time, mediumInterface);
	*pdfPos = shape->Pdf(it);
	*pdfDir = twoSided ? (0.5 * CosineHemispherePdf(AbsDot(n, ray.d))) : CosineHemispherePdf(Dot(n, ray.d));
}

std::shared_ptr<AreaLight> CreateDiffuseAreaLight(const Transform& light2world, const Medium* medium, const ParamSet &paramSet, const std::shared_ptr<Shape> &shape)
{
		Spectrum L = paramSet.FindOneSpectrum("L", Spectrum(1.0));
		Spectrum sc = paramSet.FindOneSpectrum("scale", Spectrum(1.0));
		int nSamples = paramSet.FindOneInt("samples",
			paramSet.FindOneInt("nsamples", 1));
		bool twoSided = paramSet.FindOneBool("twosided", false);
		if (PbrtOptions.quickRender) nSamples = std::max(1, nSamples / 4);
		return std::make_shared<DiffuseAreaLight>(light2world, medium, L * sc, nSamples, shape, twoSided);
}
