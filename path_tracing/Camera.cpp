#include "Camera.h"

float PerspectiveCamera::GenerateRay(uint32_t x, uint32_t y, Ray* OutRay)
{
	Vector3f pFilm = Vector3f(x,y,0);
	Vector3f pCamera = ScreenToCamera(pFilm);
	*OutRay = Ray(Vector3f(0, 0, 0), Normalize(pCamera));
	*OutRay = CameraToWorld(*OutRay);
	return 1;
}

