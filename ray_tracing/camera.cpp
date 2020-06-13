#include "camera.h"
#include "core.h"

void GetRayFromPixel(const Camera* camera, int x, int y, Ray* Result)
{
	Vec v = camera->direction;
	v = v * camera->len;
	v = v + camera->position;//screen center position
	Float screenX = ((Float)x + 0.5) - (Float)camera->screen->Width / 2.0;
	Float screenY = ((Float)y + 0.5) - (Float)camera->screen->Height / 2.0;
	Vec v2 = { screenX,screenY,0.0f };//offset
	v = v + v2;//pixel position
	v = v - camera->position;//pixel direction
	Result->P = camera->position;
	Result->D = v;
	Normalize(Result->D);
}
