#include "Scene.h"

bool Scene::Intersect(const Ray& ray, SurfaceInteraction* isect) const
{
	bool bHit = false;
	for (const auto& o : objects)
	{
		if (o->Intersect(ray, isect))
		{
			bHit = true;
			break;
		}
	}
	return bHit;
}

bool Scene::IntersectP(const Ray& ray) const
{
	bool bHit = false;
	for (const auto& o : objects)
	{
		if (o->IntersectP(ray))
		{
			bHit = true;
			break;
		}
	}
	return bHit;
}
