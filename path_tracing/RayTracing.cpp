#include "RayTracing.h"
#include "Geometry.h"
#include "Light.h"

SurfaceInteraction::SurfaceInteraction(const Vector3f& InP, const Vector2f& InUV, const Vector3f& InWO, const Vector3f& Indpdu, const Vector3f& Indpdv, const Vector3f& Indndu, const Vector3f& Indndv, const class Shape* sh)
	: Interaction(InP, Normalize(Cross(Indpdu, Indpdv)), InWO)
	, uv(InUV)
	, dpdu(Indpdu)
	, dpdv(Indpdv)
	, dndu(Indndu)
	, dndv(Indndv)
	, shape(sh)
{
	shading.n = n;
	shading.dpdu = dpdu;
	shading.dpdv = dpdv;
	shading.dndu = dndu;
	shading.dndv = dndv;
}

void SurfaceInteraction::SetShadingGeometry(const Vector3f& Indpdu, const Vector3f& Indpdv, const Vector3f& Indndu, const Vector3f& Indndv, bool orientaionIsAuthoritative)
{
	shading.n = Normalize(Cross(Indpdu, Indpdv));
	if (orientaionIsAuthoritative)
		n = Faceforward(n, shading.n);
	else
		shading.n = Faceforward(shading.n, n);

	shading.dpdu = Indpdu;
	shading.dpdv = Indpdv;
	shading.dndu = Indpdu;
	shading.dndv = Indndv;
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

