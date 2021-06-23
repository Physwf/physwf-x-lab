#include "Scene.h"

Bounds3f Scene::WorldBound() const
{

}

bool Scene::Intersect(const Ray& ray, SurfaceInteraction* isect) const
{
	for (const auto& o : objects)
	{
		if (o->Intersect(ray, isect)) break;
	}
}

bool Scene::IntersectP(const Ray& ray) const
{
	for (const auto& o : objects)
	{
		if (o->IntersectP(ray)) break;
	}
}
