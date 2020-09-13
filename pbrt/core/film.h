#pragma once

#include "pbrt.h"
#include "geometry.h"
#include "spectrum.h"
#include "filter.h"
//#include "stats.h"
#include "parallel.h"

struct FilmTilePixel
{
	Spectrum contribSum = 0.f;
	Float filterWeightSum = 0.f;
};

class Film
{
public:
	Film(const Point2i& resolution, const Bounds2f& cropWindow,
		std::unique_ptr<Filter> filter, Float diagonal,
		const std::string &filename, Float scale,
		Float maxSampleLumiance = Infinity);
	Bounds2i GetSampleBounds() const;
	Bounds2f GetPhysicalExtent() const;
	std::unique_ptr<FilmTile> GetFilmTile(const Bounds2i& sampleBounds);
	void MergeFileTile(std::unique_ptr<FilmTile> tile);
	void SetImage(const Spectrum* img) const;
	void AddSplat(const Point2f& p, Spectrum v);
	void WriteImage(Float SplateScale = 1);
	void Clear();

	const Point2i fullResolution;
	const Float diagonal;
	std::unique_ptr<Filter> filter;
	const std::string filename;
	Bounds2i croppedPixelBounds;

private:
	struct Pixel
	{
		Float xyz[3];
		Float filterWeightSum;
		AtomicFloat splatXYZ[3];
		Float pad;
	};

	std::unique_ptr<Pixel[]> pixels;
	static constexpr int filterTableWidth = 16;
	Float filterTable[filterTableWidth*filterTableWidth];
	std::mutex mutex;
	const Float scale;
	const Float maxSampleLuminance;

	Pixel& GetPixel(const Point2i& p)
	{
		DOCHECK(InsideExclusive(p, croppedPixelBounds));
		int width = croppedPixelBounds.pMax.x - croppedPixelBounds.pMin.x;
		int offset = (p.x - croppedPixelBounds.pMin.x) + (p.y - croppedPixelBounds.pMin.y) * width;
		return pixels[offset];
	}
};

class FilmTile
{
public:
	FilmTile(const Bounds2i &pixelBounds, const Vector2f& filterRadius,
		const Float* filterTable,int filterTableSize,
		Float maxSampleLuminance)
		:pixelBounds(pixelBounds),
		filterRadius(filterRadius),
		invFilterRadius(1/filterRadius.x, 1 / filterRadius.y),
		filterTable(filterTable),
		filterTableSize(filterTableSize),
		maxSampleLuminance(maxSampleLuminance)
	{
		pixels = std::vector<FilmTilePixel>(std::max(0,pixelBounds.Area()));
	}

	void AddSample(const Point2f& pFilm, Spectrum L, Float sampleWieght = 1.0)
	{
		if (L.y() > maxSampleLuminance)
			L *= maxSampleLuminance;

		Point2f pFilmDiscrete = pFilm - Vector2f(0.5f, 0.5f);
		Point2i p0 = (Point2i)Ceil(pFilmDiscrete - filterRadius);
		Point2i p1 = (Point2i)Floor(pFilmDiscrete + filterRadius) + Point2i(1,1);
		p0 = Max(p0, pixelBounds.pMin);
		p1 = Min(p1, pixelBounds.pMax);

		int *ifx = ALLOCA(int, p1.x - p0.x);
		for (int x = p0.x; x < p1.x; ++x)
		{
			Float fx = std::abs((x - pFilmDiscrete.x)*invFilterRadius.x * filterTableSize);
			ifx[x - p0.x] = std::min((int)std::floor(fx), filterTableSize - 1);
		}
		int *ify = ALLOCA(int, p1.y - p0.y);
		for (int y = p0.y; y < p1.y; ++y)
		{
			Float fy = std::abs((y - pFilmDiscrete.y)*invFilterRadius.y * filterTableSize);
			ify[y - p0.y] = std::min((int)std::floor(fy), filterTableSize - 1);
		}
		for (int y = p0.y; y < p1.y; ++y)
		{
			for (int x = p0.x; x < p1.x; ++x)
			{
				int offset = ify[y - p0.y] * filterTableSize + ifx[x - p0.x];
				Float filterWeight = filterTable[offset];
				FilmTilePixel& pixel = GetPixel(Point2i(x, y));
				pixel.contribSum += L * sampleWieght*filterWeight;
				pixel.filterWeightSum += filterWeight;
			}
		}
	}

	FilmTilePixel &GetPixel(const Point2i& p)
	{
		DOCHECK(InsideExclusive(p, pixelBounds));
		int width = pixelBounds.pMax.x - pixelBounds.pMin.x;
		int offset = (p.x - pixelBounds.pMin.x) + (p.y - pixelBounds.pMin.y) * width;
		return pixels[offset];
	}

	const FilmTilePixel& GetPixel(const Point2i &p) const
	{
		DOCHECK(InsideExclusive(p, pixelBounds));
		int width = pixelBounds.pMax.x - pixelBounds.pMin.x;
		int offset = (p.x - pixelBounds.pMin.x) + (p.y - pixelBounds.pMin.y) * width;
		return pixels[offset];
	}
	Bounds2i GetPixelBounds() const { return pixelBounds; }
private:
	const Bounds2i pixelBounds;
	const Vector2f filterRadius, invFilterRadius;
	const Float *filterTable;
	const int filterTableSize;
	std::vector<FilmTilePixel> pixels;
	const Float maxSampleLuminance;
	friend class Film;
};

Film* CreateFilm(const ParamSet &params, std::unique_ptr<Filter> filter);