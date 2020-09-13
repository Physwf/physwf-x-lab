#pragma once

#include "sampler.h"
#include "lowdiscrepancy.h"

class HaltonSampler : public GlobalSampler
{
public:
	HaltonSampler(int samplesPerPixel, const Bounds2i& sampleBounds,bool sampleAtCenter);
	virtual int64_t GetIndexForSample(int64_t sampleNum) const override;
	virtual Float SampleDimension(int64_t index, int demension) const override;
	std::unique_ptr<Sampler> Clone(int seed);
private:
	const uint16_t* PermutationForDimention(int dim) const;
private:
	static std::vector<uint16_t> radicalInversePermutations;
	Point2i baseScales, baseExponents;
	int sampleStride;
	int multInverse[2];
	mutable Point2i pixelForOffset = Point2i(std::numeric_limits<int>::max(), std::numeric_limits<int>::max());
	mutable int64_t offsetForCurrentPixel;
};

HaltonSampler *CreateHaltonSampler(const ParamSet &params,
	const Bounds2i &sampleBounds);