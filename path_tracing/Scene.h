#pragma once

#include "Geometry.h"
#include "Sampler.h"

class Light;

class Scene
{
public:
	Scene(const std::vector<std::shared_ptr<SceneObject>>& Inobjects,const std::vector<std::shared_ptr<Light>> lights);

	Bounds3f WorldBound() const
	{
		return worldBounds;
	}

	bool Intersect(const Ray& ray, SurfaceInteraction* isect) const;
	bool IntersectP(const Ray& ray) const;

	bool IntersectTr(Ray ray, Sampler& sampler, SurfaceInteraction* interaction, LinearColor* Tr) const;

	std::vector<std::shared_ptr<Light>> lights;
	std::vector<std::shared_ptr<Light>> infiniteLights;
protected:
	std::vector<std::shared_ptr<SceneObject>> objects;
private:
	Bounds3f worldBounds;
	KDNode<SceneObject> *Root;
};