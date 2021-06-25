#pragma once

#include <memory>
#include <vector>

#include "Vector.h"
#include "RayTracing.h"
#include "Material.h"
#include "Bounds.h"
#include "Transform.h"

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
	virtual bool Intersect(const Ray& ray, float* tHit, SurfaceInteraction* isect) const;
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
	Triangle(class MeshObject* InMesh, int triNumber)
		: mesh(InMesh)
	{
		v = &mesh->Indices[3 * triNumber];
	}
	Bounds3f ObjectBound() const;
	Bounds3f WorldBound() const;
	virtual bool Intersect(const Ray& ray, float* tHit, SurfaceInteraction* isect) const;
	virtual bool IntersectP(const Ray& ray) const;
private:
	void GetUVs(Vector2f uv[3]) const
	{
		if (mesh->uv)
		{
			uv[0] = mesh->uv[v[0]];
			uv[1] = mesh->uv[v[1]];
			uv[2] = mesh->uv[v[2]];
		}
	}
	class MeshObject* mesh;
	const int* v;
};

class SceneObject
{
public:
	SceneObject(const Transform& InLocalToToWorld, const std::shared_ptr<Material>& Inmaterial)
		:  LocalToWorld(InLocalToToWorld),WorldToLocal(Inverse(InLocalToToWorld)), material(Inmaterial)
	{}
	virtual ~SceneObject() {};
	virtual Bounds3f WorldBound() const = 0;
	virtual bool Intersect(const Ray& ray, SurfaceInteraction* isect) const = 0;
	virtual bool IntersectP(const Ray& ray) const = 0;
protected:
	const Transform LocalToWorld;
	const Transform WorldToLocal;
	std::shared_ptr<Material> material;
};

class GeometryObject : public SceneObject
{
public:
	GeometryObject(const Transform& InLocalToToWorld, const std::shared_ptr<Material>& Inmaterial,const std::shared_ptr<Shape>& Inshape, const std::shared_ptr<Material>& Inmaterial);
	virtual Bounds3f WorldBound() const;
	virtual bool Intersect(const Ray& ray, SurfaceInteraction* isect) const;
	virtual bool IntersectP(const Ray& ray) const;
private:
	std::shared_ptr<Shape> shape;
};

class MeshObject : public SceneObject
{
public:
	MeshObject(const Transform& InLocalToToWorld, const std::shared_ptr<Material>& Inmaterial,
		int InnTriangles, int InnVertices, Vector3f* Inp, Vector3f* Inn, Vector2f* Inuv, std::vector<int> InIndices);
	virtual Bounds3f WorldBound() const;
	virtual bool Intersect(const Ray& ray, SurfaceInteraction* isect) const;
	virtual bool IntersectP(const Ray& ray) const;
private:
	friend class Triangle;

	void BuildTriangle();

	const int nTriangles, nVertices;
	std::vector<std::shared_ptr<Triangle>> Triangles;
	std::vector<int> Indices;
	std::unique_ptr<Vector3f[]> p;
	std::unique_ptr<Vector3f[]> n;
	std::unique_ptr<Vector2f[]> uv;
	Bounds3f LocalBounds;
};

