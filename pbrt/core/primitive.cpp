#include "primitive.h"
#include "light.h"
#include "interaction.h"

Primitive::~Primitive()
{

}

Bounds3f GeometricPrimitive::WorldBound() const
{
	return shape->WorldBound();
}

bool GeometricPrimitive::Intersect(const Ray& r, SurfaceInteraction* isect) const
{
	Float tHit;
	if (!shape->Intersect(r, &tHit, isect)) return false;
	r.tMax = tHit;
	isect->primitive = this;
	DOCHECK(Dot(isect->n, isect->shading.n) > 0.);
	// Initialize _SurfaceInteraction::mediumInterface_ after _Shape_
	// intersection
	if (mediumInterface.IsMediumTransition())
		isect->mediumInterface = mediumInterface;
	else
		isect->mediumInterface = MediumInterface(r.medium);
	return true;
}

bool GeometricPrimitive::IntersectP(const Ray& r) const
{
	return shape->IntersectP(r);
}

GeometricPrimitive::GeometricPrimitive(const std::shared_ptr<Shape>& shape, const std::shared_ptr<Material> &material, const std::shared_ptr<AreaLight>& arealight, const MediumInterface &mediumInterface)
	: shape(shape),
	material(material),
	areaLight(areaLight),
	mediumInterface(mediumInterface)
{

}

const AreaLight* GeometricPrimitive::GetAreaLight() const
{
	return areaLight.get();
}

const Material* GeometricPrimitive::GetMaterial() const
{
	return material.get();
}

void GeometricPrimitive::ComputeScatteringFunctions(SurfaceInteraction* isect, MemoryArena& arena, TransportMode mode, bool allowMultipleLobes) const
{
	if (material)
		material->ComputeScatteringFunctions(isect, arena, mode, allowMultipleLobes);
}

TransformedPrimitive::TransformedPrimitive(std::shared_ptr<Primitive>& primitive, const AnimatedTransform & PrimitiveToWorld)
	:primitive(primitive),PrimitiveToWorld(PrimitiveToWorld)
{
	
}

bool TransformedPrimitive::Intersect(const Ray& r, SurfaceInteraction* isect) const
{
	// Compute _ray_ after transformation by _PrimitiveToWorld_
	Transform InterpolatedPrimToWorld;
	PrimitiveToWorld.Interpolate(r.time, &InterpolatedPrimToWorld);
	Ray ray = Inverse(InterpolatedPrimToWorld)(r);
	if (!primitive->Intersect(ray, isect)) return false;
	r.tMax = ray.tMax;
	// Transform instance's intersection data to world space
	if (!InterpolatedPrimToWorld.IsIdentity())
		*isect = InterpolatedPrimToWorld(*isect);
	//CHECK_GE(Dot(isect->n, isect->shading.n), 0);
	return true;
}

bool TransformedPrimitive::IntersectP(const Ray& r) const
{
	Transform InterpolatedPrimToWorld;
	PrimitiveToWorld.Interpolate(r.time, &InterpolatedPrimToWorld);
	Transform InterpolatedWorldToPrim = Inverse(InterpolatedPrimToWorld);
	return primitive->IntersectP(InterpolatedWorldToPrim(r));
}

const AreaLight* TransformedPrimitive::GetAreaLight() const
{
	return nullptr;
}

const Material* TransformedPrimitive::GetMaterial() const
{
	return nullptr;
}

void TransformedPrimitive::ComputeScatteringFunctions(SurfaceInteraction* isect, MemoryArena& arena, TransportMode mode, bool allowMultipleLobes) const
{
	//LOG(FATAL) << "TransformedPrimitive::ComputeScatteringFunctions() shouldn't be " "called";
}

const AreaLight* Aggregate::GetAreaLight() const
{
// 	LOG(FATAL) <<
// 		"Aggregate::GetAreaLight() method"
// 		"called; should have gone to GeometricPrimitive";
	return nullptr;
}

const Material* Aggregate::GetMaterial() const
{
// 	LOG(FATAL) <<
// 		"Aggregate::GetMaterial() method"
// 		"called; should have gone to GeometricPrimitive";
	return nullptr;
}

void Aggregate::ComputeScatteringFunctions(SurfaceInteraction* isect, MemoryArena& arena, TransportMode mode, bool allowMultipleLobes) const
{
// 	LOG(FATAL) <<
// 		"Aggregate::ComputeScatteringFunctions() method"
// 		"called; should have gone to GeometricPrimitive";
}
