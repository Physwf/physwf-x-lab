#include "Geometry.h"

Shape::Shape(const Transform* InLocalToWorld, const Transform* InWorldToLocal)
	: LocalToWorld(InLocalToWorld),WorldToLocal(InWorldToLocal)
{
}

Sphere::Sphere(const Transform* InLocalToWorld, const Transform* InWorldToLocal, float InRadius)
	: Shape(InLocalToWorld,InWorldToLocal), Radius(InRadius)
{
}

bool Sphere::Intersect(const Ray& ray, float* tHit, SurfaceInteraction* isect) const
{

}

Disk::Disk(const Transform* InLocalToWorld, const Transform* InWorldToLocal, float InRadius)
	: Shape(InLocalToWorld, InWorldToLocal), Radius(InRadius)
{
}

bool Disk::Intersect(const Ray& ray, float* tHit, SurfaceInteraction* isect) const
{

}

GeometryObject::GeometryObject(const std::shared_ptr<Shape>& Inshape, const std::shared_ptr<Material>& Inmaterial)
	: shape(Inshape),
	  material(Inmaterial)
{
}

Bounds3f GeometryObject::WorldBound() const
{
	return shape->WorldBound();
}

bool GeometryObject::Intersect(const Ray& ray, SurfaceInteraction* isect) const
{
	float tHit;
	if (!shape->Intersect(ray, &tHit, isect)) return false;
	ray.tMax = tHit;
	isect->object = this;
	return true;
}

bool GeometryObject::IntersectP(const Ray& ray) const
{
	return shape->IntersectP(ray);
}

