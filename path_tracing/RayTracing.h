#pragma once

#include "Vector.h"
#include "Memory.h"
#include "Colorimetry.h"

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
	bool IsSurfaceInteraction() const { return n != Vector3f(); }
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

	void SetShadingGeometry(const Vector3f& dpdu, const Vector3f& dpdv, const Vector3f& dndu, const Vector3f& dndv, bool orientaionIsAuthoritative);
	void ComputeScatteringFunctions(const Ray& ray, MemoryArena& arena);
	LinearColor Le(const Vector3f& w);

	Vector2f uv;
	Vector3f dpdu, dpdv;
	Vector3f dndu, dndv;
	const class Shape* shape = NULL;
	struct
	{
		Vector3f n;
		Vector3f dpdu, dpdv;
		Vector3f dndu, dndv;
	} shading;
	const class SceneObject* object = NULL;
	class BSDF* bsdf;
};

