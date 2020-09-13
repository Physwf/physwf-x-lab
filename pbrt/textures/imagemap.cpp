#include "imagemap.h"
#include "imageio.h"

// ImageTexture Method Definitions
template <typename Tmemory, typename Treturn>
ImageTexture<Tmemory, Treturn>::ImageTexture(
	std::unique_ptr<TextureMapping2D> mapping, const std::string &filename,
	bool doTrilinear, Float maxAniso, ImageWrap wrapMode, Float scale,
	bool gamma)
	: mapping(std::move(mapping)) {
	mipmap =
		GetTexture(filename, doTrilinear, maxAniso, wrapMode, scale, gamma);
}

template <typename Tmemory, typename Treturn>
MIPMap<Tmemory> *ImageTexture<Tmemory, Treturn>::GetTexture(
	const std::string &filename, bool doTrilinear, Float maxAniso,
	ImageWrap wrap, Float scale, bool gamma) {
	// Return _MIPMap_ from texture cache if present
	TexInfo texInfo(filename, doTrilinear, maxAniso, wrap, scale, gamma);
	if (textures.find(texInfo) != textures.end())
		return textures[texInfo].get();

	// Create _MIPMap_ for _filename_
	//ProfilePhase _(Prof::TextureLoading);
	Point2i resolution;
	std::unique_ptr<RGBSpectrum[]> texels = ReadImage(filename, &resolution);
	if (!texels) {
		Warning("Creating a constant grey texture to replace \"%s\".",
			filename.c_str());
		resolution.x = resolution.y = 1;
		RGBSpectrum *rgb = new RGBSpectrum[1];
		*rgb = RGBSpectrum(0.5f);
		texels.reset(rgb);
	}

	// Flip image in y; texture coordinate space has (0,0) at the lower
	// left corner.
	for (int y = 0; y < resolution.y / 2; ++y)
		for (int x = 0; x < resolution.x; ++x) {
			int o1 = y * resolution.x + x;
			int o2 = (resolution.y - 1 - y) * resolution.x + x;
			std::swap(texels[o1], texels[o2]);
		}

	MIPMap<Tmemory> *mipmap = nullptr;
	if (texels) {
		// Convert texels to type _Tmemory_ and create _MIPMap_
		std::unique_ptr<Tmemory[]> convertedTexels(
			new Tmemory[resolution.x * resolution.y]);
		for (int i = 0; i < resolution.x * resolution.y; ++i)
			convertIn(texels[i], &convertedTexels[i], scale, gamma);
		mipmap = new MIPMap<Tmemory>(resolution, convertedTexels.get(),
			doTrilinear, maxAniso, wrap);
	}
	else {
		// Create one-valued _MIPMap_
		Tmemory oneVal = scale;
		mipmap = new MIPMap<Tmemory>(Point2i(1, 1), &oneVal);
	}
	textures[texInfo].reset(mipmap);
	return mipmap;
}

template <typename Tmemory, typename Treturn>
std::map<TexInfo, std::unique_ptr<MIPMap<Tmemory>>>
ImageTexture<Tmemory, Treturn>::textures;
ImageTexture<Float, Float> *CreateImageFloatTexture(const Transform &tex2world,
	const TextureParams &tp) {
	// Initialize 2D texture mapping _map_ from _tp_
	std::unique_ptr<TextureMapping2D> map;
	std::string type = tp.FindString("mapping", "uv");
	if (type == "uv") {
		Float su = tp.FindFloat("uscale", 1.);
		Float sv = tp.FindFloat("vscale", 1.);
		Float du = tp.FindFloat("udelta", 0.);
		Float dv = tp.FindFloat("vdelta", 0.);
		map.reset(new UVMapping2D(su, sv, du, dv));
	}
	else if (type == "spherical")
		map.reset(new SphericalMapping2D(Inverse(tex2world)));
	else if (type == "cylindrical")
		map.reset(new CylindricalMapping2D(Inverse(tex2world)));
	else if (type == "planar")
		map.reset(new PlanerMapping2D(tp.FindVector3f("v1", Vector3f(1, 0, 0)),
			tp.FindVector3f("v2", Vector3f(0, 1, 0)),
			tp.FindFloat("udelta", 0.f),
			tp.FindFloat("vdelta", 0.f)));
	else {
		Error("2D texture mapping \"%s\" unknown", type.c_str());
		map.reset(new UVMapping2D);
	}

	// Initialize _ImageTexture_ parameters
	Float maxAniso = tp.FindFloat("maxanisotropy", 8.f);
	bool trilerp = tp.FindBool("trilinear", false);
	std::string wrap = tp.FindString("wrap", "repeat");
	ImageWrap wrapMode = ImageWrap::Repeat;
	if (wrap == "black")
		wrapMode = ImageWrap::Black;
	else if (wrap == "clamp")
		wrapMode = ImageWrap::Clamp;
	Float scale = tp.FindFloat("scale", 1.f);
	std::string filename = tp.FindFilename("filename");
	bool gamma = tp.FindBool("gamma", HasExtension(filename, ".tga") ||
		HasExtension(filename, ".png"));
	return new ImageTexture<Float, Float>(std::move(map), filename, trilerp,
		maxAniso, wrapMode, scale, gamma);
}

ImageTexture<RGBSpectrum, Spectrum> *CreateImageSpectrumTexture(
	const Transform &tex2world, const TextureParams &tp) {
	// Initialize 2D texture mapping _map_ from _tp_
	std::unique_ptr<TextureMapping2D> map;
	std::string type = tp.FindString("mapping", "uv");
	if (type == "uv") {
		Float su = tp.FindFloat("uscale", 1.);
		Float sv = tp.FindFloat("vscale", 1.);
		Float du = tp.FindFloat("udelta", 0.);
		Float dv = tp.FindFloat("vdelta", 0.);
		map.reset(new UVMapping2D(su, sv, du, dv));
	}
	else if (type == "spherical")
		map.reset(new SphericalMapping2D(Inverse(tex2world)));
	else if (type == "cylindrical")
		map.reset(new CylindricalMapping2D(Inverse(tex2world)));
	else if (type == "planar")
		map.reset(new PlanerMapping2D(tp.FindVector3f("v1", Vector3f(1, 0, 0)),
			tp.FindVector3f("v2", Vector3f(0, 1, 0)),
			tp.FindFloat("udelta", 0.f),
			tp.FindFloat("vdelta", 0.f)));
	else {
		Error("2D texture mapping \"%s\" unknown", type.c_str());
		map.reset(new UVMapping2D);
	}

	// Initialize _ImageTexture_ parameters
	Float maxAniso = tp.FindFloat("maxanisotropy", 8.f);
	bool trilerp = tp.FindBool("trilinear", false);
	std::string wrap = tp.FindString("wrap", "repeat");
	ImageWrap wrapMode = ImageWrap::Repeat;
	if (wrap == "black")
		wrapMode = ImageWrap::Black;
	else if (wrap == "clamp")
		wrapMode = ImageWrap::Clamp;
	Float scale = tp.FindFloat("scale", 1.f);
	std::string filename = tp.FindFilename("filename");
	bool gamma = tp.FindBool("gamma", HasExtension(filename, ".tga") ||
		HasExtension(filename, ".png"));
	return new ImageTexture<RGBSpectrum, Spectrum>(
		std::move(map), filename, trilerp, maxAniso, wrapMode, scale, gamma);
}

template class ImageTexture<Float, Float>;
template class ImageTexture<RGBSpectrum, Spectrum>;