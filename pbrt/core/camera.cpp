#include "camera.h"

Camera::Camera(const AnimatedTransform& CameraToWorld, Float shutterOpen, Float shutterClose, Film* film, const Medium* medium)
	:CameraToWorld(CameraToWorld),
	shutterOpen(shutterOpen),
	shutterClose(shutterClose),
	film(film),
	medium(medium)
{
	if (CameraToWorld.HasScale())
	{
		//Warning();
	}
}

Camera::~Camera() { delete film; }

Float Camera::GenerateRayDifferential(const CameraSample& sample, RayDifferential* rd) const
{
	Float wt = GenerateRay(sample, rd);
	if (wt == 0) return 0;

	Float wtx;
	for (Float eps : { .05, -.05})
	{
		CameraSample sshift = sample;
		sshift.pFilm.x += eps;
		Ray rx;
		wtx = GenerateRay(sshift, &rx);
		rd->rxOrigin = rd->o + (rx.o - rd->o) / eps;
		rd->rxDirection = rd->d + (rx.d - rd->d) / eps;
		if(wtx !=0)
			break;
	}
	if (wtx == 0)
		return 0;

	Float wty;
	for (Float eps : { .05, -.05})
	{
		CameraSample sshift = sample;
		sshift.pFilm.y += eps;
		Ray ry;
		wty = GenerateRay(sshift, &ry);
		rd->ryOrigin = rd->o + (ry.o - rd->o) / eps;
		rd->ryDirection = rd->d + (ry.d - rd->d) / eps;
		if(wty != 0)
			break;
	}
	if (wty == 0)
		return 0;

	rd->hasDifferentials = true;
	return wt;
}

Spectrum Camera::We(const Ray& ray, Point2f *pRaster2 /*= nullptr*/) const
{
	return Spectrum(0);
}

void Camera::Pdf_We(const Ray& ray, Float *pdfPos, Float *pdfDir) const
{

}

Spectrum Camera::Sample_Wi(const Interaction& ref, const Point2f &u, Vector3f* wi, Float* pdf, Point2f* pRaster, VisibilityTester* vis) const
{
	return Spectrum(0.f);
}

