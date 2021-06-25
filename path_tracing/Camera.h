#pragma once

#include <memory>

#include "Transform.h"
#include "Bounds.h"

class Filter
{

};

class FilmTile
{
public:
	FilmTile(const Bounds2i& pixelBounds);
	Bounds2i GetPixelBounds() const { return pixelBounds; }
	void AddSample(const Vector2f& pFilm, LinearColor L, float sampleWight = 1.0f);
private:
	const Bounds2i pixelBounds;

	friend class Film;
};

class Film
{
public:
	Film(const Vector2i& resolution, std::unique_ptr<Filter> filter);

	Bounds2i GetBounds() const;
	std::unique_ptr<FilmTile> GetFilmTile(const Bounds2i& sampleBounds);
	void MergeFilmTile(std::unique_ptr<FilmTile> tile);

	const Vector2i fullResolution;
	Bounds2i bounds;
	std::unique_ptr<Filter> filter;
};

class Camera
{
public:
	Camera(const Transform& InCameraToWorld):CameraToWorld(InCameraToWorld){}
	virtual float GenerateRay(const Vector2f& sample, Ray* OutRay) const = 0;

	Film* film;
protected:
	Transform CameraToWorld;
};

class PerspectiveCamera : public Camera
{
public:
	PerspectiveCamera(const Transform& InCameraToWorld,int ScreenWidth, int ScreenHeight, float fov,float zNeer,float zFar)
		: Camera(InCameraToWorld), 
		CameraToNDC(Perspective(fov, (float)ScreenWidth / (float)ScreenHeight, zNeer, zFar)),
		Viewport(Vector2i(0, 0),Vector2i(ScreenWidth, ScreenHeight))
	{
		Vector2i Resolution = Viewport.Diagonal();
		NDCToScreen = Scale(Resolution.X / 2.f, Resolution.Y / 2.f,0.f) * Translate(Vector3f(-1.f,-1.f,0.f));
		ScreenToNDC = Inverse(NDCToScreen);
		ScreenToCamera = Inverse(CameraToNDC) * ScreenToNDC;
	}
	virtual float GenerateRay(const Vector2f& pixelSample, Ray* OutRay) const override;
private:
	Transform CameraToNDC;
	Transform NDCToScreen;
	Transform ScreenToNDC;
	Transform ScreenToCamera;
	Bounds2i Viewport;
	float FocalLength;
};