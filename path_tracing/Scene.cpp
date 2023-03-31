#include "Scene.h"
#include "Light.h"
#include "Medium.h"

Scene::Scene(const std::vector<std::shared_ptr<SceneObject>>& Inobjects, const std::vector<std::shared_ptr<Light>> lights) : objects(Inobjects)
, lights(lights)
{
	for (auto& obj : objects)
	{
		worldBounds = Union(worldBounds, obj->WorldBound());
	}

	for (const auto& light : lights)
	{
		light->Preprocess(*this);
		if (light->flags & (int)LightFlags::Infinite)
			infiniteLights.push_back(light);
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

bool Scene::IntersectTr(Ray ray, Sampler& sampler, SurfaceInteraction* isect, LinearColor* Tr) const
{
	*Tr = LinearColor(1);
	while (true)
	{
		bool bHitSurface = Intersect(ray, isect);
		if (ray.medium)
			*Tr *= ray.medium->Tr(ray, sampler);
		if (!bHitSurface)
			return false;
		if (isect->object->GetMaterial() != nullptr)
			return true;
		ray = isect->SpawnRay(ray.d);
	}
}
