#pragma once

#include "shape.h"

class Sphere : public Shape {
public:
	// Sphere Public Methods
	Sphere(const Transform *ObjectToWorld, const Transform *WorldToObject,
		bool reverseOrientation, Float radius, Float zMin, Float zMax,
		Float phiMax)
		: Shape(ObjectToWorld, WorldToObject, reverseOrientation),
		radius(radius),
		zMin(Clamp(std::min(zMin, zMax), -radius, radius)),
		zMax(Clamp(std::max(zMin, zMax), -radius, radius)),
		thetaMin(std::acos(Clamp(std::min(zMin, zMax) / radius, -1, 1))),
		thetaMax(std::acos(Clamp(std::max(zMin, zMax) / radius, -1, 1))),
		phiMax(Radians(Clamp(phiMax, 0, 360))) {}
	Bounds3f ObjectBound() const;
	bool Intersect(const Ray &ray, Float *tHit, SurfaceInteraction *isect,
		bool testAlphaTexture) const;
	bool IntersectP(const Ray &ray, bool testAlphaTexture) const;
	Float Area() const;
	Interaction Sample(const Point2f &u, Float *pdf) const;
	Interaction Sample(const Interaction &ref, const Point2f &u,
		Float *pdf) const;
	Float Pdf(const Interaction &ref, const Vector3f &wi) const;
	Float SolidAngle(const Point3f &p, int nSamples) const;

private:
	// Sphere Private Data
	const Float radius;
	const Float zMin, zMax;
	const Float thetaMin, thetaMax, phiMax;
};

std::shared_ptr<Shape> CreateSphereShape(const Transform *o2w,
	const Transform *w2o,
	bool reverseOrientation,
	const ParamSet &params);