#include "RayTracing.h"
#include "Geometry.h"
#include "Light.h"

SurfaceInteraction::SurfaceInteraction(const Vector3f& InP, const Vector2f& InUV, const Vector3f& InWO, const Vector3f& Indpdu, const Vector3f& Indpdv, const Vector3f& Indndu, const Vector3f& Indndv, const class Shape* sh)
	: Interaction(InP, Normalize(Cross(Indpdu, Indpdv)), InWO), 
	uv(InUV), dpdu(Indpdu), dpdv(Indpdv), dndu(Indndu), dndv(Indndv),shape(sh)
{
}

void SurfaceInteraction::ComputeScatteringFunctions(const Ray& ray, MemoryArena& arena)
{
	object->ComputeScatteringFunctions(this, arena);
}

LinearColor SurfaceInteraction::Le(const Vector3f& w)
{
	const AreaLight* al = object->GetAreaLight();
	return al ? al->L(*this, w) : LinearColor(0);
}

