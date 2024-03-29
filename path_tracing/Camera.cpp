#include "Camera.h"
#define LOG(format,...)									\
	do {												\
		char info[1024] = {'\n'};						\
		sprintf(info, format, __VA_ARGS__);			\
		void OutputDebug(const char* Format);\
		OutputDebug(info);						\
	} while (0);

FilmTile::FilmTile(const Bounds2i& InpixelBounds, const Vector2f& InfilterRadius, const float* InfilterTable, int InfilterTableSize)
	: pixelBounds(InpixelBounds),filterRadius(InfilterRadius), invFilterRadius(1 / filterRadius.X, 1 / filterRadius.Y), filterTable(InfilterTable), filterTableSize(InfilterTableSize)
{
	pixels = std::vector<PhysicalPixel>(std::max(0, pixelBounds.Area()));
}

void FilmTile::AddSample(const Vector2f& pFilm, LinearColor L, float sampleWight /*= 1.0f*/)
{
	Vector2f pFilmDiscrete = pFilm - Vector2f(0.5f, 0.5f);
	Vector2i lu = (Vector2i)Ceil(pFilmDiscrete - filterRadius);
	Vector2i rb = (Vector2i)Floor(pFilmDiscrete + filterRadius) + Vector2i(1,1);
	lu = Max(lu, pixelBounds.pMin);
	rb = Min(rb, pixelBounds.pMax);
	//LOG("lu(%d,%d),size(%d,%d)\n", lu.X,lu.Y,(rb-lu).X, (rb - lu).Y);
	for (int y = lu.Y; y < rb.Y; ++y)
	{
		for (int x = lu.X; x < rb.X; ++x)
		{
			float fx = std::abs((x - pFilmDiscrete.X) * invFilterRadius.X * filterTableSize);
			fx = std::min((int)std::floor(fx), filterTableSize - 1);
			float fy = std::abs((y - pFilmDiscrete.Y) * invFilterRadius.Y * filterTableSize);
			fy = std::min((int)std::floor(fy), filterTableSize - 1);
			int offset = fy * filterTableSize + fx;
			float filterWeight = filterTable[offset];
			//LOG("(%d,%d) (%f,%f,%f) offset:%d filterWeight:%f sampleWight:%f\n", x, y, L[0], L[1], L[2], offset, filterWeight, sampleWight);
			PhysicalPixel& pixel = GetPixel(Vector2i(x,y));
			pixel.contribSum += L * sampleWight * filterWeight;
			pixel.filterWeightSum += filterWeight;
		}
	}
}

PhysicalPixel& FilmTile::GetPixel(const Vector2i& p)
{
	int width = pixelBounds.pMax.X - pixelBounds.pMin.X;
	int offset = (p.X - pixelBounds.pMin.X) + (p.Y - pixelBounds.pMin.Y) * width;
	return pixels[offset];
}

const PhysicalPixel& FilmTile::GetPixel(const Vector2i& p) const
{
	int width = pixelBounds.pMax.X - pixelBounds.pMin.X;
	int offset = (p.X - pixelBounds.pMin.X) + (p.Y - pixelBounds.pMin.Y) * width;
	return pixels[offset];
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

	bounds = Bounds2i(Vector2i(0,0), Vector2i(fullResolution.X, fullResolution.Y));
}

Bounds2i Film::GetBounds() const
{
	return bounds;
}

std::unique_ptr<FilmTile> Film::GetFilmTile(const Bounds2i& sampleBounds)
{
	Vector2f halfpixel = Vector2f(0.5f, 0.5f);
	Bounds2f floatBounds = (Bounds2f)sampleBounds;
	//include sampleBounds plus filter->radius
	Vector2i leftUp =		(Vector2i)Ceil(floatBounds.pMin - halfpixel - filter->radius);
	Vector2i rightDown =	(Vector2i)Floor(floatBounds.pMax - halfpixel + filter->radius) + Vector2i(1,1);
	Bounds2i tilePixelBounds = Intersect(Bounds2i(leftUp,rightDown),bounds);
	return std::make_unique<FilmTile>(tilePixelBounds, filter->radius,filterTable,filterTableWidth);
}

