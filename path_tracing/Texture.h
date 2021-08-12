#pragma once

#include "Vector.h"
#include "Transform.h"

class SurfaceInteraction;

class TextureMapping2D
{
public:
	virtual Vector2f Map(const SurfaceInteraction& si, Vector2f* dstdx, Vector2f* dstdy) const = 0;
};

class UVMapping2D : TextureMapping2D
{
public:
	UVMapping2D(float su, float sv, float du, float dv) : su(su), sv(sv), du(du), dv(dv) {}
	virtual Vector2f Map(const SurfaceInteraction& si, Vector2f* dstdx, Vector2f* dstdy) const override;
private:
	float su, sv, du, dv;
};

class ShpereMapping2D : TextureMapping2D
{
public: 
	ShpereMapping2D(const Transform& WorldToTexture) :WorldToTexture(WorldToTexture) {}
	virtual Vector2f Map(const SurfaceInteraction& si, Vector2f* dstdx, Vector2f* dstdy) const;
private:
	Vector2f sphere(const Vector3f& p) const;
	const Transform WorldToTexture;
};

enum class ImageWrap { Repeat, Black, Clamp };

template <typename T>
class MipMap
{
public:
	MipMap(const Vector2i& resolution, const T* data, bool doTri = false, float maxAniso = 8.f, ImageWrap wrapmode = ImageWrap::Repeat);
	int Width()  const	{ return resolution[0]; }
	int Height() const	{ return resolution[1]; }

private:
	const bool doTrilinear;
	const float maxAnisotropy;
	const ImageWrap wrapMode;
	Vector2i resolution;
};

template<typename T>
class Texture
{
public:
	virtual T Evaluate(const SurfaceInteraction& si) const = 0;
	virtual ~Texture() {}
};