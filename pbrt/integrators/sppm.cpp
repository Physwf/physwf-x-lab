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
	SPPMPixel() : M(0) {}

	Float radius = 0;
	Spectrum Ld;

	struct VisiblePoint
	{
		VisiblePoint() {}
		VisiblePoint(const Point3f &p, const Vector3f &wo, const BSDF *bsdf,
			const Spectrum &beta)
			: p(p), wo(wo), bsdf(bsdf), beta(beta) {}
		Point3f p;
		Vector3f wo;
		const BSDF* bsdf = nullptr;
		Spectrum beta;
	} vp;

	AtomicFloat Phi[Spectrum::nSamples];
	std::atomic<int> M;
	Float N = 0;
	Spectrum tau;
};

struct SPPMPixelListNode
{
	SPPMPixel* pixel;
	SPPMPixelListNode* next;
};

static bool ToGrid(const Point3f& p, const Bounds3f& bounds, const int gridRes[3], Point3i* pi)
{
	bool inBounds = true;
	Vector3f pg = bounds.Offset(p);
	for (int i = 0; i < 3; ++i)
	{
		(*pi)[i] = (int)(gridRes[i] * pg[i]);
		inBounds &= ((*pi)[i] >= 0 && (*pi)[i] < gridRes[i]);
		(*pi)[i] = Clamp((*pi)[i], 0, gridRes[i] - 1);
	}
	return inBounds;
}

