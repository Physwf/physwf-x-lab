#include "sppm.h"
#include "parallel.h"
#include "scene.h"
#include "imageio.h"
#include "spectrum.h"
#include "rng.h"
#include "paramset.h"
#include "interaction.h"
#include "sampling.h"
#include "samplers/halton.h"

struct SPPMPixel
{
	Float radius = 0;
	Spectrum Ld;

	struct VisiblePoint
	{

		Point3f p;
		Vector3f wo;
		const BSDF* bsdf = nullptr;
		Spectrum beta;
	} vp;
};

struct SPPMPixelListNode
{
	SPPMPixel* pixel;
	SPPMPixelListNode* next;
};

void SPPMIntegrator::Render(const Scene& scene)
{
	Bounds2i pixelBounds = camera->film->croppedPixelBounds;
	int nPixels = pixelBounds.Area();
	std::unique_ptr<SPPMPixel[]> pixels(new SPPMPixel[nPixels]);
	for (int i = 0; i < nPixels; ++i)
		pixels[i].radius = initialSearchRadius;

	std::unique_ptr<Distribution1D> lightDistr = ComputeLightPowerDistribution(scene);

	HaltonSampler sampler(nIterations, pixelBounds);

	Vector2i pixelExtent = pixelBounds.Diagonal();
	const int tileSize = 16;
	Point2i nTiles((pixelExtent.x + tileSize - 1) / tileSize,
					(pixelExtent.y + tileSize - 1) / tileSize);

	std::vector<MemoryArena> perThreadArenas(MaxThreadIndex());

	for (int iter = 0; iter < nIterations; ++iter)
	{
		ParallelFor2D([&](Point2i tile) {
			MemoryArena& arena = perThreadArenas[ThreadIndex];
			int tileIndex = tile.y * nTiles.x + tile.x;
			std::unique_ptr<Sampler> tileSampler = sampler.Clone(tileIndex);

			int x0 = pixelBounds.pMin.x + tile.x * tileSize;
			int x1 = std::min(x0 + tileSize, pixelBounds.pMax.x);
			int y0 = pixelBounds.pMin.y + tile.y * tileSize;
			int y1 = std::min(y0 + tileSize, pixelBounds.pMax.y);
			Bounds2i tileBounds(Point2i(x0, y0), Point2i(x1, y1));

			for (Point2i pPixel : tileBounds)
			{
				tileSampler->StartPixel(pPixel);
				tileSampler->SetSamplerNumber(iter);
				CameraSample cameraSample = tileSampler->GetCameraSample(pPixel);
				RayDifferential ray;
				Spectrum beta = camera->GenerateRayDifferential(cameraSample, &ray);

				Point2i pPixel0 = Point2i(pPixel - pixelBounds.pMin);
				int pixelOffset = pPixel0.x + pPixel0.y *(pixelBounds.pMax.x - pixelBounds.pMin.x);
				SPPMPixel& pixel = pixels[pixelOffset];
				bool specularBounce = false;
				for(int depth = 0; depth < maxDepth; ++depth)
				{
					SurfaceInteraction isect;
					if (!scene.Intersect(ray, &isect))
					{
						for (const auto& light : scene.lights)
							pixel.Ld += beta * light->Le(ray);
						break;
					}

					isect.ComputeScatteringFunctions(ray, arena, true);
					if (!isect.bsdf)
					{
						ray = isect.SpawnRay(ray.d);
						--depth;
						continue;
					}
					const BSDF& bsdf = *isect.bsdf;
					Vector3f wo = -ray.d;
					if (depth == 0 || specularBounce)
						pixel.Ld += beta * isect.Le(wo);
					pixel.Ld += beta * UniformSampleOneLight(isect, scene, arena, *tileSampler);

					bool isDiffuse = bsdf.NumComponents(BxDFType(BSDF_DIFFUSE | BSDF_REFLECTION | BSDF_TRANSMISSION)) > 0;
					bool isGlossy = bsdf.NumComponents(BxDFType(BSDF_GLOSSY | BSDF_REFLECTION | BSDF_TRANSMISSION)) > 0;

					if (isDiffuse || (isGlossy && depth == maxDepth - 1))
					{
						pixel.vp = { isect.p,wo,&bsdf,beta };
						break;
					}

					if (depth < maxDepth - 1)
					{
						Float pdf;
						Vector3f wi;
						BxDFType type;
						Spectrum f = bsdf.Sample_f(wo, &wi, tileSampler->Get2D(), &pdf, BSDF_ALL, &type);
						if (pdf == 0. || f.IsBlack()) break;
						specularBounce = (type & BSDF_SPECULAR) != 0;
						beta *= f * AbsDot(wi, isect.shading.n) / pdf;
						if (beta.y() < 0.25)
						{
							Float continueProb = std::min(Float(1), beta.y());
							if (tileSampler->Get1D() > continueProb) break;
							beta /= continueProb;
						}
						ray = (RayDifferential)isect.SpawnRay(wi);
					}
				}
			}
		}, nTiles);
	}

	int hashSize = nPixels;
	std::vector<std::atomic<SPPMPixelListNode*>> grid(hashSize);
}

Integrator* CreateSPPMIntegrator(const ParamSet& params, std::shared_ptr<const Camera> camera)
{
	int nIterations =
		params.FindOneInt("iterations",
			params.FindOneInt("numiterations", 64));
	int maxDepth = params.FindOneInt("maxdepth", 5);
	int photonsPerIter = params.FindOneInt("photonsperiteration", -1);
	int writeFreq = params.FindOneInt("imagewritefrequency", 1 << 31);
	Float radius = params.FindOneFloat("radius", 1.f);
	if (PbrtOptions.quickRender) nIterations = std::max(1, nIterations / 16);
	return new SPPMIntegrator(camera, nIterations, photonsPerIter, maxDepth,
		radius, writeFreq);
}
