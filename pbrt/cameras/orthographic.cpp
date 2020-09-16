#include "orthographic.h"
#include "paramset.h"
#include "sampler.h"
#include "sampling.h"


// OrthographicCamera Definitions
Float OrthographicCamera::GenerateRay(const CameraSample &sample,
	Ray *ray) const {
	//ProfilePhase prof(Prof::GenerateCameraRay);
	// Compute raster and camera sample positions
	Point3f pFilm = Point3f(sample.pFilm.x, sample.pFilm.y, 0);
	Point3f pCamera = RasterToCamera(pFilm);
	*ray = Ray(pCamera, Vector3f(0, 0, 1));
	// Modify ray for depth of field
	if (lensRadius > 0) {
		// Sample point on lens
		Point2f pLens = lensRadius * ConcentricSampleDisk(sample.pLens);

		// Compute point on plane of focus
		Float ft = focalDistance / ray->d.z;
		Point3f pFocus = (*ray)(ft);

		// Update ray for effect of lens
		ray->o = Point3f(pLens.x, pLens.y, 0);
		ray->d = Normalize(pFocus - ray->o);
	}
	ray->time = Lerp(sample.time, shutterOpen, shutterClose);
	ray->medium = medium;
	*ray = CameraToWorld(*ray);
	return 1;
}

Float OrthographicCamera::GenerateRayDifferential(const CameraSample &sample,
	RayDifferential *ray) const {
	//ProfilePhase prof(Prof::GenerateCameraRay);
	// Compute main orthographic viewing ray

	// Compute raster and camera sample positions
	Point3f pFilm = Point3f(sample.pFilm.x, sample.pFilm.y, 0);
	Point3f pCamera = RasterToCamera(pFilm);
	*ray = RayDifferential(pCamera, Vector3f(0, 0, 1));

	// Modify ray for depth of field
	if (lensRadius > 0) {
		// Sample point on lens
		Point2f pLens = lensRadius * ConcentricSampleDisk(sample.pLens);

		// Compute point on plane of focus
		Float ft = focalDistance / ray->d.z;
		Point3f pFocus = (*ray)(ft);

		// Update ray for effect of lens
		ray->o = Point3f(pLens.x, pLens.y, 0);
		ray->d = Normalize(pFocus - ray->o);
	}

	// Compute ray differentials for _OrthographicCamera_
	if (lensRadius > 0) {
		// Compute _OrthographicCamera_ ray differentials accounting for lens

		// Sample point on lens
		Point2f pLens = lensRadius * ConcentricSampleDisk(sample.pLens);
		Float ft = focalDistance / ray->d.z;

		Point3f pFocus = pCamera + dxCamera + (ft * Vector3f(0, 0, 1));
		ray->rxOrigin = Point3f(pLens.x, pLens.y, 0);
		ray->rxDirection = Normalize(pFocus - ray->rxOrigin);

		pFocus = pCamera + dyCamera + (ft * Vector3f(0, 0, 1));
		ray->ryOrigin = Point3f(pLens.x, pLens.y, 0);
		ray->ryDirection = Normalize(pFocus - ray->ryOrigin);
	}
	else {
		ray->rxOrigin = ray->o + dxCamera;
		ray->ryOrigin = ray->o + dyCamera;
		ray->rxDirection = ray->ryDirection = ray->d;
	}
	ray->time = Lerp(sample.time, shutterOpen, shutterClose);
	ray->hasDifferentials = true;
	ray->medium = medium;
	*ray = CameraToWorld(*ray);
	return 1;
}

OrthographicCamera *CreateOrthographicCamera(const ParamSet &params,
	const AnimatedTransform &cam2world,
	Film *film, const Medium *medium) {
	// Extract common camera parameters from _ParamSet_
	Float shutteropen = params.FindOneFloat("shutteropen", 0.f);
	Float shutterclose = params.FindOneFloat("shutterclose", 1.f);
	if (shutterclose < shutteropen) {
		Warning("Shutter close time [%f] < shutter open [%f].  Swapping them.",
			shutterclose, shutteropen);
		std::swap(shutterclose, shutteropen);
	}
	Float lensradius = params.FindOneFloat("lensradius", 0.f);
	Float focaldistance = params.FindOneFloat("focaldistance", 1e6f);
	Float frame = params.FindOneFloat(
		"frameaspectratio",
		Float(film->fullResolution.x) / Float(film->fullResolution.y));
	Bounds2f screen;
	if (frame > 1.f) {
		screen.pMin.x = -frame;
		screen.pMax.x = frame;
		screen.pMin.y = -1.f;
		screen.pMax.y = 1.f;
	}
	else {
		screen.pMin.x = -1.f;
		screen.pMax.x = 1.f;
		screen.pMin.y = -1.f / frame;
		screen.pMax.y = 1.f / frame;
	}
	int swi;
	const Float *sw = params.FindFloat("screenwindow", &swi);
	if (sw) {
		if (swi == 4) {
			screen.pMin.x = sw[0];
			screen.pMax.x = sw[1];
			screen.pMin.y = sw[2];
			screen.pMax.y = sw[3];
		}
		else
			Error("\"screenwindow\" should have four values");
	}
	return new OrthographicCamera(cam2world, screen, shutteropen, shutterclose,
		lensradius, focaldistance, film, medium);
}