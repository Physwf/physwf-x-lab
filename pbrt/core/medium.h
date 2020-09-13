#pragma once

#include "pbrt.h"
#include "geometry.h"
#include "spectrum.h"
#include <memory>

class PhaseFunction
{
public:
	virtual ~PhaseFunction();
	virtual Float p(const Vector3f& wo, const Vector3f& wi) const = 0;
	virtual Float Sample_p(const Vector3f& wo, Vector3f* wi, const Point2f& u) const = 0;

};

class Medium
{
public:
	virtual ~Medium() {}
	virtual Spectrum Tr(const Ray& ray, Sampler& sampler) const = 0;
	virtual Spectrum Sample(const Ray& ray, Sampler& sampler, MemoryArena& arena, MediumInteraction* mi) const = 0;
};

struct MediumInterface
{
	MediumInterface() : inside(nullptr), outside(nullptr) {}
	// MediumInterface Public Methods
	MediumInterface(const Medium *medium) : inside(medium), outside(medium) {}
	MediumInterface(const Medium *inside, const Medium *outside)
		: inside(inside), outside(outside) {}
	bool IsMediumTransition() const { return inside != outside; }
	const Medium* inside, *outside;
};