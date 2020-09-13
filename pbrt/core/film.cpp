#include "film.h"
#include "paramset.h"
#include "imageio.h"

Film::Film(const Point2i & resolution, 
	const Bounds2f & cropWindow, //NDC空间,在fullResolution截取部分窗口
	std::unique_ptr<Filter> filter, 
	Float diagonal, 
	const std::string & filename, 
	Float scale, 
	Float maxSampleLumiance)
	:fullResolution(resolution),
	diagonal(diagonal*.001),
	filter(std::move(filter)),
	filename(filename),
	scale(scale),
	maxSampleLuminance(maxSampleLuminance)
{
	croppedPixelBounds = 
		Bounds2i(Point2i(	std::ceil(fullResolution.x * cropWindow.pMin.x ),
							std::ceil(fullResolution.y * cropWindow.pMin.y))),
				Point2i(	std::ceil(fullResolution.x * cropWindow.pMax.x),
							std::ceil(fullResolution.y * cropWindow.pMax.y));

	pixels = std::unique_ptr<Pixel[]>(new Pixel[croppedPixelBounds.Area()]);
	//filmPixelMemory += croppedPixelBounds.Area() * sizeof(Pixel);

	int offset = 0;
	for (int y = 0; y < filterTableWidth; ++y)
	{
		for (int x = 0; x < filterTableWidth; ++x, ++offset)
		{
			Point2f p;
			p.x = (x + 0.5f) * filter->radius.x / filterTableWidth;
			p.y = (y + 0.5f) * filter->radius.y / filterTableWidth;
			filterTable[offset] = filter->Evaluate(p);
		}
	}
}

Bounds2i Film::GetSampleBounds() const
{
	Bounds2f floatBounds(	Floor(Point2f(croppedPixelBounds.pMin)+Vector2f(0.5f,0.5f) -filter->radius),
							Ceil(Point2f(croppedPixelBounds.pMax)- Vector2f(0.5f,0.5f) + filter->radius));

	return (Bounds2i)floatBounds;
}

Bounds2f Film::GetPhysicalExtent() const
{
	Float aspect = (Float)fullResolution.y / (Float)fullResolution.x;
	Float x = std::sqrt(diagonal * diagonal / (1 + aspect * aspect));
	Float y = aspect * x;
	return Bounds2f(Point2f(-x / 2, -y / 2), Point2f(x / 2, y / 2));
}

std::unique_ptr<FilmTile> Film::GetFilmTile(const Bounds2i& sampleBounds)
{
	Vector2f halfPixel = Vector2f(0.5f, 0.5f);
	Bounds2f floatBounds = (Bounds2f)sampleBounds;
	Point2i p0 = (Point2i)(Ceil(floatBounds.pMin - halfPixel - filter->radius));
	Point2i p1 = (Point2i)(Floor(floatBounds.pMax - halfPixel - filter->radius));
	Bounds2i tilePixelBounds = Intersect(Bounds2i(p0, p1), croppedPixelBounds);
	return std::unique_ptr<FilmTile>(new FilmTile(tilePixelBounds,filter->radius,filterTable,filterTableWidth,maxSampleLuminance));
}

void Film::MergeFileTile(std::unique_ptr<FilmTile> tile)
{
	std::lock_guard<std::mutex> lock(mutex);
	for (Point2i pixel : tile->GetPixelBounds())
	{
		const FilmTilePixel& tilePixel = tile->GetPixel(pixel);
		Pixel& mergePixel = GetPixel(pixel);
		Float xyz[3];
		tilePixel.contribSum.ToXYZ(xyz);
		for (int i = 0; i < 3; ++i) mergePixel.xyz[i] += xyz[i];
		mergePixel.filterWeightSum += tilePixel.filterWeightSum;
	}
}

void Film::SetImage(const Spectrum * img) const
{
	int nPixels = croppedPixelBounds.Area();
	for (int i = 0; i < nPixels; ++i)
	{
		Pixel &p = pixels[i];
		img[i].ToXYZ(p.xyz);
		p.filterWeightSum = 1;
		p.splatXYZ[0] = p.splatXYZ[1] = p.splatXYZ[2] = 0;
	}
}

void Film::AddSplat(const Point2f& p, Spectrum v)
{
	Point2i pi = Point2i(Floor(p));
	if (InsideExclusive(pi, croppedPixelBounds)) return;
	if (v.y() > maxSampleLuminance)
		v *= maxSampleLuminance;
	Float xyz[3];
	v.ToXYZ(xyz);
	Pixel &pixel = GetPixel(pi);
	for (int i = 0; i < 3; ++i) pixel.splatXYZ[i].Add(xyz[i]);
}

void Film::WriteImage(Float splatScale /*= 1*/)
{
	std::unique_ptr<Float[]> rgb(new Float[3 * croppedPixelBounds.Area()]);
	int offset = 0;
	for (Point2i p : croppedPixelBounds) {
		// Convert pixel XYZ color to RGB
		Pixel &pixel = GetPixel(p);
		XYZToRGB(pixel.xyz, &rgb[3 * offset]);

		// Normalize pixel with weight sum
		Float filterWeightSum = pixel.filterWeightSum;
		if (filterWeightSum != 0) {
			Float invWt = (Float)1 / filterWeightSum;
			rgb[3 * offset] = std::max((Float)0, rgb[3 * offset] * invWt);
			rgb[3 * offset + 1] =
				std::max((Float)0, rgb[3 * offset + 1] * invWt);
			rgb[3 * offset + 2] =
				std::max((Float)0, rgb[3 * offset + 2] * invWt);
		}

		// Add splat value at pixel
		Float splatRGB[3];
		Float splatXYZ[3] = { pixel.splatXYZ[0], pixel.splatXYZ[1],
			pixel.splatXYZ[2] };
		XYZToRGB(splatXYZ, splatRGB);
		rgb[3 * offset] += splatScale * splatRGB[0];
		rgb[3 * offset + 1] += splatScale * splatRGB[1];
		rgb[3 * offset + 2] += splatScale * splatRGB[2];

		// Scale pixel value by _scale_
		rgb[3 * offset] *= scale;
		rgb[3 * offset + 1] *= scale;
		rgb[3 * offset + 2] *= scale;
		++offset;
	}

	::WriteImage(filename, &rgb[0], croppedPixelBounds, fullResolution);
}

void Film::Clear()
{
	for (Point2i p : croppedPixelBounds) {
		Pixel &pixel = GetPixel(p);
		for (int c = 0; c < 3; ++c)
			pixel.splatXYZ[c] = pixel.xyz[c] = 0;
		pixel.filterWeightSum = 0;
	}
}

Film* CreateFilm(const ParamSet &params, std::unique_ptr<Filter> filter)
{
	std::string filename;
	int xres = params.FindOneInt("xresolution", 1280);
	int yres = params.FindOneInt("yresolution", 720);
	Bounds2f crop;
	crop.pMin.x = crop.pMin.y = 0.0f;
	crop.pMax.x = crop.pMax.y = 1.0f;

	Float scale = params.FindOneFloat("scale", 1.);
	Float diagonal = params.FindOneFloat("diagonal", 35.);
	Float maxSampleLuminance = params.FindOneFloat("maxsampleluminance", Infinity);
	return new Film(Point2i(xres, yres), crop, std::move(filter), diagonal, filename, scale, maxSampleLuminance);
}
