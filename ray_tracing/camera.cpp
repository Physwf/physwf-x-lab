#include "camera.h"
#include "core.h"

void GetRayFromPixel(const Camera* camera, int x, int y, Ray* Result)
{
	Vec v;
	Normalize(camera->direction, v);
	Multiply(v, camera->len, v);
	Add(camera->position, v, v);//screen center position
	Float screenX = ((Float)x + 0.5) - (Float)camera->screen->Width / 2.0;
	Float screenY = ((Float)y + 0.5) - (Float)camera->screen->Height / 2.0;
	Vec v2 = { screenX,screenY,0.0f };//offset
	Add(v, v2, v);//pixel position
	Subtract(v, camera->position, v);//pixel direction
	Copy(camera->position, Result->P);
	Copy(v, Result->D);
}
