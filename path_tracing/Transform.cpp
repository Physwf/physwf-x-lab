#include "Transform.h"

Transform Transform::operator*(const Transform& t)
{
	Transform Result;
	XMStoreFloat4x4(&Result.M,	  XMMatrixMultiply(XMLoadFloat4x4(&M), XMLoadFloat4x4(&t.M)));
	XMStoreFloat4x4(&Result.InvM, XMMatrixMultiply(XMLoadFloat4x4(&InvM), XMLoadFloat4x4(&t.InvM)));
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
	ret.n = Normalize(t(si.n));
	ret.wo = Normalize(t(si.wo));
	ret.shape = si.shape;
	ret.dpdu = t(si.dpdu);
	ret.dpdv = t(si.dpdv);
	ret.dndu = t(si.dndu);
	ret.dndv = t(si.dndv);
	ret.bsdf = si.bsdf;
	ret.object = si.object;

	return ret;
}


Transform Perspective(float fov, float aspect, float znear, float zfar)
{
	XMFLOAT4X4 M;
	XMStoreFloat4x4(&M, XMMatrixPerspectiveFovLH(fov, aspect, znear, zfar));
	return Transform(M);
}
Transform Translate(const Vector3f& delta)
{
	XMFLOAT4X4 m(1, 0, 0, delta.X, 0, 1, 0, delta.Y, 0, 0, 1, delta.Z, 0, 0, 0, 1);
	XMFLOAT4X4 minv(1, 0, 0, -delta.X, 0, 1, 0, -delta.Y, 0, 0, 1, -delta.Z, 0, 0, 0, 1);
	return Transform(m, minv);
}
Transform Scale(float x, float y, float z)
{
	XMFLOAT4X4 m(x, 0, 0, 0, 0, y, 0, 0, 0, 0, z, 0, 0, 0, 0, 1);
	XMFLOAT4X4 minv(1 / x, 0, 0, 0, 0, 1 / y, 0, 0, 0, 0, 1 / z, 0, 0, 0, 0, 1);
	return Transform(m, minv);
}
