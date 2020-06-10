#pragma once

#include "math.h"

struct Screen
{
	int Width;
	int Height;
	unsigned int* buffer;
};

struct Camera
{
	Point position;
	Vec direction;
	Screen* screen;
	Float len;
};

void GetRayFromPixel(const Camera* camera, int x, int y, struct Ray* R);