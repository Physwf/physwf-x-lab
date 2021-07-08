#include "SamplerIntegrator.h"
#include "ParallelFor.h"

LinearColor UniformSampleAllLights(const Interaction& it, const Scene& scene, MemoryArena& arena, Sampler& sampler, const std::vector<int>& nLightSamples)
{

}

LinearColor UniformSampleOneLight(const Interaction& it, const Scene& scene, MemoryArena& arena, Sampler& sampler, const Distribution1D* lightDistrib /*= nullptr*/)
{
	int nLights = int(scene.lights.size());
	if (nLights == 0) return LinearColor(0);

	int lightIndex;
	float lightPdf;
	if (lightDistrib)
	{
		lightIndex = lightDistrib->SampleDiscrete(sampler.Get1D(), &lightPdf);
		if (lightPdf == 0) return LinearColor(0);
	}
	else
	{
		lightIndex = std::min((int)(sampler.Get1D() * nLights), nLights - 1);
		lightPdf = 1.f / nLights;
	}

	const std::shared_ptr<Light>& light = scene.lights[lightIndex];
	Vector2f uLight = sampler.Get2D();
	Vector2f uScattering = sampler.Get2D();
	return EstimateDirect(it, uScattering, *light, uLight, scene, sampler, arena) / lightPdf;
}

void SamplerIntegrator::Render(const Scene& scene)
{
	Bounds2i imageBounds = camera->film->GetBounds();
	Vector2i imageExtents = imageBounds.Diagonal();

	const int tileSize = 16;
	Vector2i nTiles((imageExtents.X + tileSize - 1) / tileSize,
		(imageExtents.Y + tileSize - 1) / tileSize);


	ParallelFor2D([&](Vector2i tile) {

		int seed = tile.Y * nTiles.X + tile.X;
		std::unique_ptr<Sampler> tileSampler = sampler->Clone(seed);

		int x0 = imageBounds.pMin.X + tile.X * tileSize;
		int y0 = imageBounds.pMin.Y + tile.Y * tileSize;
		int x1 = std::min(x0 + tileSize, imageBounds.pMax.X);
		int y1 = std::min(y0 + tileSize, imageBounds.pMax.Y);
		Bounds2i tileBounds(Vector2i(x0, y0), Vector2i(x1, y1));

		std::unique_ptr<FilmTile> filmTile = camera->film->GetFilmTile(tileBounds);

		int StartX = tileBounds.pMin.X;
		int EndX = StartX + tileSize;
		int StartY = tileBounds.pMin.Y;
		int EndY = StartY + tileSize;
		for (int y = StartY; y < EndY; ++y)
		{
			for (int x = StartY; x < EndY; ++x)
			{
				do
				{
					Vector2f pixelSample = tileSampler->GetPixelSample(Vector2i(x, y));
					Ray r;
					float rayWeight = camera->GenerateRay(pixelSample, &r);

					LinearColor L(0.f);
					if (rayWeight > 0) L = Li(r, scene, *tileSampler);

					filmTile->AddSample(pixelSample, L, rayWeight);

				} while (tileSampler->StartNextSample());
			}
		}

	}, nTiles);
}

