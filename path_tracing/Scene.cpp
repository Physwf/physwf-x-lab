#include "Scene.h"

bool Scene::Intersect(const Ray& ray, SurfaceInteraction* isect) const
{
// 	Ray ray2 = ray;
// 	bool bHit = false;
// 	for (const auto& o : objects)
// 	{
// 		if (o->Intersect(ray, isect))
// 		{
// 			bHit = true;
// 			break;
// 		}
// 	}
// 	//return bHit;
// 	bool bHit2 = Root->Intersect(ray2, isect);
// 	assert(bHit == bHit2);
// 	assert(ray.tMax == ray2.tMax);
// 	return bHit;
	return Root->Intersect(ray, isect);
}

bool Scene::IntersectP(const Ray& ray) const
{
// 	bool bHit = false;
// 	for (const auto& o : objects)
// 	{
// 		if (o->IntersectP(ray))
// 		{
// 			bHit = true;
// 			break;
// 		}
// 	}
// 	return bHit;
	return Root->IntersectP(ray);
}
