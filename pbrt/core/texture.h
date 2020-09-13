#pragma once

#include "pbrt.h"
#include "spectrum.h"
#include "geometry.h"
#include "transform.h"
#include "memory.h"

class TextureMapping2D
{
public:
	virtual ~TextureMapping2D();
	//返回(s,t)在x和y方向上的改变
	virtual Point2f Map(const SurfaceInteraction& si, Vector2f* dstdx, Vector2f* dstdy) const = 0;
};

class UVMapping2D : public TextureMapping2D
{
public:
	UVMapping2D(Float su = 1, Float sv = 1, Float du = 0, Float dv = 0);
	Point2f Map(const SurfaceInteraction& si, Vector2f* dstdx, Vector2f* dstdy) const;
private:
	//缩放和偏移值
	const Float su, sv, du, dv;
};

class SphericalMapping2D : public TextureMapping2D
{
public:
	SphericalMapping2D(const Transform& WorldToTexture)
		:WorldToTexture(WorldToTexture) {}
	Point2f Map(const SurfaceInteraction& si, Vector2f* dstdx, Vector2f* dstdy) const;
private:
	Point2f sphere(const Point3f& P) const;
	const Transform WorldToTexture;
};

class CylindricalMapping2D : public TextureMapping2D
{
public:
	CylindricalMapping2D(const Transform& WorldToTexture):WorldToTexture(WorldToTexture) {}
	Point2f Map(const SurfaceInteraction& si, Vector2f* dstdx, Vector2f* dstdy) const;
private:
	Point2f cylinder(const Point3f& p) const
	{
		Vector3f vec = Normalize(WorldToTexture(p) - Point3f(0, 0, 0));
		return Point2f((Pi + std::atan2(vec.y, vec.x)) * Inv2Pi, vec.z);
	}
	const Transform WorldToTexture;
};

class PlanerMapping2D : public TextureMapping2D
{
public:
	PlanerMapping2D(const Vector3f& vs, const Vector3f& vt, Float ds = 0,Float dt = 0)
		:vs(vs),vt(vt),ds(ds),dt(dt) {}

	Point2f Map(const SurfaceInteraction& si, Vector2f* dstdx, Vector2f* dstdy) const;
private:
	//通过两个向量和两个对应的偏移量确定一个平面坐标系
	const Vector3f vs, vt;
	const Float ds, dt;
};

class TextureMapping3D
{
public:
	virtual ~TextureMapping3D();
	virtual Point3f Map(const SurfaceInteraction& si, Vector3f* dpdx, Vector3f* dpdy) const = 0;
};

class IdentityMapping3D : public TextureMapping3D
{
public:
	IdentityMapping3D(const Transform& WorldToTexture) : WorldToTexture(WorldToTexture) {}
	Point3f Map(const SurfaceInteraction& si, Vector3f* dpdx, Vector3f* dpdy) const;
private:
	Transform WorldToTexture;
};

template <typename T>
class Texture
{
public:
	virtual T Evaluate(const SurfaceInteraction&) const = 0;
	virtual ~Texture() {}
};

Float Lanczos(Float, Float tau = 2);
Float Noise(float x, float y = 0.5f, Float z = .5f);
Float Noise(const Point3f &p);
Float FBm(const Point3f &p, const Vector3f &dpdx, const Vector3f &dpdy,
	Float omega, int octaves);
Float Turbulence(const Point3f &p, const Vector3f &dpdx, const Vector3f &dpdy,
	Float omega, int octaves);