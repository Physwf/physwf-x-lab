#include "RayTracing.h"

SurfaceInteraction::SurfaceInteraction(const Vector3f& InP, const Vector2f& InUV, const Vector3f& InWO, const Vector3f& Indpdu, const Vector3f& Indpdv, const Vector3f& Indndu, const Vector3f& Indndv)
	: Interaction(InP, Normalize(Cross(Indpdu, Indpdv)), InWO), 
	uv(InUV), dpdu(Indpdu), dpdv(Indpdv), dndu(Indndu), dndv(Indndv)
{
}

