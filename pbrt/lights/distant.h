#pragma once

#include "pbrt.h"
#include "light.h"
#include "shape.h"
#include "scene.h"

class DistantLight : public Light
{
public:
	DistantLight(const Transform& LightToWorld, const Spectrum& L, const Vector3f &w);

	void PreProcess(const Scene& scene)
	{
		scene.WorldBound().BoundingSphere(&worldCenter, &worldRadius);
	}

	virtual Spectrum Power() const override;
	virtual Spectrum Sample_Li(const Interaction& ref, const Point2f& u, Vector3f *wi, Float* pdf, VisibilityTester* vis) const override;
	virtual Float Pdf_Li(const Interaction& ref, const Vector3f& wi) const override;
	virtual Spectrum Sample_Le(const Point2f &u1, const Point2f& u2, Float time, Ray* ray, Normal3f *nLight, Float *pdfPos, Float *pdfDir) const override;
	virtual void Pdf_Le(const Ray &ray, const Normal3f &nLight, Float *pdfPos, Float *pdfDir) const override;
private:
	const Spectrum L;
	const Vector3f wLight;
	Point3f worldCenter;
	Float worldRadius;
};

std::shared_ptr<DistantLight> CreateDistantLight(const Transform& light2world,const ParamSet &paramSet);