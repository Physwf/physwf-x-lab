#pragma once

#include "Geometry.h"

class Light;

class Scene
{
public:
	Scene(const std::vector<std::shared_ptr<SceneObject>>& Inobjects,const std::vector<std::shared_ptr<Light>> lights) 
		: objects(Inobjects)
		, lights(lights)
	{
		for (auto& obj : objects)
		{
			worldBounds = Union(worldBounds, obj->WorldBound());
		}

		std::vector<int> Indices(objects.size());
		for (int i = 0; i < (int)objects.size(); ++i)
		{
			Indices[i] = i;
		}

		std::vector<Vector3f> objectCenters;
		std::vector<Bounds3f> AllWorldBounds;
		Bounds3f SceneWorldBounds;
		Vector3f SquareDiff;
		for (int i : Indices)
		{
			Bounds3f WorldBounds = objects[i]->WorldBound();
			AllWorldBounds.push_back(WorldBounds);
			SceneWorldBounds = Union(SceneWorldBounds, WorldBounds);
			Vector3f TriangleCenter = (WorldBounds.pMax + WorldBounds.pMin) / 2.f;
			objectCenters.push_back(TriangleCenter);
		}
		Root = BuildKDTree<SceneObject>(objects, objectCenters, SceneWorldBounds, AllWorldBounds, Indices);
	}

	Bounds3f WorldBound() const
	{
		return worldBounds;
	}

	bool Intersect(const Ray& ray, SurfaceInteraction* isect) const;
	bool IntersectP(const Ray& ray) const;

	std::vector<std::shared_ptr<Light>> lights;
	std::vector<std::shared_ptr<Light>> infiniteLights;
protected:
	std::vector<std::shared_ptr<SceneObject>> objects;
private:
	Bounds3f worldBounds;
	KDNode<SceneObject> *Root;
};