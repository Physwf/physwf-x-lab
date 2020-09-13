#pragma once


#include "pbrt.h"
#include "texture.h"
#include "mipmap.h"
#include "paramset.h"
#include <map>

// TexInfo Declarations
struct TexInfo {
	TexInfo(const std::string &f, bool dt, Float ma, ImageWrap wm, Float sc,
		bool gamma)
		: filename(f),
		doTrilinear(dt),
		maxAniso(ma),
		wrapMode(wm),
		scale(sc),
		gamma(gamma) {}
	std::string filename;
	bool doTrilinear;
	Float maxAniso;
	ImageWrap wrapMode;
	Float scale;
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

// ImageTexture Declarations
template <typename Tmemory, typename Treturn>
class ImageTexture : public Texture<Treturn> {
public:
	// ImageTexture Public Methods
	ImageTexture(std::unique_ptr<TextureMapping2D> m,
		const std::string &filename, bool doTri, Float maxAniso,
		ImageWrap wm, Float scale, bool gamma);
	static void ClearCache() {
		textures.erase(textures.begin(), textures.end());
	}
	Treturn Evaluate(const SurfaceInteraction &si) const {
		Vector2f dstdx, dstdy;
		Point2f st = mapping->Map(si, &dstdx, &dstdy);
		Tmemory mem = mipmap->Lookup(st, dstdx, dstdy);
		Treturn ret;
		convertOut(mem, &ret);
		return ret;
	}

private:
	// ImageTexture Private Methods
	static MIPMap<Tmemory> *GetTexture(const std::string &filename,
		bool doTrilinear, Float maxAniso,
		ImageWrap wm, Float scale, bool gamma);
	static void convertIn(const RGBSpectrum &from, RGBSpectrum *to, Float scale,
		bool gamma) {
		for (int i = 0; i < RGBSpectrum::nSamples; ++i)
			(*to)[i] = scale * (gamma ? InverseGammaCorrect(from[i]) : from[i]);
	}
	static void convertIn(const RGBSpectrum &from, Float *to, Float scale,
		bool gamma) {
		*to = scale * (gamma ? InverseGammaCorrect(from.y()) : from.y());
	}
	static void convertOut(const RGBSpectrum &from, Spectrum *to) {
		Float rgb[3];
		from.ToRGB(rgb);
		*to = Spectrum::FromRGB(rgb);
	}
	static void convertOut(Float from, Float *to) { *to = from; }

	// ImageTexture Private Data
	std::unique_ptr<TextureMapping2D> mapping;
	MIPMap<Tmemory> *mipmap;
	static std::map<TexInfo, std::unique_ptr<MIPMap<Tmemory>>> textures;
};

extern template class ImageTexture<Float, Float>;
extern template class ImageTexture<RGBSpectrum, Spectrum>;

ImageTexture<Float, Float> *CreateImageFloatTexture(const Transform &tex2world,
	const TextureParams &tp);
ImageTexture<RGBSpectrum, Spectrum> *CreateImageSpectrumTexture(
	const Transform &tex2world, const TextureParams &tp);