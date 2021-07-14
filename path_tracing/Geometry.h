#pragma once

#include <memory>
#include <vector>

#include "Vector.h"
#include "RayTracing.h"
#include "Material.h"
#include "Bounds.h"
#include "Transform.h"
#include "Light.h"

class Shape
{
public:
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
	void SetTransform(const Transform* InLocalToWorld, const Transform* InWorldToLocal)
	{
		LocalToWorld = InLocalToWorld;
		WorldToLocal = InWorldToLocal;
	}
protected:
	const Transform	*LocalToWorld, *WorldToLocal;

};

class Sphere : public Shape
{
public:
	Sphere(float InRadius);
	virtual Bounds3f ObjectBound() const;
	virtual bool Intersect(const Ray& ray, float* tHit, SurfaceInteraction* isect) const;
	virtual bool IntersectP(const Ray& ray) const override;
private:
	float Radius;
};

class Disk : public Shape
{
public:
	Disk(float InRadius);
	virtual bool Intersect(const Ray& ray, float* tHit, SurfaceInteraction* isect) const;
private:
	float Radius;
};

class Triangle : public Shape
{
public:
	Triangle(class MeshObject* InMesh, int triNumber);
	Bounds3f ObjectBound() const;
	Bounds3f WorldBound() const;
	virtual bool Intersect(const Ray& ray, float* tHit, SurfaceInteraction* isect) const;
	virtual bool IntersectP(const Ray& ray) const;
private:
	void GetUVs(Vector2f uv[3]) const;
	class MeshObject* mesh;
	const int* v;
};

class SceneObject
{
public:
	SceneObject(const Transform& InLocalToToWorld)
		:  LocalToWorld(InLocalToToWorld),WorldToLocal(Inverse(InLocalToToWorld))
	{}
	virtual ~SceneObject() {};
	virtual Bounds3f WorldBound() const = 0;
	virtual bool Intersect(const Ray& ray, SurfaceInteraction* isect) const = 0;
	virtual bool IntersectP(const Ray& ray) const = 0;
	virtual AreaLight* GetAreaLight() const = 0;
	virtual void ComputeScatteringFunctions(SurfaceInteraction* isect, MemoryArena& arena) const = 0;
protected:
	const Transform LocalToWorld;
	const Transform WorldToLocal;
};

class GeometryObject : public SceneObject
{
public:
	GeometryObject(const Transform& InLocalToToWorld, const std::shared_ptr<Material>& Inmaterial,const std::shared_ptr<Shape>& Inshape);
	virtual Bounds3f WorldBound() const;
	virtual bool Intersect(const Ray& ray, SurfaceInteraction* isect) const;
	virtual bool IntersectP(const Ray& ray) const;
	virtual void ComputeScatteringFunctions(SurfaceInteraction* isect, MemoryArena& arena) const;
	virtual AreaLight* GetAreaLight() const { return nullptr; }
private:
	std::shared_ptr<Shape> shape;
	std::shared_ptr<Material> material;
};

class MeshObject : public SceneObject
{
public:
	MeshObject(const Transform& InLocalToToWorld, const std::shared_ptr<Material>& Inmaterial,
		int InnTriangles, int InnVertices, Vector3f* Inp, Vector3f* Inn, Vector2f* Inuv, std::vector<int> InIndices);
	virtual Bounds3f WorldBound() const;
	virtual bool Intersect(const Ray& ray, SurfaceInteraction* isect) const;
	virtual bool IntersectP(const Ray& ray) const;
	virtual void ComputeScatteringFunctions(SurfaceInteraction* isect, MemoryArena& arena) const;
	virtual AreaLight* GetAreaLight() const { return nullptr; }
private:
	friend class Triangle;

	void BuildTriangle();

	std::shared_ptr<Material> material;

	const int nTriangles, nVertices;
	std::vector<std::shared_ptr<Triangle>> Triangles;
	std::vector<int> Indices;
	Vector3f* p;
	Vector3f* n;
	Vector2f* uv;
	Bounds3f LocalBounds;
};

