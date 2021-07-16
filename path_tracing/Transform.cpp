#include "Transform.h"

Transform Transform::operator*(const Transform& t)
{
	Transform Result;
	XMStoreFloat4x4(&Result.M,	  XMMatrixMultiply(XMLoadFloat4x4(&M), XMLoadFloat4x4(&t.M)));
	XMStoreFloat4x4(&Result.InvM, XMMatrixMultiply(XMLoadFloat4x4(&t.InvM), XMLoadFloat4x4(&InvM)));
	return Result;
}

Bounds3f Transform::operator()(const Bounds3f& b) const
{
	const Transform& M = *this;
	Bounds3f ret(M(Vector3f(b.pMin.X, b.pMin.Y, b.pMin.Z)));
	ret = Union(ret, M(Vector3f(b.pMax.X, b.pMin.Y, b.pMin.Z)));
	ret = Union(ret, M(Vector3f(b.pMin.X, b.pMax.Y, b.pMin.Z)));
	ret = Union(ret, M(Vector3f(b.pMin.X, b.pMin.Y, b.pMax.Z)));
	ret = Union(ret, M(Vector3f(b.pMin.X, b.pMax.Y, b.pMax.Z)));
	ret = Union(ret, M(Vector3f(b.pMax.X, b.pMax.Y, b.pMin.Z)));
	ret = Union(ret, M(Vector3f(b.pMax.X, b.pMin.Y, b.pMax.Z)));
	ret = Union(ret, M(Vector3f(b.pMax.X, b.pMax.Y, b.pMax.Z)));
	return ret;
}

SurfaceInteraction Transform::operator()(const SurfaceInteraction& si) const
{
	SurfaceInteraction ret;
	ret.p = (*this)(si.p);

	const Transform& t = *this;
	ret.n = Normalize(t.Normal(si.n));
	ret.wo = Normalize(t.Normal(si.wo));
	ret.shape = si.shape;
	ret.dpdu = t.Normal(si.dpdu);
	ret.dpdv = t.Normal(si.dpdv);
	ret.dndu = t.Normal(si.dndu);
	ret.dndv = t.Normal(si.dndv);
	ret.shading.n = Normalize(t(si.shading.n));
	ret.shading.dpdu = t(si.shading.dpdu);
	ret.shading.dpdv = t(si.shading.dpdv);
	ret.shading.dndu = t(si.shading.dndu);
	ret.shading.dndv = t(si.shading.dndv);
	ret.bsdf = si.bsdf;
	ret.object = si.object;

	return ret;
}

