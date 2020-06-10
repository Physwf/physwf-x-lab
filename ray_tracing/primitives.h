#pragma once

#include "core.h"

struct Sphere
{
	Point center;
	Float rad, rad2;
};

inline const char* SphereName(const Prim* prim)
{
	return "sphere";
}

void SpherePrint(const Prim* prim);

Prim* SphereRead(const char* desc, std::size_t size);

int SphereIntersect(const Ray* ray, const Prim* prim, Isect* hit);

void SphereNormal(const Prim* prim, const Point P, Point N);

