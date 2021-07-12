#include "SamplerIntegrator.h"
#include "ParallelFor.h"
#include "SurfaceScattering.h"

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

LinearColor EstimateDirect(const Interaction& it, const Vector2f& uShading, const Light& light, const Vector2f& ulight, const Scene& scene, Sampler& sampler, MemoryArena& arena, bool specular /*= false*/)
{
	BxDFType bsdfFlags = specular ? BSDF_ALL : BxDFType(BSDF_ALL & ~BSDF_SPECULAR);
	LinearColor Ld(0.f);

	Vector3f wi;
	float LightPdf = 0, scatteringPdf = 0;
	VisibilityTester vt;
	LinearColor Li = light.Sample_Li(it, ulight, &wi, &LightPdf, &vt);
	if (LightPdf > 0 && !Li.IsBlack())
	{
		LinearColor f;
		if (it.IsSurfaceInteraction())
		{
			const SurfaceInteraction& isect = (const SurfaceInteraction&)it;
			f = isect.bsdf->f(isect.wo, wi, bsdfFlags) * AbsDot(wi, isect.shading.n);
			scatteringPdf = isect.bsdf->Pdf(isect.wo, wi, bsdfFlags);
		}
		else
		{
			//media
		}
		if (!f.IsBlack())
		{
			if (/*hanleMedia*/false)
			{

			}
			else
			{
				if (!vt.Unoccluded(scene))
				{
					Li = LinearColor(0.f);
				}
				else
				{

				}
			}

			if (!Li.IsBlack())
			{
				if (IsDeltaLight(light.flags))
				{
					Ld += f * Li / LightPdf;
				}
				else
				{

				}
			}
		}
	}

	return Ld;
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
		MemoryArena arena;

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
					if (rayWeight > 0) L = Li(r, scene, *tileSampler, arena);

					filmTile->AddSample(pixelSample, L, rayWeight);

				} while (tileSampler->StartNextSample());
			}
		}

	}, nTiles);
}

