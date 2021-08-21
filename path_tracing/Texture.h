#pragma once

#include "Vector.h"
#include "Transform.h"
#include "Memory.h"

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <cctype>

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

float Lanczos(float, float tau = 2.f);
float GammaCorrect(float v);
float InverseGammaCorrect(float v);
inline bool HasExtension(const std::string& value, const std::string& ending) {
	if (ending.size() > value.size()) return false;
	return std::equal(
		ending.rbegin(), ending.rend(), value.rbegin(),
		[](char a, char b) { return std::tolower(a) == std::tolower(b); });
}
std::unique_ptr<LMSColor[]> ReadImage(const std::string& name, Vector2i* resolution);

enum class ImageWrap { Repeat, Black, Clamp };
struct ResampleWeight
{
	int firstTexel;
	float Weight[4];
};
template <typename T>
class MipMap
{
public:
	MipMap(const Vector2i& resolution, const T* data, bool doTri = false, float maxAniso = 8.f, ImageWrap wrapmode = ImageWrap::Repeat);
	int Width()  const	{ return resolution[0]; }
	int Height() const	{ return resolution[1]; }
	int Levels() const { return (int)pyramid.size(); }
	const T& Texel(int level, int s, int t) const;
	T LookUp(const Vector2f& st, float width = 0.f) const;
	T LookUp(const Vector2f& st, Vector2f dstdx, Vector2f dstdy) const;
private:
	std::unique_ptr<ResampleWeight[]> resampleWeights(int oldRes, int newRes)
	{
		assert(newRes >= oldRes);
		std::unique_ptr<ResampleWeight[]> wt(new ResampleWeight[newRes]);
		float filterWidth = 2.f;
		for (int i = 0; i < newRes; ++i)
		{
			float center = (i + 0.5f) * oldRes / newRes;
			wt[i].firstTexel = std::floor((center - filterWidth) + 0.5f);
			for (int j = 0; j < 4; ++i)
			{
				float pos = wt[i].firstTexel + j + 0.5f;
				wt[i].Weight[j] = Lanczos((pos - center) / filterWidth);
			}

			float invSumWts = 1 / (wt[i].Weight[0] + wt[i].Weight[1] + wt[i].Weight[2] + wt[i].Weight[3]);
			for (int j = 0; j < 4; ++j) wt[i].Weight[j] *= invSumWts;
		}
		return wt;
	}

	float clamp(float v) { return Math::Clamp(v, 0.f, std::numeric_limits<float>::infinity()); }
	LMSColor clamp(const LMSColor& v) const { return v.Clamp(0.f, std::numeric_limits<float>::infinity()); }
	T triangle(int level, const Vector2f& st) const;
private:
	const bool doTrilinear;
	const float maxAnisotropy;
	const ImageWrap wrapMode;
	Vector2i resolution;
	std::vector<std::unique_ptr<BlockArray2D<T>>> pyramid;
};

template <typename T>
T MipMap<T>::LookUp(const Vector2f& st, float width /*= 0.f*/) const
{
	float level = Levels() - 1 + Math::Log2(std::max(width, (float)(1e-8)));
	if (level < 0)
		return triangle(0, st);
	else if (level >= Levels() - 1)
		return Texel(Levels() - 1, 0, 0);
	else
	{
		int iLevel = std::floor(level);
		float delta = level - iLevel;
		return Lerp(delta, triangle(iLevel, st), triangle(iLevel + 1, st));
	}
}

template <typename T>
T MipMap<T>::LookUp(const Vector2f& st, Vector2f dstdx, Vector2f dstdy) const
{
	if (doTrilinear)
	{
		float width = std::max(
			std::max(std::abs(dstdx[0]), std::abs(dstdx[1])),
			std::max(std::abs(dstdy[0]), std::abs(dstdy[1])));
		return LookUp(st, width);
	}
}

template <typename T>
const T& MipMap<T>::Texel(int level, int s, int t) const
{
	assert(level < pyramid.size());
	const BlockArray2D<T> & l = *pyramid[level];
	switch (wrapMode)
	{
	case ImageWrap::Repeat:
		s = Math::Mod(s, l.uSize());
		t = Math::Mod(t, l.vSize());
		break;
	case ImageWrap::Clamp:
		s = Math::Clamp(s, 0, l.uSize() - 1);
		t = Math::Clamp(t, 0, l.vSize() - 1);
	case ImageWrap::Black:
		static const T black = 0.f;
		if (s < 0 || s >= (int)l.uSize() || t < 0 || t >= l.vSize()) return black;
	}
	return l(s, t);
}

