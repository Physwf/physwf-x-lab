#include "imageio.h"
#include "spectrum.h"

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

}

