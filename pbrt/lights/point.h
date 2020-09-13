#pragma once

#include "pbrt.h"
#include "light.h"
#include "shape.h"

class PointLight : public Light
{
public:
	PointLight(const Transform& LightToWorld,const MediumInterface& mediumInterface,const Spectrum& I)
		:Light((int)LightFlags::DeltaPosition,LightToWorld,mediumInterface),
		pLight(LightToWorld(Point3f(0,0,0))),
		I(I) {}
	virtual Spectrum Sample_Li(const Interaction& ref, const Point2f &u, Vector3f* wi, Float* pdf, VisibilityTester* vis) const override;
	virtual Spectrum Power() const;
	virtual Float Pdf_Li(const Interaction&, const Vector3f&) const;
	virtual Spectrum Sample_Le(const Point2f& u1, const Point2f& u2, Float time, Ray* ray, Normal3f* nLight, Float* pdfPos, Float *pdfDir) const override;
	virtual void Pdf_Le(const Ray&, const Normal3f&, Float* pdfPos, Float* pdfDir) const;

private:
	const Point3f pLight;
	const Spectrum I;
};

std::shared_ptr<PointLight> CreatePointLight(const Transform &light2World, const Medium *medium, const ParamSet &paramSet);
