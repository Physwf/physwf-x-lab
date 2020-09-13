#pragma once

#include "pbrt.h"
#include "geometry.h"
#include "primitive.h"
#include "light.h"

class Scene
{
public:
	Scene(std::shared_ptr<Primitive> aggregate, const std::vector<std::shared_ptr<Light>>& lights)
		:lights(lights), aggregate(aggregate)
	{
		worldBound = aggregate->WorldBound();
		for (const auto& light : lights)
		{
			light->Preprocess(*this);
			if (light->flags &(int)LightFlags::Infinite)
			{
				infiniteLights.push_back(light);
			}
		}
	}

	const Bounds3f& WorldBound() const { return worldBound; }
	bool Intersect(const Ray& ray, SurfaceInteraction* isect) const;
	bool IntersectP(const Ray& ray) const;
	bool IntersectTr(Ray ray, Sampler& sampler, SurfaceInteraction* isect, Spectrum* transmittance) const;

	std::vector<std::shared_ptr<Light>> lights;
	std::vector<std::shared_ptr<Light>> infiniteLights;
private:
	std::shared_ptr<Primitive> aggregate;
	Bounds3f worldBound;
};