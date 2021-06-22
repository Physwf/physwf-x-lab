#pragma once

#include <memory>

#include "Vector.h"
#include "RayTracing.h"
#include "Material.h"
#include "Bounds.h"
#include "Transform.h"

class Shape
{
public:
	Shape(const Transform* InLocalToWorld, const Transform* InWorldToLocal);
	virtual ~Shape() {};
	virtual Bounds3f ObjectBound() const = 0;
	virtual Bounds3f WorldBound() const
	{
		return (*LocalToWorld)(ObjectBound());
	}
	virtual bool Intersect(const Ray& ray, float* tHit, SurfaceInteraction* isect) const = 0;
	virtual bool IntersectP(const Ray& ray) const
	{
		return Intersect(ray, nullptr, nullptr);
	}
protected:
	const Transform	*LocalToWorld, *WorldToLocal;

};

class Sphere : public Shape
{
public:
	Sphere(const Transform* InLocalToWorld, const Transform* InWorldToLocal, float InRadius);
	virtual bool Intersect(const Ray& ray, float* tHit, SurfaceInteraction* isect) const;
private:
	float Radius;
};

class Disk : public Shape
{
public:
	Disk(const Transform* InLocalToWorld, const Transform* InWorldToLocal, float InRadius);
	virtual bool Intersect(const Ray& ray, float* tHit, SurfaceInteraction* isect) const;
private:
	float Radius;
};

class Triangle : public Shape
{

};

class SceneObject
{
public:
	virtual ~SceneObject() {};
	virtual Bounds3f WorldBound() const = 0;
	virtual bool Intersect(const Ray& ray, SurfaceInteraction* isect) const = 0;
	virtual bool IntersectP(const Ray& ray) const = 0;
};

class GeometryObject : public SceneObject
{
public:
	GeometryObject(const std::shared_ptr<Shape>& Inshape, const std::shared_ptr<Material>& Inmaterial);
	virtual Bounds3f WorldBound() const;
	virtual bool Intersect(const Ray& ray, SurfaceInteraction* isect) const;
	virtual bool IntersectP(const Ray& ray) const;
private:
	std::shared_ptr<Shape> shape;
	std::shared_ptr<Material> material;
	const Transform LocalToWorld;
};


