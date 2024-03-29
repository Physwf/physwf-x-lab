#include "SamplerIntegrator.h"
#include "ParallelFor.h"
#include "SurfaceScattering.h"
#include "Light.h"


LinearColor UniformSampleAllLights(const Interaction& it, const Scene& scene, MemoryArena& arena, Sampler& sampler, const std::vector<int>& nLightSamples)
{
	return LinearColor(0);
}

LinearColor UniformSampleOneLight(const Interaction& it, const Scene& scene, MemoryArena& arena, Sampler& sampler, const Distribution1D* lightDistrib /*= nullptr*/, bool handleMedia /*= false*/)
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
	return EstimateDirect(it, uScattering, *light, uLight, scene, sampler, arena, handleMedia) / lightPdf;
}

LinearColor EstimateDirect(const Interaction& it, const Vector2f& uShading, const Light& light, const Vector2f& ulight, const Scene& scene, Sampler& sampler, MemoryArena& arena, bool handleMedia /*= false*/, bool specular /*= false*/)
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
			const MediumInteraction& mi = (const MediumInteraction&)it;
			float p = mi.phase->p(mi.wo, wi);
			f = LinearColor(p);
			scatteringPdf = p;
		}
		if (!f.IsBlack())
		{
			if (handleMedia)
			{
				Li *= vt.Tr(scene,sampler);
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
					float weight = PowerHeuristic(1, LightPdf, 1, scatteringPdf);
					Ld += f * Li * weight / LightPdf;
				}
			}
		}
	}

	//sample BSDF
	if (!IsDeltaLight(light.flags))
	{
		LinearColor f;
		bool sampledSpecular = false;
		if (it.IsSurfaceInteraction())
		{
			BxDFType sampledType;
			const SurfaceInteraction& isect = (const SurfaceInteraction&)it;
			f = isect.bsdf->Sample_f(isect.wo, &wi, uShading, &scatteringPdf, bsdfFlags, &sampledType);
			f *= AbsDot(wi, isect.shading.n);
			sampledSpecular = (sampledType & BSDF_SPECULAR) != 0;
		}
		else
		{
			//medium
			const MediumInteraction& mi = (const MediumInteraction&)it;
			float p = mi.phase->Sample_p(mi.wo, &wi, uShading);
			f = LinearColor(p);
			scatteringPdf = p;
		}

		if (!f.IsBlack() && scatteringPdf > 0)
		{
			float weight = 1;
			if (!sampledSpecular)
			{
				LightPdf = light.Pdf_Li(it, wi);
				if (LightPdf == 0) return Ld;
				weight = PowerHeuristic(1, scatteringPdf, 1, LightPdf);
			}

			SurfaceInteraction lightIsect;
			Ray ray = it.SpawnRay(wi);
			LinearColor Tr(1.f);
			bool foundSurfaceInteraction =  handleMedia ? scene.IntersectTr(ray,sampler,&lightIsect,&Tr) : scene.Intersect(ray, &lightIsect);

			LinearColor Li;
			if (foundSurfaceInteraction)
			{
				if (lightIsect.object->GetAreaLight() == &light)
				{
					Li = lightIsect.Le(-wi);
				}
			}
			else
			{
				Li = light.Le(ray);
			}

			if (!Li.IsBlack()) Ld += f * Li * Tr * weight / scatteringPdf;
		}
		
	}
	assert(!std::isnan(Ld[0]));
	return Ld;
}

void SamplerIntegrator::Render(const Scene& scene)
{
	Bounds2i imageBounds = camera->film->GetBounds();
	Vector2i imageExtents = imageBounds.Diagonal();

	const int tileSize = 32;
	Vector2i nTiles((imageExtents.X + tileSize - 1) / tileSize,
		(imageExtents.Y + tileSize - 1) / tileSize);

	Preprocess(scene, *sampler.get());

	ParallelFor2D([=](Vector2i tile) {

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
			for (int x = StartX; x < EndX; ++x)
			{
				tileSampler->StartPixel(Vector2i(x,y));
				int i = 0;
				do
				{
					Vector2f pixelSample = tileSampler->GetPixelSample(Vector2i(x, y));
					Ray r;
					float rayWeight = camera->GenerateRay(pixelSample, &r);

					LinearColor L(0.f);
					if (rayWeight > 0) L = Li(r, scene, *tileSampler, arena);
					i++;
					filmTile->AddSample(pixelSample, L, rayWeight);
					arena.Reset();

				} while (tileSampler->StartNextSample());
			}
		}
		camera->film->MergeFilmTile(std::move(filmTile));

		if(OnRenderProgress)
			OnRenderProgress(tile);

	}, nTiles);

}