template <typename T>
T MipMap<T>::triangle(int level, const Vector2f& st) const
{
	level = Math::Clamp(level, 0, Levels() - 1);
	float s = st[0] * pyramid[level]->uSize() - 0.5f;
	float t = st[1] * pyramid[level]->vSize() - 0.5f;
	int s0 = std::floor(s);
	int t0 = std::floor(t);
	float ds = s - s0;
	float dt = t - t0;
	return	(1 - ds) * (1 - dt) * Texel(level, s0, t0) +
			(1 - ds) * dt * Texel(level, s0, t0 + 1) +
			ds * (1 - dt) * Texel(level, s0 + 1, t0) +
			ds * dt * Texel(level, s0 + 1, t0 + 1);
}

template <typename T>
MipMap<T>::MipMap(const Vector2i& res, const T* data, bool doTri /*= false*/, float maxAniso /*= 8.f*/, ImageWrap wrapmode /*= ImageWrap::Repeat*/)
	: doTrilinear(doTri)
	, maxAnisotropy(maxAnisotropy)
	, wrapMode(wrapmode)
	, resolution(res)
{
	std::unique_ptr<T[]> resampledImage = nullptr;
	if (!Math::IsPowerOf2(resolution[0]) || !Math::IsPowerOf2(resolution[1]))
	{
		Vector2i resPow2(Math::RoundUpPower2(resolution[0]), Math::RoundUpPower2(resolution[1]));
		std::unique_ptr<ResampleWeight[]> sWeights = resampleWeights(resolution[0], resPow2[0]);
		resampledImage.reset(new T[resPow2[0] * resPow2[1]]);

		ParallelFor([&](size_t t) 
			{
				for (int s = 0; s < resPow2[0]; ++s)
				{
					resampledImage[t * resPow2[0] + s] = 0.f;
					for (int j = 0; j < 4; ++j)
					{
						int origS = sWeights[s].firstTexel + j;
						if (wrapMode == ImageWrap::Repeat)
							origS = origS % resolution[0];
						else if (wrapMode == ImageWrap::Clamp)
							origS = Math::Clamp(origS, 0, resolution[0] - 1);
						if (origS >= 0 && origS < (int)resolution[0])
							resampledImage[t * resPow2[0] + s] += sWeights[s].Weight[j] * data[t * resolution[0] + origS];
					}
				}
			}, resolution[1], 16);

		std::unique_ptr<ResampleWeight[]> tWeights = resampleWeights(resolution[1], resPow2[1]);
		std::vector<T*> resampleBufs;
		int nThreads = MaxThreadIndex();
		for (int i = 0; i < nThreads; ++i)
			resampleBufs.push_back(new T[resPow2[1]]);
		ParallelFor([&](size_t s)
			{
				T* workData = resampleBufs[ThreadIndex];
				for (int t = 0; t < resPow2[1]; ++t)
				{
					workData[t] = 0.f;
					for (int j = 0; j < 4; ++j)
					{
						int offset = tWeights[t].firstTexel + j;
						if (wrapMode == ImageWrap::Repeat)
							offset = offset % resolution[1];
						else if (wrapMode == ImageWrap::Clamp)
							workData[t] += tWeights[t].Weight[j] * resampledImage[offset * resPow2[0] + s];
					}
				}
				for (int t = 0; t < resPow2[1]; ++t)
					resampledImage[t*resPow2[0] + s] = clamp(workData[t]);
			}, resPow2[0], 32);
		for (auto ptr : resampleBufs) delete[] ptr;
		resolution = resPow2;
	}

	int nLevels = 1 + Math::Log2Int(std::max(resolution[0], resolution[1]));
	pyramid.resize(nLevels);

	pyramid[0].reset(new BlockArray2D<T>(resolution[0], resolution[1], resampledImage ? resampledImage.get() : data));

	for (int i = 1; i < nLevels; ++i )
	{
		int sRes = std::max(1, pyramid[i - 1]->uSize() / 2);
		int tRes = std::max(1, pyramid[i - 1]->vSize() / 2);
		pyramid[i].reset(new BlockArray2D<T>(sRes, tRes));
			
		ParallelFor([&](size_t t)
			{
				for (int s = 0; s < sRes; ++s)
				{
					(*pyramid[i])(s, (int)t) =
						0.25f * (Texel(i - 1, 2 * s,		2 * t) +
								Texel(i - 1,  2 * s + 1,	2 * t) +
								Texel(i - 1,  2 * s,		2 * t + 1) +
								Texel(i - 1,  2 * s + 1,	2 * t + 1));
				}
			}, tRes, 16);
	}
}

template<typename T>
class Texture
{
public:
	virtual T Evaluate(const SurfaceInteraction& si) const = 0;
	virtual ~Texture() {}
};

