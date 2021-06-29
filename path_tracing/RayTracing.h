#pragma once

#include "Vector.h"
#include "SurfaceScattering.h"

struct Ray
{
	Ray() : tMax(std::numeric_limits<float>::infinity()) {}
	Ray(const Vector3f& InO, const Vector3f& InD, float tMax = std::numeric_limits<float>::infinity())
	: o(InO), d(InD), tMax(tMax)  
	{}
	Vector3f operator() (float t) const { return o + (d * t); }

	Vector3f o;
	Vector3f d;
	mutable float tMax;
};

struct Interaction
{
	Interaction() {}
	Interaction(const Vector3f& InP, const Vector3f& InN, const Vector3f& InWO) :p(InP),n(InN),wo(InWO) {}
	Interaction(const Vector3f& InP) :p(InP) {}

	Ray SpawnRay(const Vector3f& d) const
	{

	}
	Vector3f p;
	Vector3f n;
	Vector3f wo;
};

struct SurfaceInteraction : public Interaction
{
	SurfaceInteraction() {}
	SurfaceInteraction(
		const Vector3f& InP, const Vector2f& InUV, const Vector3f& InWO, 
		const Vector3f &Indpdu, const Vector3f &Indpdv, 
		const Vector3f &Indndu, const Vector3f &Indndv, const class Shape* sh);

	void ComputeScatteringFunctions();
	LinearColor Le(const Vector3f& w);

	Vector2f uv;
	Vector3f dpdu, dpdv;
	Vector3f dndu, dndv;
	BSDF* bsdf;
	const class SceneObject* object = NULL;
	const class Shape* shape = NULL;
};