void Film::MergeFilmTile(std::unique_ptr<FilmTile> tile)
{
	std::lock_guard<std::mutex> lk(tileMutex);
	Bounds2i tileBounds = tile->GetPixelBounds();
	for (int y = tileBounds.pMin.Y; y < tileBounds.pMax.Y; ++y)
	{
		for (int x = tileBounds.pMin.X; x < tileBounds.pMax.X; ++x)
		{
			const PhysicalPixel& tilePixel = tile->GetPixel(Vector2i(x,y));
			DisplayPixel& mergePixel = GetPixel(Vector2i(x, y));
			float xyz[3];
			tilePixel.contribSum.ToXYZ(xyz);
			for (int i = 0; i < 3; ++i) mergePixel.xyz[i] += xyz[i];
			mergePixel.weight += tilePixel.filterWeightSum;
		}
	}
}

std::unique_ptr<float[]> Film::GetBuffer() const
{
	float* rgb = (new float[3 * fullResolution.X * fullResolution.Y]);

	int offset = 0;
	for (int y = 0; y < fullResolution.Y; ++y)
	{
		for (int x = 0; x < fullResolution.X; ++x, ++offset)
		{
			DisplayPixel p = GetPixel(Vector2i(x, y));
			XYZToRGB(p.xyz, &rgb[3 * offset]);
			float filterWeightSum = p.weight;
			if (filterWeightSum != 0)
			{
				float invWt = 1.f /*/ filterWeightSum*/;
				rgb[3 * offset + 0] = std::max(0.f, rgb[3 * offset + 0] * invWt);
				rgb[3 * offset + 1] = std::max(0.f, rgb[3 * offset + 1] * invWt);
				rgb[3 * offset + 2] = std::max(0.f, rgb[3 * offset + 2] * invWt);
				//if (rgb[3 * offset] != 0)
				{
					//LOG("x=%d,y=%d  %f %f %f | %f %f %f\n", x,y, rgb[3 * offset + 0], rgb[3 * offset + 1],rgb[3 * offset + 2], p.xyz[0], p.xyz[1], p.xyz[2]);
				}
			}
		}
	}

	return std::unique_ptr<float[]>(rgb);
}

PerspectiveCamera::PerspectiveCamera(const Transform& InCameraToWorld, Film* film, float fov, float zNeer, float zFar) 
	: Camera(InCameraToWorld, film)
	, CameraToNDC(Transform::Perspective(fov, (float)film->fullResolution.X / (float)film->fullResolution.Y, zNeer, zFar))
{
	Vector2i Resolution = film->fullResolution;
	NDCToScreen = Transform::Scale(1.f, -1.f, 1.0) * Transform::Translate(1.f, 1.f, 0.f) * Transform::Scale(Resolution.X / 2.f, Resolution.Y / 2.f, 1.0);
	ScreenToNDC = Inverse(NDCToScreen);
	//Vector3f NDC1 = ScreenToNDC.Point(Vector3f(0, 0, 0));
	//Vector3f NDC2 = ScreenToNDC.Point(Vector3f(500, 500, 0));
	ScreenToCamera = ScreenToNDC * Inverse(CameraToNDC);
	//Vector3f pCamera1 = ScreenToCamera.Point(Vector3f(0, 0, 0));
	//Vector3f pCamera2 = ScreenToCamera.Point(Vector3f(500, 500, 0));
}

float PerspectiveCamera::GenerateRay(const Vector2f& pixelSample, Ray* OutRay) const
{
	Vector3f pFilm = Vector3f(pixelSample.X, pixelSample.Y,0.f);
	Vector3f pCamera = ScreenToCamera.Point(pFilm);
	*OutRay = Ray(Vector3f(0, 0, 0), Normalize(pCamera));
	*OutRay = CameraToWorld(*OutRay);
	return 1;
}

