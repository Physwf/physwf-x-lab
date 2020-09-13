#include "whitted.h"
#include "interaction.h"
#include "camera.h"
#include "film.h"
#include "paramset.h"


Spectrum WhittedIntegrator::Li(const RayDifferential &ray, const Scene &scene, Sampler &sampler, MemoryArena &arena, int depth) const
{
	Spectrum L(0.);

	SurfaceInteraction isect;
	//如果射线与场景不相交，则寻找是否有同非几何体的光源相交
	if (!scene.Intersect(ray, &isect))
	{
		for (const auto& light : scene.lights)
		{
			L += light->Le(ray);
		}
	}

	Normal3f n = isect.shading.n;
	Vector3f wo = isect.wo;

	isect.ComputeScatteringFunctions(ray, arena);
	if (!isect.bsdf)
		return Li(isect.SpawnRay(ray.d), scene, sampler, arena, depth);

	// Compute emitted light if ray hit an area light source
	L += isect.Le(wo);

	for (const auto &light : scene.lights)
	{
		Vector3f wi;
		Float pdf;
		VisibilityTester visibitlity;
		Spectrum Li = light->Sample_Li(isect, sampler.Get2D(), &wi, &pdf, &visibitlity);
		if (Li.IsBlack() || pdf == 0) continue;
		Spectrum f = isect.bsdf->f(wo, wi);
		if (!f.IsBlack() && visibitlity.Unoccluded(scene))
			L += f * Li*AbsDot(wi, n) / pdf;

		if (depth + 1 < maxDepth)
		{
			L += SpecularReflect(ray, isect, scene, sampler, arena, depth);
			L += SpecularTransmit(ray, isect, scene, sampler, arena, depth);
		}
	}
	return L;
}


WhittedIntegrator *CreateWhittedIntegrator(
	const ParamSet &params, std::shared_ptr<Sampler> sampler,
	std::shared_ptr<const Camera> camera) {
	int maxDepth = params.FindOneInt("maxdepth", 5);
	int np;
	const int *pb = params.FindInt("pixelbounds", &np);
	Bounds2i pixelBounds = camera->film->GetSampleBounds();
	if (pb) {
		if (np != 4)
			Error("Expected four values for \"pixelbounds\" parameter. Got %d.",
				np);
		else {
			pixelBounds = Intersect(pixelBounds,
				Bounds2i{ { pb[0], pb[2] },{ pb[1], pb[3] } });
			if (pixelBounds.Area() == 0)
				Error("Degenerate \"pixelbounds\" specified.");
		}
	}
	return new WhittedIntegrator(maxDepth, camera, sampler, pixelBounds);
}