inline unsigned int hash(const Point3i &p, int hashSize) {
	return (unsigned int)((p.x * 73856093) ^ (p.y * 19349663) ^
		(p.z * 83492791)) %
		hashSize;
}

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
		//生成visible point
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
	
		//创建visible point栅格
		int hashSize = nPixels;
		std::vector<std::atomic<SPPMPixelListNode*>> grid(hashSize);
		Bounds3f gridBounds;
		Float maxRadius = 0;
		for (int i = 0; i < nPixels; ++i)
		{
			const SPPMPixel& pixel = pixels[i];
			if (pixel.vp.beta.IsBlack())
				continue;
			Bounds3f vpBound = Expand(Bounds3f(pixel.vp.p), pixel.radius);
			gridBounds = Union(gridBounds, vpBound);
			maxRadius = std::max(maxRadius, pixel.radius);
		}
		Vector3f diag = gridBounds.Diagonal();
		Float maxDiag = MaxComponent(diag);
		int baseGridRes = (int)(maxDiag / maxRadius);
		int gridRes[3];
		for (int i = 0; i < 3; ++i)
			gridRes[i] = std::max((int)(baseGridRes*diag[i] / maxDiag), 1);
	
		//将visible point 加入栅格
		ParallelFor([&](int64_t pixelIndex) {
			MemoryArena& arena = perThreadArenas[ThreadIndex];
			SPPMPixel& pixel = pixels[pixelIndex];
			if (!pixel.vp.beta.IsBlack())
			{
				Float radius = pixel.radius;
				Point3i pMin, pMax;
				ToGrid(pixel.vp.p - Vector3f(radius, radius, radius), gridBounds, gridRes, &pMin);
				ToGrid(pixel.vp.p + Vector3f(radius, radius, radius), gridBounds, gridRes, &pMax);
				for (int z = pMin.z; z < pMax.z; ++z)
					for (int y = pMin.y; y < pMax.y; ++y)
						for (int x = pMin.x; x < pMax.x; ++x)
						{
							int h = hash(Point3i(x, y, z), hashSize);
							SPPMPixelListNode* node = arena.Alloc<SPPMPixelListNode>();
							node->pixel = &pixel;
							node->next = grid[h];
							while (grid[h].compare_exchange_weak(node->next, node) == false)
								;
						}
			}
		}, nPixels, 4096);

		//发射光子
		std::vector<MemoryArena> photoShootArenas(MaxThreadIndex());
		ParallelFor([&](int64_t photonIndex) {
			MemoryArena& arena = photoShootArenas[ThreadIndex];
			uint64_t haltonIndex = (uint64_t)iter *(uint64_t)photonsPerIteration + photonIndex;
			int haltonDim = 0;

			Float lightPdf;
			Float lightSample = RadicalInverse(haltonDim++, haltonIndex);
			int lightNum = lightDistr->SampleDiscrete(lightSample, &lightPdf);
			const std::shared_ptr<Light>& light = scene.lights[lightNum];

			Point2f uLihgt0(RadicalInverse(haltonDim, haltonIndex),
				RadicalInverse(haltonDim + 1, haltonIndex));
			Point2f uLihgt1(RadicalInverse(haltonDim + 2, haltonIndex),
				RadicalInverse(haltonDim + 3, haltonIndex));
			Float uLightTime = Lerp(RadicalInverse(haltonDim + 4, haltonIndex), camera->shutterOpen, camera->shutterClose);
			haltonDim += 5;

			RayDifferential photonRay;
			Normal3f nLight;
			Float pdfPos, pdfDir;
			Spectrum Le = light->Sample_Le(uLihgt0, uLihgt1, uLightTime, &photonRay, &nLight, &pdfPos, &pdfDir);
			if (pdfPos == 0 || pdfDir == 0 || Le.IsBlack()) return;
			Spectrum beta = (AbsDot(nLight, photonRay.d)*Le) / (lightPdf*pdfPos*pdfDir);
			if (beta.IsBlack()) return;

			SurfaceInteraction isect;
			for (int depth = 0; depth < maxDepth; ++depth)
			{
				if (!scene.Intersect(photonRay, &isect))
					break;
				if (depth > 0)
				{
					Point3i photoGridIndex;
					if (ToGrid(isect.p, gridBounds, gridRes, &photoGridIndex))
					{
						int h = hash(photoGridIndex, hashSize);
						for (SPPMPixelListNode* node = grid[h].load(std::memory_order_relaxed); node != nullptr; node = node->next)
						{
							SPPMPixel &pixel = *node->pixel;
							Float radius = pixel.radius;
							if (DistanceSquared(pixel.vp.p, isect.p) > radius*radius)
								continue;
							Vector3f wi = -photonRay.d;
							Spectrum Phi = beta * pixel.vp.bsdf->f(pixel.vp.wo, wi);
							for (int i = 0; i < Spectrum::nSamples; ++i)
								pixel.Phi[i].Add(Phi[i]);
							++pixel.M;
						}
					}
				}
				isect.ComputeScatteringFunctions(photonRay, arena, true, TransportMode::Importance);
				if (!isect.bsdf)
				{
					--depth;
					photonRay = isect.SpawnRay(photonRay.d);
					continue;
				}
				const BSDF& photonBSDF = *isect.bsdf;
				Vector3f wi, wo = -photonRay.d;
				Float pdf;
				BxDFType flags;
				Point2f bsdfSample(RadicalInverse(haltonDim, haltonIndex),
					RadicalInverse(haltonDim + 1, haltonIndex));
				Spectrum fr = photonBSDF.Sample_f(wo, &wi, bsdfSample, &pdf, BSDF_ALL, &flags);
				if (fr.IsBlack() || pdf == 0.f) break;

				Spectrum bnew = beta * fr * AbsDot(wi, isect.shading.n) / pdf;

				Float q = std::max((Float)0, 1 - bnew.y() / beta.y());
				if (RadicalInverse(haltonDim++, haltonIndex) < q)
					break;
				beta = bnew / (1 - q);

				photonRay = (RayDifferential)isect.SpawnRay(wi);
			}
			arena.Reset();
		}, photonsPerIteration, 8192);

		//更新像素
		ParallelFor([&](int64_t i) {
			SPPMPixel &p = pixels[i];
			if (p.M > 0) {
				// Update pixel photon count, search radius, and $\tau$ from
				// photons
				Float gamma = (Float)2 / (Float)3;
				Float Nnew = p.N + gamma * p.M;
				Float Rnew = p.radius * std::sqrt(Nnew / (p.N + p.M));
				Spectrum Phi;
				for (int j = 0; j < Spectrum::nSamples; ++j)
					Phi[j] = p.Phi[j];
				p.tau = (p.tau + p.vp.beta * Phi) * (Rnew * Rnew) /
					(p.radius * p.radius);
				p.N = Nnew;
				p.radius = Rnew;
				p.M = 0;
				for (int j = 0; j < Spectrum::nSamples; ++j)
					p.Phi[j] = (Float)0;
			}
			// Reset _VisiblePoint_ in pixel
			p.vp.beta = 0.;
			p.vp.bsdf = nullptr;
		}, nPixels, 4096);


		// Periodically store SPPM image in film and write image
		if (iter + 1 == nIterations || ((iter + 1) % writeFrequency) == 0) {
			int x0 = pixelBounds.pMin.x;
			int x1 = pixelBounds.pMax.x;
			uint64_t Np = (uint64_t)(iter + 1) * (uint64_t)photonsPerIteration;
			std::unique_ptr<Spectrum[]> image(new Spectrum[pixelBounds.Area()]);
			int offset = 0;
			for (int y = pixelBounds.pMin.y; y < pixelBounds.pMax.y; ++y) {
				for (int x = x0; x < x1; ++x) {
					// Compute radiance _L_ for SPPM pixel _pixel_
					const SPPMPixel &pixel =
						pixels[(y - pixelBounds.pMin.y) * (x1 - x0) + (x - x0)];
					Spectrum L = pixel.Ld / (iter + 1);
					L += pixel.tau / (Np * Pi * pixel.radius * pixel.radius);
					image[offset++] = L;
				}
			}
			camera->film->SetImage(image.get());
			camera->film->WriteImage();
			// Write SPPM radius image, if requested
			if (getenv("SPPM_RADIUS")) {
				std::unique_ptr<Float[]> rimg(
					new Float[3 * pixelBounds.Area()]);
				Float minrad = 1e30f, maxrad = 0;
				for (int y = pixelBounds.pMin.y; y < pixelBounds.pMax.y; ++y) {
					for (int x = x0; x < x1; ++x) {
						const SPPMPixel &p =
							pixels[(y - pixelBounds.pMin.y) * (x1 - x0) +
							(x - x0)];
						minrad = std::min(minrad, p.radius);
						maxrad = std::max(maxrad, p.radius);
					}
				}
// 				fprintf(stderr,
// 					"iterations: %d (%.2f s) radius range: %f - %f\n",
// 					iter + 1, progress.ElapsedMS() / 1000., minrad, maxrad);
				int offset = 0;
				for (int y = pixelBounds.pMin.y; y < pixelBounds.pMax.y; ++y) {
					for (int x = x0; x < x1; ++x) {
						const SPPMPixel &p =
							pixels[(y - pixelBounds.pMin.y) * (x1 - x0) +
							(x - x0)];
						Float v = 1.f - (p.radius - minrad) / (maxrad - minrad);
						rimg[offset++] = v;
						rimg[offset++] = v;
						rimg[offset++] = v;
					}
				}
				Point2i res(pixelBounds.pMax.x - pixelBounds.pMin.x,
					pixelBounds.pMax.y - pixelBounds.pMin.y);
				WriteImage("sppm_radius.png", rimg.get(), pixelBounds, res);
			}
		}

		// Reset memory arenas
		for (size_t i = 0; i < perThreadArenas.size(); ++i)
			perThreadArenas[i].Reset();
	}
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
