#pragma once

#include "pbrt.h"
#include "light.h"
#include "primitive.h"

class DiffuseAreaLight : public AreaLight
{
public:
	DiffuseAreaLight(const Transform& LightToWorld,
		const MediumInterface& mediumInterface,
		const Spectrum& Le,
		int nSamples,
		const std::shared_ptr<Shape> &shape,
		bool twoSide = false);

	Spectrum L(const Interaction& intr, const Vector3f &w) const
	{
		return Spectrum();
	}

	virtual Spectrum Power() const override;
	virtual Spectrum Sample_Li(const Interaction& ref, const Point2f& u, Vector3f *wi, Float* pdf, VisibilityTester *vis) const override;
	virtual Float Pdf_Li(const Interaction&, const Vector3f&) const override;
	virtual Spectrum Sample_Le(const Point2f &u1, const Point2f& u2, Float time, Ray* ray, Normal3f *nLight, Float *pdfPos, Float *pdfDir) const override;
	virtual void Pdf_Le(const Ray &ray, const Normal3f &nLight, Float *pdfPos, Float *pdfDir) const override;
protected:
	const Spectrum Lemit;
	std::shared_ptr<Shape> shape;

	const bool twoSided;
	const Float area;
};

std::shared_ptr<AreaLight> CreateDiffuseAreaLight(const Transform& light2world, const Medium* medium, const ParamSet &paramSet, const std::shared_ptr<Shape> &shape);