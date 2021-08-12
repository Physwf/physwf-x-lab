#include "Texture.h"
#include "RayTracing.h"

Vector2f UVMapping2D::Map(const SurfaceInteraction& si, Vector2f* dstdx, Vector2f* dstdy) const
{
	*dstdx = Vector2f(su * si.dudx, sv * si.dvdx);
	*dstdy = Vector2f(su * si.dudy, sv * si.dvdy);
	return Vector2f(du + su * si.uv[0], dv + sv * si.uv[1]);
}

Vector2f ShpereMapping2D::Map(const SurfaceInteraction& si, Vector2f* dstdx, Vector2f* dstdy) const
{
	Vector2f st = sphere(si.p);
	const float delta = 0.1f;
	Vector2f stDeltaX = sphere(si.p + delta * si.dpdx);
	Vector2f stDeltaY = sphere(si.p + delta * si.dpdy);
	*dstdx = (stDeltaX - st) / delta;
	*dstdy = (stDeltaY - st) / delta;
	return st;
}

Vector2f ShpereMapping2D::sphere(const Vector3f& p) const
{
	Vector3f vec = Normalize(WorldToTexture.Point(p) - Vector3f(0,0,0));
	float Theta = std::acos(p.Z);
	float Phi = std::atan2(p.Y,p.X);
	if (Phi < 0) Phi += 2 * PI;
	return Vector2f(Theta * INV_PI, Phi * INV_2PI);
}
