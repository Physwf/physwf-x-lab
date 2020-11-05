#pragma once

#include "Math.h"

struct PointLight
{
	Vector4 PoistionAndRadias;
	Vector4 ColorAndFalloffExponent;
};

struct DirectionalLight
{
	Vector Direction;
	float Intencity;
	Vector Color;
	float LightSourceAngle;		// Defaults to 0.5357 which is the angle for our sun.
	float LightSourceSoftAngle;	//0.0
};

