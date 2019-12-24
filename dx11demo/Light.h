#pragma once

#include "Math.h"

struct PointLight
{
	Vector4 PoistionAndRadias;
	Vector4 ColorAndFalloffExponent;
};

struct DirectionalLight
{
	Vector4 Direction;
	Vector4 Color;
};

struct AmbientLight
{
	Vector4 Color;
};