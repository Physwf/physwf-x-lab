#include "Texture.h"
#include "RayTracing.h"
#include "ImageIO.h"

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

float Lanczos(float x, float tau /*= 2.f*/)
{
	x = std::abs(x);
	if (x < 1e-5f) return 1;
	if (x > 1.f) return 0;
	x *= PI;
	float s = std::sin(x * tau) / (x * tau);
	float lanczos = std::sin(x) / x;
	return s * lanczos;
}

float GammaCorrect(float value)
{
	if (value <= 0.0031308f) return 12.92f * value;
	return 1.055f * std::pow(value, (float)(1.f / 2.4f)) - 0.055f;
}

float InverseGammaCorrect(float value)
{
	if (value <= 0.04045f) return value * 1.f / 12.92f;
	return std::pow((value + 0.055f) * 1.f / 1.055f, (float)2.4f);
}

std::unique_ptr<LMSColor[]> ReadImage(const std::string& name, Vector2i* resolution)
{
	if (HasExtension(name, ".hdr"))
	{
		int Width = 0, Height = 0;
		if(ReadImageHDR(name.c_str(), NULL, &Width, &Height))
		{
			std::unique_ptr<LMSColor[]> Content = std::make_unique<LMSColor[]>(Width * Height);
			if (ReadImageHDR(name.c_str(), Content.get(), &Width, &Height))
			{
				resolution->X = Width;
				resolution->Y = Height;
				return Content;
			}
		}
	}
	return NULL;
}
