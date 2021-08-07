#pragma once

#include <memory>
#include <vector>

#include "Vector.h"
#include "RayTracing.h"
#include "Material.h"
#include "Bounds.h"
#include "Transform.h"

class AreaLight;

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
	virtual float Area() const = 0;
	virtual Interaction Sample(const Vector2f& u, float* pdf) const = 0;
	virtual float Pdf(const Interaction&) const { return 1.f / Area(); }
	virtual Interaction Sample(const Interaction& ref, const Vector2f& u, float* pdf) const;
	virtual float Pdf(const Interaction& ref, const Vector3f& wi) const;
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
	virtual float Area() const { return 4.f * PI * Radius * Radius; }
	virtual Interaction Sample(const Vector2f& u, float* pdf) const;
	virtual Interaction Sample(const Interaction& ref, const Vector2f& u, float* pdf) const;
	float Pdf(const Interaction& ref, const Vector3f& wi) const;
private:
	float Radius;
};

class Disk : public Shape
{
public:
	Disk(float InRadius);
	virtual Bounds3f ObjectBound() const;
	virtual bool Intersect(const Ray& ray, float* tHit, SurfaceInteraction* isect) const;
	virtual bool IntersectP(const Ray& ray) const;
	virtual float Area() const;
	virtual Interaction Sample(const Vector2f& u, float* pdf) const;
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
	virtual float Area() const;
	virtual Interaction Sample(const Vector2f& u, float* pdf) const;
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

template<typename T>
class KDNode
{
public:
	KDNode(const std::vector<std::shared_ptr<T>>& InAllElements, Bounds3f Bounds)
		: AllElements(InAllElements)
		, WorldBounds(Bounds)
		, Left(NULL)
		, Right(NULL)
		, IsLeafNode(false)
	{}

	bool Intersect(const Ray& ray, float* tHit, SurfaceInteraction* isect) const;
	bool Intersect(const Ray& ray, SurfaceInteraction* isect) const;
	bool IntersectP(const Ray& ray) const;

