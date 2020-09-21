#pragma once


#include "pbrt.h"
#include "integrator.h"
#include "camera.h"
#include "film.h"

class SPPMIntegrator : public Integrator
{
public:
	SPPMIntegrator(std::shared_ptr<const Camera>& camera, int nIterations,
		int photonsPerIteration, int maxDepth, Float initialSearchRadius, int writeFrequency)
		:camera(camera),
		initialSearchRadius(initialSearchRadius),
		nIterations(nIterations),
		maxDepth(maxDepth),
		photonsPerIteration(photonsPerIteration > 0 ? photonsPerIteration : camera->film - croppedPixelBounds.Area()),
		writeFrequency(writeFrequency) {}

	void Render(const Scene& scene);

private:
	std::shared_ptr<const Camera> camera;
	const Float initialSearchRadius;
	const int nIterations;
	const int maxDepth;
	const int photonsPerIteration;
	const int writeFrequency;
};

Integrator* CreateSPPMIntegrator(const ParamSet& params, std::shared_ptr<const Camera> camera);