struct TexInfo
{
	TexInfo(const std::string &f, bool dt, float ma, ImageWrap wm, float sc, bool gamma)
		: filename(f),
		doTrilinear(dt),
		maxAniso(ma),
		wrapMode(wm),
		scale(sc),
		gamma(gamma) {}

	std::string filename;
	bool doTrilinear;
	float maxAniso;
	ImageWrap wrapMode;
	float scale;
	bool gamma;
	bool operator<(const TexInfo &t2) const {
		if (filename != t2.filename) return filename < t2.filename;
		if (doTrilinear != t2.doTrilinear) return doTrilinear < t2.doTrilinear;
		if (maxAniso != t2.maxAniso) return maxAniso < t2.maxAniso;
		if (scale != t2.scale) return scale < t2.scale;
		if (gamma != t2.gamma) return !gamma;
		return wrapMode < t2.wrapMode;
	}
};
template<typename Tstore, typename Treturn>
class ImageTexture : public Texture<Treturn>
{
public:
	ImageTexture(std::unique_ptr<TextureMapping2D> m, const std::string& filename, bool doTri, float maxAniso, ImageWrap wrapMode, float scale, bool gamma);
	Treturn Evaluate(const SurfaceInteraction& si) const
	{
		Vector2f dstdx, dstdy;
		Vector2f st = mapping->Map(si, &dstdx, &dstdy);
		Tstore store = mipmap->LookUp(st, dstdx, dstdy);
		Treturn ret;
		convertOut(store,&ret);
		return ret;
	}
private:
	static MipMap<Tstore>* GetTexture(const std::string& filename, bool doTrilinear, float maxAniso, ImageWrap wrapMode, float scale, bool gamma);
	static void convertIn(const LMSColor& from, LMSColor* to, float scale, bool gamma)
	{
		for (int i = 0; i < LMSColor::nSamples; ++i)
			(*to)[i] = scale * (gamma ? InverseGammaCorrect(from[i]) : from[i]);
	}
	static void convertIn(const LMSColor &from, float *to, float scale, bool gamma) 
	{
		*to = scale * (gamma ? InverseGammaCorrect(from.y()) : from.y());
	}
	static void convertOut(const LMSColor &from, LinearColor *to) {
		float rgb[3];
		from.ToRGB(rgb);
		*to = LinearColor::FromRGB(rgb);
	}
	static void convertOut(float from, float *to) { *to = from; }
	std::unique_ptr<TextureMapping2D> mapping;
	MipMap<Tstore> *mipmap;
	static std::map<TexInfo, std::unique_ptr<MipMap<Tstore>>> textures;
};

template<typename Tstore, typename Treturn>
ImageTexture<Tstore, Treturn>::ImageTexture(std::unique_ptr<TextureMapping2D> m, const std::string& filename, bool doTri, float maxAniso, ImageWrap wrapMode, float scale, bool gamma)
	: mapping(std::move(m))
{
	mipmap = GetTexture(filename, doTrilinear, maxAniso, wrapMode, scale, gamma);
}

template<typename Tstore, typename Treturn>
MipMap<Tstore>* ImageTexture<Tstore, Treturn>::GetTexture(const std::string& filename, bool doTrilinear, float maxAniso, ImageWrap wrapMode, float scale, bool gamma)
{
	TexInfo texInfo(filename, doTrilinear, maxAniso, wrapMode, scale, gamma);
	if (textures.find(texInfo) != textures.end())
		return textures[texInfo].get();

	Vector2i resolution;
	std::unique_ptr<LMSColor[]> texels = ReadImage(filename, resolution);
	if (!texels)
	{
		resolution.X = resolution.Y = 1;
		LMSColor* rgb = new LMSColor[1];
		*rgb = LMSColor(0.5f);
		texels.reset(rgb);
	}

	for (int y = 0; y < resolution.y / 2; ++y)
		for (int x = 0; x < resolution.x; ++x) {
			int o1 = y * resolution.x + x;
			int o2 = (resolution.y - 1 - y) * resolution.x + x;
			std::swap(texels[o1], texels[o2]);
		}

	MipMap<Tstore> *mipmap = nullptr;
	if (texels) {
		// Convert texels to type _Tmemory_ and create _MIPMap_
		std::unique_ptr<Tstore[]> convertedTexels(
			new Tstore[resolution.x * resolution.y]);
		for (int i = 0; i < resolution.x * resolution.y; ++i)
			convertIn(texels[i], &convertedTexels[i], scale, gamma);
		mipmap = new MipMap<Tstore>(resolution, convertedTexels.get(), doTrilinear, maxAniso, wrapMode);
	}
	else {
		// Create one-valued _MIPMap_
		Tstore oneVal = scale;
		mipmap = new MipMap<Tstore>(Vector2i(1, 1), &oneVal);
	}
	textures[texInfo].reset(mipmap);
	return mipmap;
}
