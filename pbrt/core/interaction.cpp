#include "interaction.h"
#include "transform.h"
#include "primitive.h"
#include "shape.h"
#include "light.h"

SurfaceInteraction::SurfaceInteraction(const Point3f &p,
	const Vector3f &pError,
	const Point2f &uv,
	const Vector3f &wo,
	const Vector3f &dpdu, const Vector3f &dpdv,
	const Normal3f &dndu, const Normal3f &dndv,
	Float time,
	const Shape *sh,
	int faceIndex /*= 0*/)
	:Interaction(p, Normal3f(Normalize(Cross(dpdu, dpdv))), pError, wo, time, nullptr),
	uv(uv),
	dpdu(dpdu),
	dpdv(dpdv),
	dndu(dndu),
	dndv(dndv),
	shape(sh),
	faceIndex(faceIndex)
{
	shading.n = n;
	shading.dpdu = dpdu;
	shading.dpdv = dpdv;
	shading.dndu = dndu;
	shading.dndu = dndu;
	shading.dndv = dndv;
	if (shape && (shape->reverseOrientation ^ shape->transformSwapsHandedness))
	{
		n *= -1;
		shading.n *= -1;
	}
}

void SurfaceInteraction::SetShadingGeometry(const Vector3f& dpdus, const Vector3f& dpdvs, const Normal3f& dndus, const Normal3f& dndvs, bool orientationIsAuthoriative)
{
	shading.n = Normalize((Normal3f)Cross(dpdus, dndus));
	if (shape && (shape->reverseOrientation ^ shape->transformSwapsHandedness))
		shading.n = -shading.n;
	if (orientationIsAuthoriative)
		n = FaceForward(n, shading.n);
	else
		shading.n = FaceForward(shading.n, n);

	shading.dpdu = dpdus;
	shading.dpdv = dpdvs;
	shading.dndu = dndus;
	shading.dndv = dndvs;
}

void SurfaceInteraction::ComputeScatteringFunctions(const RayDifferential& ray, MemoryArena& arena, bool allowMultipleLobes /*= false*/, TransportMode mode /*= TransportMode::Radiance*/)
{
	ComputeDifferencials(ray);
	primitive->ComputeScatteringFunctions(this, arena, mode, allowMultipleLobes);
}

void SurfaceInteraction::ComputeDifferencials(const RayDifferential& r) const
{
	if (r.hasDifferentials)
	{
		//射线(o,dir)同平面ax+by+cz+d=0的交点:
		//t = -Dot((a,b,c),o)-d/Dot((a,b,c),dir)
		
		Float d = Dot(n, Vector3f(p.x, p.y, p.z));
		Float tx = -(Dot(n, Vector3f(p.x, p.y, p.z)) - d) / Dot(n, r.rxDirection);
		Point3f px = r.rxOrigin + tx * r.rxDirection;
		Float ty = -(Dot(n, Vector3f(r.ryOrigin)) - d) / Dot(n, r.ryDirection);
		Point3f py = r.ryOrigin + ty * r.ryDirection;
		dpdx = px - p;
		dpdy = py - p;
		//通过p′=p+delta_u*∂p/∂u+delta_v*∂p/∂v计算delta_u,delta_v,即dudx,dudy,dudx,dvdy
		//see page603
		int dim[2];
		if (std::abs(n.x) > std::abs(n.y) && std::abs(n.x) > std::abs(n.z))
		{
			dim[0] = 1; dim[1] = 2;
		}
		else if (std::abs(n.y) > std::abs(n.z))
		{
			dim[0] = 0; dim[1] = 2;
		}
		else
		{
			dim[0] = 0; dim[1] = 1;
		}
		Float A[2][2] = { {dpdu[dim[0]], dpdv[dim[0]]},{ dpdu[dim[1]], dpdu[dim[1]]} };
		Float Bx[2] = { px[dim[0]] - p[dim[0]], px[dim[1]] - p[dim[1]] };
		Float By[2] = { py[dim[0]] - p[dim[0]], py[dim[1]] - p[dim[1]] };
		if (!SolveLinearSystem2x2(A, Bx, &dudx, &dvdx))
			dudx = dvdx = 0;
		if (!SolveLinearSystem2x2(A, By, &dudy, &dvdy))
			dudy = dvdy = 0;
	}
	else
	{
		dudx = dvdx = 0;
		dudy = dvdy = 0;
		dpdx = dpdy = Vector3f(0, 0, 0);
	}
}

Spectrum SurfaceInteraction::Le(const Vector3f& w) const
{
	const AreaLight *area = primitive->GetAreaLight();
	return area ? area->L(*this, w) : Spectrum(0.0f);
}

