#pragma once

#include "Geometry.h"

class Scene
{
public:
	Scene(const std::vector<std::shared_ptr<SceneObject>>& Inobjects) : objects(Inobjects)
	{}
	Bounds3f WorldBound() const;
	bool Intersect(const Ray& ray, SurfaceInteraction* isect) const;
	bool IntersectP(const Ray& ray) const;
protected:
	std::vector<std::shared_ptr<SceneObject>> objects;
};