#pragma once

#include "pbrt.h"
#include "geometry.h"
#include "transform.h"
#include "medium.h"
#include "material.h"

struct Interaction
{
	Interaction() :time(0) {}
	Interaction(const Point3f& p, const Normal3f &n, const Vector3f &pError, const Vector3f &wo, Float time, const MediumInterface& mediumInterface)
		:p(p),
		time(time),
		pError(pError),
		wo(Normalize(wo)),
		n(n),
		mediumInterface(mediumInterface)
	{
		DOCHECK(!((n.x == 0) && (n.y == 0) && (n.z == 0)));
	}

	bool IsSurfaceInteraction() const { return n != Normal3f(); }
	Ray SpawnRay(const Vector3f &d) const
	{
		Point3f o = OffsetRayOrigin(p, pError, n, d);
		return Ray(o, d, Infinity, time, GetMedium(d));
	}
	Ray SpawnRayTo(const Point3f& p2)
	{
		Point3f origin = OffsetRayOrigin(p, pError, n, p2 - p);
		Vector3f d = p2 - p;
		return Ray(origin,d,1-ShadowEpsilon,time,GetMedium(d));
	}
	Ray SpawnRayTo(const Interaction& it) const
	{
		Point3f origin = OffsetRayOrigin(p, pError, n, it.p - p);
		Point3f target = OffsetRayOrigin(it.p, it.pError, it.n, origin - it.p);
		Vector3f d = target - origin;
		return Ray(origin, d, 1 - ShadowEpsilon, time, GetMedium(d));
	}
	Interaction(const Point3f &p, const Vector3f &wo, Float time,
		const MediumInterface &mediumInterface)
		: p(p), time(time), wo(wo), mediumInterface(mediumInterface) {}
	Interaction(const Point3f &p, Float time,
		const MediumInterface &mediumInterface)
		: p(p), time(time), mediumInterface(mediumInterface) {}
	bool IsMediumInteraction() const { return !IsSurfaceInteraction(); }
	const Medium* GetMedium(const Vector3f &w) const
	{
		return Dot(w, n) > 0 ? mediumInterface.outside : mediumInterface.inside;
	}

	Point3f p;
	Float time;
	Vector3f pError;
	Vector3f wo;
	Normal3f n;
	MediumInterface mediumInterface;
};

class MediumInteraction : public Interaction
{
public:
	MediumInteraction():phase(nullptr) {}
	MediumInteraction(const Point3f &p, const Vector3f &wo, Float time,
		const Medium *medium, const PhaseFunction *phase)
		:Interaction(p,wo,time,medium),phase(phase) {}
	bool IsValid() const { return phase == nullptr; }

	const PhaseFunction *phase;
};

class SurfaceInteraction : public Interaction
{
public:
	// SurfaceInteraction Public Methods
	SurfaceInteraction() {}
	SurfaceInteraction(const Point3f &p, const Vector3f &pError,
		const Point2f &uv, const Vector3f &wo,
		const Vector3f &dpdu, const Vector3f &dpdv,
		const Normal3f &dndu, const Normal3f &dndv, Float time,
		const Shape *sh,
		int faceIndex = 0);

	void SetShadingGeometry(const Vector3f& dpdu, const Vector3f& dpdv, const Normal3f& dndu, const Normal3f& dndv, bool orientationIsAuthoriative);
	void ComputeScatteringFunctions(const RayDifferential& ray, MemoryArena& arena, bool allowMultipleLobes = false, TransportMode mode = TransportMode::Radiance);
	void ComputeDifferencials(const RayDifferential& r) const;
	Spectrum Le(const Vector3f& w) const;

	Point2f uv;//参数表示
	Vector3f dpdu, dpdv;//空间坐标在参数方向的偏分
	Normal3f dndu, dndv;//法线在参数方向的偏分
	const Shape* shape = nullptr;
	struct
	{
		Normal3f n;
		Vector3f dpdu, dpdv;
		Normal3f dndu, dndv;
	} shading;
	const Primitive* primitive = nullptr;
	BSDF* bsdf;
	BSSRDF* bssrdf = nullptr;

	//partial derivatives of world space positions
	mutable Vector3f dpdx, dpdy;
	//partial derivatives of(u,v) parametric coordinates
	mutable Float dudx = 0, dvdx = 0, dudy = 0, dvdy = 0;

	int faceIndex = 0;
};