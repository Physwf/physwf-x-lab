#pragma once

#include "pbrt.h"
#include "memory.h"
#include "interaction.h"

enum class LightFlags : int
{
	DeltaPosition = 1,
	DeltaDirection = 2,
	Area = 4,
	Infinite = 8,
};

inline bool IsDeltaLight(int flags)
{
	return flags & (int)LightFlags::DeltaPosition || flags & (int)LightFlags::DeltaDirection;
}

class Light
{
public:
	virtual ~Light();
	Light(int flags, const Transform& LightToWorld, const MediumInterface& mediumInterface, int nSamples = 1);
	//samples an incident direction at a point in the scene along which illumination from the light may be arriving
	virtual Spectrum Power() const = 0;
	virtual void Preprocess(const Scene& scene);
	virtual Spectrum Le(const RayDifferential& r) const;
	virtual Spectrum Sample_Li(const Interaction& ref, const Point2f& u, Vector3f *wi, Float* pdf, VisibilityTester* vis) const = 0;
	virtual Float Pdf_Li(const Interaction& ref, const Vector3f& wi) const = 0;
	//returns light-carrying ray leaving the light source
	virtual Spectrum Sample_Le(const Point2f &u1, const Point2f& u2, Float time, Ray* ray, Normal3f *nLight, Float *pdfPos, Float *pdfDir) const = 0;
	virtual void Pdf_Le(const Ray &ray, const Normal3f &nLight, Float *pdfPos, Float *pdfDir) const = 0;

	const int flags;
	const int nSamples;
	const MediumInterface mediumInterface;
protected:
	const Transform LightToWorld, WorldToLight;
};

class VisibilityTester
{
public:
	VisibilityTester() {}
	VisibilityTester(const Interaction &p0, const Interaction &p1)
		: p0(p0), p1(p1) {}
	const Interaction &P0() const { return p0; }
	const Interaction &P1() const { return p1; }
	bool Unoccluded(const Scene &scene) const;
	Spectrum Tr(const Scene &scene, Sampler &sampler) const;
private:
	Interaction p0, p1;
};

class AreaLight : public Light
{
public:
	// AreaLight Interface
	AreaLight(const Transform &LightToWorld, const MediumInterface &medium, int nSamples);
	virtual Spectrum L(const Interaction &intr, const Vector3f &w) const = 0;
};