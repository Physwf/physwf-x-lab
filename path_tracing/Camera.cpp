#include "Camera.h"


FilmTile::FilmTile(const Bounds2i& InpixelBounds, const Vector2f& InfilterRadius, const float* InfilterTable, int InfilterTableSize)
	: pixelBounds(InpixelBounds),filterRadius(InfilterRadius), invFilterRadius(1 / filterRadius.X, 1 / filterRadius.Y), filterTable(InfilterTable), filterTableSize(InfilterTableSize)
{
}

void FilmTile::AddSample(const Vector2f& pFilm, LinearColor L, float sampleWight /*= 1.0f*/)
{

}

Film::Film(const Vector2i& Inresolution, std::unique_ptr<Filter> Infilter)
	: fullResolution(Inresolution),
	filter(std::move(Infilter))
{
	pixels = std::make_unique<DisplayPixel[]>(fullResolution.X*fullResolution.Y);
	int offset = 0;
	for (int y=0; y <filterTableWidth;++y)
	{
		for (int x =0;x < filterTableWidth;++x,++offset)
		{
			Vector2f p;
			p.X = filter->radius.X * (x + 0.5f) / filterTableWidth;
			p.Y = filter->radius.Y * (y + 0.5f) / filterTableWidth;
			filterTable[offset] = filter->Evaluate(p);
		}
	}
}

Bounds2i Film::GetBounds() const
{
	return bounds;
}


std::unique_ptr<FilmTile> Film::GetFilmTile(const Bounds2i& sampleBounds)
{
	Vector2f halfpixel = Vector2f(0.5f, 0.5f);
	Bounds2f floatBounds = (Bounds2f)sampleBounds;
	Vector2i leftUp =		(Vector2i)Ceil(floatBounds.pMin - halfpixel - filter->radius);
	Vector2i rightDown =	(Vector2i)Floor(floatBounds.pMin - halfpixel + filter->radius) + Vector2i(1,1);
	Bounds2i tilePixelBounds = Intersect(Bounds2i(leftUp,rightDown),bounds);
	return std::make_unique<FilmTile>(tilePixelBounds, filter->radius,filterTable,filterTableWidth);
}

float PerspectiveCamera::GenerateRay(const Vector2f& pixelSample, Ray* OutRay) const
{
	Vector3f pFilm = Vector3f(pixelSample.X, pixelSample.X,0);
	Vector3f pCamera = ScreenToCamera(pFilm);
	*OutRay = Ray(Vector3f(0, 0, 0), Normalize(pCamera));
	*OutRay = CameraToWorld(*OutRay);
	return 1;
}

