#pragma once

#include <memory>
#include <vector>
#include <mutex>

#include "Transform.h"
#include "Bounds.h"
#include "Filter.h"

struct PhysicalPixel
{
	LinearColor contribSum = 0;
	float filterWeightSum = 0;
};

struct DisplayPixel
{
	float xyz[3]{0};
	float weight = 0;
};

class FilmTile
{
public:
	FilmTile(const Bounds2i& pixelBounds,const Vector2f& filterRadius,const float* filterTable, int filterTableSize );
	Bounds2i GetPixelBounds() const { return pixelBounds; }
	void AddSample(const Vector2f& pFilm, LinearColor L, float sampleWight = 1.0f);
	PhysicalPixel& GetPixel(const Vector2i& p);
	const PhysicalPixel& GetPixel(const Vector2i& p) const;
private:
	const Bounds2i pixelBounds;
	std::vector<PhysicalPixel> pixels;
	const Vector2f filterRadius, invFilterRadius;
	const float* filterTable;
	const int filterTableSize;
	friend class Film;
};

class Film
{
public:
	Film(const Vector2i& resolution, std::unique_ptr<Filter> filter);

	Bounds2i GetBounds() const;
	std::unique_ptr<FilmTile> GetFilmTile(const Bounds2i& sampleBounds);
	void MergeFilmTile(std::unique_ptr<FilmTile> tile);
	std::unique_ptr<float[]> GetBuffer() const;

	const Vector2i fullResolution;
	Bounds2i bounds;
	std::unique_ptr<Filter> filter;

private:
	std::unique_ptr<DisplayPixel[]> pixels;
	static const int filterTableWidth = 16;
	float filterTable[filterTableWidth*filterTableWidth];
	std::mutex tileMutex;

	DisplayPixel& GetPixel(const Vector2i& p) const
	{
		int width = bounds.pMax.X - bounds.pMin.X;
		int offset = (p.X - bounds.pMin.X) + (p.Y - bounds.pMin.Y) * width;
		return pixels[offset];
	}
};

class Camera
{
public:
	Camera(const Transform& InCameraToWorld, Film* film)
		: CameraToWorld(InCameraToWorld)
		, film(film)
	{}
	virtual float GenerateRay(const Vector2f& sample, Ray* OutRay) const = 0;

	Film* film;
protected:
	Transform CameraToWorld;
};

class PerspectiveCamera : public Camera
{
public:
	PerspectiveCamera(const Transform& InCameraToWorld, Film* film, float fov,float zNeer,float zFar)
		: Camera(InCameraToWorld, film)
		, CameraToNDC(Perspective(fov, (float)film->fullResolution.X / (float)film->fullResolution.Y, zNeer, zFar))
	{
		Vector2i Resolution = film->fullResolution;
		NDCToScreen = Scale(Resolution.X / 2.f, Resolution.Y / 2.f,1.f) * Translate(Vector3f(-1.f,-1.f,0.f));
		ScreenToNDC = Inverse(NDCToScreen);
		ScreenToCamera = Inverse(CameraToNDC) * ScreenToNDC;
	}
	virtual float GenerateRay(const Vector2f& pixelSample, Ray* OutRay) const override;
private:
	Transform CameraToNDC;
	Transform NDCToScreen;
	Transform ScreenToNDC;
	Transform ScreenToCamera;
	float FocalLength;
};