#pragma once

#include "pbrt.h"
#include "geometry.h"
#include "transform.h"
#include "film.h"

class Camera
{
public:
	Camera(const AnimatedTransform& CameraToWorld, Float shutterOpen, Float shutterClose, Film* film, const Medium* medium);
	virtual ~Camera();
	virtual Float GenerateRay(const CameraSample& sample, Ray* ray) const = 0;
	virtual Float GenerateRayDifferential(const CameraSample& sample, RayDifferential* rd) const;
	virtual Spectrum We(const Ray& ray, Point2f *pRaster2 = nullptr) const;
	virtual void Pdf_We(const Ray& ray, Float *pdfPos, Float *pdfDir) const;
	virtual Spectrum Sample_Wi(const Interaction& ref, const Point2f &u, Vector3f* wi, Float* pdf, Point2f* pRaster, VisibilityTester* vis) const;

	AnimatedTransform CameraToWorld;
	const Float shutterOpen, shutterClose;
	Film* film;
	const Medium* medium;
};

struct CameraSample
{
	Point2f pFilm;
	Point2f pLens;
	Float time;
};

class ProjectiveCamera : public Camera
{
public:
	ProjectiveCamera(const AnimatedTransform& CameraToWorld,
		const Transform& CameraToScreen,
		const Bounds2f& screenWindow,
		Float shutteropen,
		Float shutterClose,
		Float lensr, Float focald, Film* film,
		const Medium* medium)
		:Camera(CameraToWorld, shutterOpen, shutterClose, film, medium), CameraToScreen(CameraToScreen)
	{
		lensRadius = lensr;
		focalDistance = focald;

		ScreenToRaster = Scale(film->fullResolution.x, film->fullResolution.y, 1) *
			Scale(	1 / (screenWindow.pMax.x - screenWindow.pMin.x), 
					1 / (screenWindow.pMin.y - screenWindow.pMax.y), 1) *
			Translate(Vector3f(-screenWindow.pMin.x, -screenWindow.pMax.y, 0));

		RasterToScreen = Inverse(ScreenToRaster);
		RasterToCamera = Inverse(CameraToScreen) * RasterToScreen;

	}
protected:
	Transform CameraToScreen, RasterToCamera;
	Transform ScreenToRaster, RasterToScreen;
	Float lensRadius, focalDistance;
};