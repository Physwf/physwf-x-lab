#pragma once

#include <memory>
#include <string>

#include "Colorimetry.h"
#include "Transform.h"
#include "Geometry.h"

class Scene;
class Distribution1D;

class VisibilityTester
{
public:
	VisibilityTester() {}

	VisibilityTester(const Interaction& p0, const Interaction& p1)
		: p0(p0),p1(p1) {}
	const Interaction& P0() const { return p0; }
	const Interaction& P1() const { return p1; }
	bool Unoccluded(const Scene& scene) const;
private:
	Interaction p0, p1;
};

enum class LightFlags : int
{
	DeltaPosition = 1,
	DeltaDirection = 2,
	Area = 4,
	Infinite = 8
};

inline bool IsDeltaLight(int flags)
{
	return flags & (int)LightFlags::DeltaPosition || flags & (int)LightFlags::DeltaDirection;
}

class Light
{
public:
	virtual ~Light() {}

	Light(int flags, const Transform& LightToWorld, int nSamples = 1);
	virtual LinearColor Sample_Li(const Interaction& ref,const Vector2f& u, Vector3f* wi, float* pdf, VisibilityTester* vis) const =0;
	virtual LinearColor Power() const = 0;
	virtual void Preprocess(const Scene& scene) {}
	virtual LinearColor Le(const Ray& r) const;
	virtual float Pdf_Li(const Interaction& ref, const Vector3f& wi) const = 0;
	virtual LinearColor Sample_Le(const Vector2f& u1, const Vector2f& u2, Ray* ray, Vector3f* nLight, float* pdfPos, float* pdfDir) const = 0;
	virtual void Pdf_Le(const Ray& ray, const Vector3f& nLight, float* pdfPos, float* pdfDir) const = 0;
	
	const int flags;
	const int nSamples;
protected:
	const Transform LightToWorld, WorldToLight;
};

class AreaLight : public Light
{
public:
	AreaLight(const Transform& LightToWorld, int nSamples);

	virtual LinearColor L(const Interaction& intr, const Vector3f& w) const = 0;
};

class PointLight : public Light
{
public:
	PointLight(const Transform& LightToWorld, const LinearColor& I)
		:Light((int)LightFlags::DeltaPosition,LightToWorld),
		pLight(LightToWorld.Point(Vector3f(0,0,0))),
		I(I)
	{}

	virtual LinearColor Sample_Li(const Interaction& ref, const Vector2f& u, Vector3f* wi, float* pdf, VisibilityTester* vis) const;
	virtual LinearColor Power() const;
	virtual float Pdf_Li(const Interaction& ref, const Vector3f& wi) const;
	virtual LinearColor Sample_Le(const Vector2f& u1, const Vector2f& u2, Ray* ray, Vector3f* nLight, float* pdfPos, float* pdfDir) const;
	virtual void Pdf_Le(const Ray& ray, const Vector3f& nLight, float* pdfPos, float* pdfDir) const;
private:
	const Vector3f pLight;
	const LinearColor I;

};

class DisffuseAreaLight : public AreaLight
{
public:
	DisffuseAreaLight(const Transform& InLightToWorld, const LinearColor& emit,const std::shared_ptr<Shape>& shape, int nSamples, bool bTwoSide)
		: AreaLight(InLightToWorld, nSamples)
		, Lemit(emit)
		, shape(shape)
		, bTwoSide(bTwoSide)
		, area(shape->Area())
	{
		shape->SetTransform(&LightToWorld, &WorldToLight);
	}

	virtual LinearColor L(const Interaction& intr, const Vector3f& w) const override;
	virtual LinearColor Power() const;

	virtual LinearColor Sample_Li(const Interaction& ref, const Vector2f& u, Vector3f* wi, float* pdf, VisibilityTester* vis) const override;
	virtual float Pdf_Li(const Interaction& ref, const Vector3f& wi) const override;
	virtual LinearColor Sample_Le(const Vector2f& u1, const Vector2f& u2, Ray* ray, Vector3f* nLight, float* pdfPos, float* pdfDir) const override;
	virtual void Pdf_Le(const Ray& ray, const Vector3f& nLight, float* pdfPos, float* pdfDir) const override;

private:
	LinearColor Lemit;
	const std::shared_ptr<Shape> shape;
	bool bTwoSide;
	const float area;
};

class LightDistribution
{
public: 
	virtual ~LightDistribution() {}

	virtual const Distribution1D* Lookup(const Vector3f& p) const = 0;
};

class UniformLightDistribution : public LightDistribution
{
public:
	UniformLightDistribution(const Scene& scene);
	virtual const Distribution1D* Lookup(const Vector3f& p) const;
private:
	std::unique_ptr<Distribution1D> distrib;
};

std::unique_ptr<LightDistribution> CreateLightDistribution(const std::string& name, const Scene& scene);
