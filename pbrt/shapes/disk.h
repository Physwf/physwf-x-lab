#pragma once

#include "shape.h"

class Disk : public Shape
{
public:
	Disk(const Transform* ObjectToWorld,const Transform* WorldToObject,
		bool reverseOrienttation,Float height, Float radius, Float innerRadius, float phiMax)
		:Shape(ObjectToWorld,WorldToObject,reverseOrienttation),
		height(height),
		radius(radius),
		innerRadius(innerRadius),
		phiMax(Radians(Clamp(phiMax,0,360))) {}

	Bounds3f ObjectBound() const;
	bool Intersect(const Ray& ray, Float* tHit, SurfaceInteraction *isect, bool testAlphaTexture /* = true */) const;
	bool IntersectP(const Ray &ray, bool testAlphaTexture) const;
	Float Area() const;
	Interaction Sample(const Point2f& u, Float *pdf) const;
private:
	const Float height/*对象空间中的z轴位置*/, radius, innerRadius, phiMax;
};

std::shared_ptr<Disk> CreateDiskShape(const Transform *o2w,
	const Transform *w2o,
	bool reverseOrientation,
	const ParamSet &params);