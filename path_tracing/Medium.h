#pragma once

#include "Vector.h"
#include "Colorimetry.h"
#include "Sampler.h"
#include "Memory.h"

struct Ray;
struct MediumInteraction;

class Medium
{
public:
	virtual LinearColor Tr(const Ray& ray, Sampler& sampler) const =0;
	virtual LinearColor Sample(const Ray& ray, Sampler& sampler, MemoryArena& arena, MediumInteraction* mi) const = 0;
};

class MediumInterface
{
public:
	MediumInterface() : inside(nullptr), outside(nullptr) {}
	MediumInterface(const Medium* medium) : inside(medium), outside(medium) {}
	MediumInterface(const Medium* inside, const Medium* outside) : inside(inside), outside(outside) {}

	bool IsMediumTransition() const { return inside != outside; }

	const Medium* inside, *outside;
};

class PhaseFunction
{
public:
	virtual float p(const Vector3f& wo, const Vector3f& wi) const = 0;
	virtual float Sample_p(const Vector3f& wo, Vector3f* wi, const Vector2f& u) const = 0;
	inline float PhaseHG(float cosTheta, float g) const
	{
		float denom = 1 + g * g + 2 * g * cosTheta;
		return INV_4PI * (1 - g * g) / (denom * std::sqrt(denom));
	}
};

