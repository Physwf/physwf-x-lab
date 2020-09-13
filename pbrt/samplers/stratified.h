#pragma once

#include "sampler.h"
#include "rng.h"


class StratifiedSampler : public PixelSampler
{
public:
	StratifiedSampler(int xPixelSamples, int yPixelSamples, bool jitterSamples, int nSampledDimentions) :PixelSampler(xPixelSamples* yPixelSamples, nSampledDimentions), xPixelSamples(xPixelSamples), yPixelSamples(yPixelSamples), jitterSamples(jitterSamples) {}
	virtual void StartPixel(const Point2i& p) override;
private:
	const int xPixelSamples, yPixelSamples;
	const bool jitterSamples;
};