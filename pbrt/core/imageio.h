#pragma once

#include "pbrt.h"
#include "geometry.h"
#include <cctype>

// ImageIO Declarations
std::unique_ptr<RGBSpectrum[]> ReadImage(const std::string &name,
	Point2i *resolution);
RGBSpectrum *ReadImageEXR(const std::string &name, int *width,
	int *height, Bounds2i *dataWindow = nullptr,
	Bounds2i *displayWindow = nullptr);

void WriteImage(const std::string &name, const Float *rgb,
	const Bounds2i &outputBounds, const Point2i &totalResolution);