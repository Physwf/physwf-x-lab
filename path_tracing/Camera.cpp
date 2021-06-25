#include "Camera.h"

Bounds2i Film::GetBounds() const
{
	return bounds;
}

float PerspectiveCamera::GenerateRay(const Vector2f& pixelSample, Ray* OutRay) const
{
	Vector3f pFilm = Vector3f(pixelSample.X, pixelSample.X,0);
	Vector3f pCamera = ScreenToCamera(pFilm);
	*OutRay = Ray(Vector3f(0, 0, 0), Normalize(pCamera));
	*OutRay = CameraToWorld(*OutRay);
	return 1;
}

