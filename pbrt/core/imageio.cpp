#include "imageio.h"
#include "spectrum.h"
#include "ext/lodepng.h"
#include "fileutil.h"

std::unique_ptr<RGBSpectrum[]> ReadImage(const std::string &name, Point2i *resolution)
{
	return nullptr;
}

RGBSpectrum * ReadImageEXR(const std::string &name, int *width, int *height, Bounds2i *dataWindow /*= nullptr*/, Bounds2i *displayWindow /*= nullptr*/)
{
	return nullptr;
}

void WriteImage(const std::string &name, const Float *rgb, const Bounds2i &outputBounds, const Point2i &totalResolution)
{
	if (HasExtension(name, ".tga") || HasExtension(name, ".png"))
	{
		// 8-bit formats; apply gamma
		Vector2i resolution = outputBounds.Diagonal();
		std::unique_ptr<uint8_t[]> rgb8(new uint8_t[3 * resolution.x * resolution.y]);
		uint8_t *dst = rgb8.get();
		for (int y = 0; y < resolution.y; ++y) {
			for (int x = 0; x < resolution.x; ++x) {
#define TO_BYTE(v) (uint8_t) Clamp(255.f * GammaCorrect(v) + 0.5f, 0.f, 255.f)
				dst[0] = TO_BYTE(rgb[3 * (y * resolution.x + x) + 0]);
				dst[1] = TO_BYTE(rgb[3 * (y * resolution.x + x) + 1]);
				dst[2] = TO_BYTE(rgb[3 * (y * resolution.x + x) + 2]);
#undef TO_BYTE
				if (dst[0] == 255 && dst[1] == 255 && dst[2] == 255)
				{
					printf("r:%f,g:%f,b:%f\n", rgb[3 * (y * resolution.x + x) + 0], rgb[3 * (y * resolution.x + x) + 1], rgb[3 * (y * resolution.x + x) + 2]);
					printf("r:%d,g:%d,b:%d\n", dst[0], dst[1], dst[2]);
				}

				dst += 3;

			}
		}
		if (HasExtension(name, ".tga"))
		{

		}
		else
		{
			unsigned int error = lodepng_encode24_file(name.c_str(), rgb8.get(), resolution.x, resolution.y);
			if (error != 0)
				Error("Error writing PNG \"%s\": %s", name.c_str(),
					lodepng_error_text(error));
		}
	}
}