	const std::vector<std::shared_ptr<T>>& AllElements;
	std::vector<int> Indices;
	Bounds3f WorldBounds;
	KDNode* Left;
	KDNode* Right;
	bool IsLeafNode;
};

template<typename T>
bool KDNode<T>::Intersect(const Ray& ray, float* tHit, SurfaceInteraction* isect) const
{
	if (!WorldBounds.IntersectP(ray)) return false;

	if (IsLeafNode)
	{
		bool bIntersect = false;
		for (int i : Indices)
		{
			float t;
			SurfaceInteraction insection;
			if (AllElements[i]->Intersect(ray,&t,&insection))
			{
				if (t < ray.tMax)
				{
					ray.tMax = t;
					*isect = insection;
					bIntersect = true;
				}
			}
		}
		return bIntersect;
	}
	else
	{
		assert(Left != nullptr);
		assert(Right != nullptr);
		float lt0, lt1;
		float rt0, rt1;
		bool bLeft = Left->WorldBounds.IntersectP(ray, &lt0, &lt1);
		bool bRight = Right->WorldBounds.IntersectP(ray, &rt0, &rt1);
		if (bLeft && bRight)
		{
			KDNode* Next = NULL;
			KDNode* Last = NULL;
			if (lt0 < rt0)
			{
				Next = Left;
				Last = Right;
			}
			else
			{
				Next = Right;
				Last = Left;
			}
			return Last->Intersect(ray, tHit, isect);
		}
		else if (bLeft)
		{
			return Left->Intersect(ray, tHit, isect);
		}
		else if (bRight)
		{
			return Right->Intersect(ray, tHit, isect);
		}
		else
		{
			assert(false);
			return false;
		}
	}

}

template<typename T>
bool KDNode<T>::Intersect(const Ray& ray, SurfaceInteraction* isect) const
{
	if (!WorldBounds.IntersectP(ray)) return false;

	if (IsLeafNode)
	{
		bool bIntersect = false;
		for (int i : Indices)
		{
			if (AllElements[i]->Intersect(ray, isect))
			{
				bIntersect = true;
			}
		}
		return bIntersect;
	}
	else
	{
		assert(Left != nullptr);
		assert(Right != nullptr);
		float lt0, lt1;
		float rt0, rt1;
		bool bLeft = Left->WorldBounds.IntersectP(ray, &lt0, &lt1);
		bool bRight = Right->WorldBounds.IntersectP(ray, &rt0, &rt1);
		if (bLeft && bRight)
		{
			KDNode* Next = NULL;
			KDNode* Last = NULL;
			if (lt0 < rt0)
			{
				Next = Left;
				Last = Right;
			}
			else
			{
				Next = Right;
				Last = Left;
			}
			return Last->Intersect(ray, isect);
		}
		else if (bLeft)
		{
			return Left->Intersect(ray, isect);
		}
		else if (bRight)
		{
			return Right->Intersect(ray, isect);
		}
		else
		{
			assert(false);
			return false;
		}
	}
}

template<typename T>
bool KDNode<T>::IntersectP(const Ray& ray) const
{
	if (!WorldBounds.IntersectP(ray,NULL,NULL)) return false;

	if (IsLeafNode)
	{
		for (int i : Indices)
		{
			if (AllElements[i]->IntersectP(ray))
			{
				return true;
			}
		}
		return false;
	}
	else
	{
		assert(Left != nullptr);
		assert(Right != nullptr);
		float lt0, lt1;
		float rt0, rt1;
		bool bLeft = Left->WorldBounds.IntersectP(ray, &lt0, &lt1);
		bool bRight = Right->WorldBounds.IntersectP(ray, &rt0, &rt1);
		if (bLeft && bRight)
		{
			KDNode* Next = NULL;
			KDNode* Last = NULL;
			if (lt0 < rt0)
			{
				Next = Left;
				Last = Right;
			}
			else
			{
				Next = Right;
				Last = Left;
			}
			return Last->IntersectP(ray);
		}
		else if (bLeft)
		{
			return Left->IntersectP(ray);
		}
		else if (bRight)
		{
			return Right->IntersectP(ray);
		}
		else
		{
			assert(false);
			return false;
		}
	}
}

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

	KDNode<Triangle>* Root;
};

template<typename T>
KDNode<T>* BuildKDTree(const std::vector<std::shared_ptr<T>>& AllElements, const std::vector<Vector3f>& elementsCenters, std::vector<int> Indices)
{
	Bounds3f WorldBounds;
	int i = 0;
	for (int i : Indices)
	{
		WorldBounds = Union(WorldBounds, AllElements[i]->WorldBound());
	}

	KDNode<T>* Node = new KDNode<T>(AllElements, WorldBounds);

	if (AllElements.size() < 4ull)
	{
		Node->IsLeafNode = true;
		Node->Indices = Indices;
		return Node;
	}

	Vector3f WorldCenter = (WorldBounds.pMax + WorldBounds.pMin) / 2.f;

	Vector3f SquareDiff;
	for (int i : Indices)
	{
		Vector3f TriangleCenter = elementsCenters[i];
		Vector3f Diff = TriangleCenter - WorldCenter;
		SquareDiff += (Diff * Diff);
	}
	int SplitAxis = SquareDiff.MaxComponentIndex();

	std::sort(Indices.begin(), Indices.end(), [=](int A, int B)
		{
			if (SplitAxis == 0)
				return elementsCenters[A].X > elementsCenters[B].X;
			else if (SplitAxis == 1)
				return elementsCenters[A].Y > elementsCenters[B].Y;
			else /*if (SplitAxis == 2)*/
				return elementsCenters[A].Z > elementsCenters[B].Z;
		});

	int MiddleIndex = Indices.size() / 2;

	Node->Left = BuildKDTree<T>(AllElements, elementsCenters, std::vector<int>(Indices.begin(), Indices.begin() + MiddleIndex));
	Node->Right = BuildKDTree<T>(AllElements, elementsCenters, std::vector<int>(Indices.begin() + MiddleIndex, Indices.end()));

	return Node;
}

