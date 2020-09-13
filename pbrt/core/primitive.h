#pragma once

#include "pbrt.h"
#include "shape.h"
#include "material.h"
#include "medium.h"
#include "transform.h"

class Primitive
{
public:
	virtual ~Primitive();
	virtual Bounds3f WorldBound() const = 0;
	virtual bool Intersect(const Ray& r, SurfaceInteraction*) const = 0;
	virtual bool IntersectP(const Ray& r) const = 0;
	virtual const AreaLight* GetAreaLight() const = 0;
	virtual const Material* GetMaterial() const = 0;
	virtual void ComputeScatteringFunctions(SurfaceInteraction* isect, MemoryArena& arena, TransportMode mode, bool allowMultipleLobes) const = 0;
};

class GeometricPrimitive : public Primitive
{
public:
	virtual Bounds3f WorldBound() const override;
	virtual bool Intersect(const Ray& r, SurfaceInteraction* isect) const override;
	virtual bool IntersectP(const Ray& r) const override;

	GeometricPrimitive(const std::shared_ptr<Shape>& shape,
		const std::shared_ptr<Material> &material,
		const std::shared_ptr<AreaLight>& arealight,
		const MediumInterface &mediumInterface);

	virtual const AreaLight* GetAreaLight() const override;
	virtual const Material* GetMaterial() const override;

	virtual void ComputeScatteringFunctions(SurfaceInteraction* isect, MemoryArena& arena, TransportMode mode, bool allowMultipleLobes) const;
private:
	std::shared_ptr<Shape> shape;
	std::shared_ptr<Material> material;
	std::shared_ptr<AreaLight> areaLight;
	MediumInterface mediumInterface;
};

class TransformedPrimitive : public Primitive
{
public:
	TransformedPrimitive(std::shared_ptr<Primitive>& primitive, const AnimatedTransform& PrimitiveToWorld);

	bool Intersect(const Ray& r, SurfaceInteraction* in) const;
	bool IntersectP(const Ray& r) const;
	const AreaLight* GetAreaLight() const;
	const Material* GetMaterial() const;

	virtual void ComputeScatteringFunctions(SurfaceInteraction* isect, MemoryArena& arena, TransportMode mode, bool allowMultipleLobes) const;

	Bounds3f WorldBound() const
	{
		return PrimitiveToWorld.MotionBounds(primitive->WorldBound());
	}

private:
	std::shared_ptr<Primitive> primitive;
	const AnimatedTransform PrimitiveToWorld;
};

class Aggregate : public Primitive
{
public:
	const AreaLight* GetAreaLight() const;
	const Material* GetMaterial() const;

	virtual void ComputeScatteringFunctions(SurfaceInteraction* isect, MemoryArena& arena, TransportMode mode, bool allowMultipleLobes) const;
};