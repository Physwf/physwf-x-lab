#include "scene.h"

bool Scene::Intersect(const Ray& ray, SurfaceInteraction* isect) const
{
	return aggregate->Intersect(ray, isect);
}

bool Scene::IntersectP(const Ray& ray) const
{
	return aggregate->IntersectP(ray);
}

bool Scene::IntersectTr(Ray ray, Sampler& sampler, SurfaceInteraction* isect, Spectrum* Tr) const
{
	*Tr = Spectrum(1.0f);
	while (true)
	{
		bool hitSurface = Intersect(ray, isect);
		if (ray.medium) *Tr *= ray.medium->Tr(ray, sampler);
		if (!hitSurface) return false;
		if (isect->primitive->GetMaterial() != nullptr) return true;
		ray = isect->SpawnRay(ray.d);
	}
}
