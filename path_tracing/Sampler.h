#pragma once

#include "Vector.h"
#include "RNG.h"

#include <memory>
#include <vector>

class SampleGenerator
{
public:
	SampleGenerator(int64_t nSamples) {}
	virtual float Get1D(int64_t i) = 0;
	virtual Vector2f Get2D(int64_t index) = 0;
	virtual std::unique_ptr<SampleGenerator> Clone(int Seed) = 0;
protected:
};

class StratifiedSampleGenerator : public SampleGenerator
{
public:
	StratifiedSampleGenerator(int64_t xPixelSamples, int64_t yPixelSamples)
		: SampleGenerator(xPixelSamples* yPixelSamples) 
		, samples1D(xPixelSamples* yPixelSamples)
		, samples2D(xPixelSamples* yPixelSamples)
	{
		StratifiedSample1D(&samples1D[0], xPixelSamples * yPixelSamples, rng);
		StratifiedSample2D(&samples2D[0], xPixelSamples , yPixelSamples, rng);
	}
	
	virtual float Get1D(int64_t i) override;
	virtual Vector2f Get2D(int64_t i) override;

	virtual std::unique_ptr<SampleGenerator> Clone(int Seed);
private:
	void StratifiedSample1D(float* samples, int nSamples, RNG& rng, bool bJitter = true);
	void StratifiedSample2D(Vector2f* samples, int nx, int ny, RNG& rng, bool bJitter = true);
private:
	std::vector<float> samples1D;
	std::vector<Vector2f> samples2D;
	RNG rng;
};

class PixelSampler
{
public:
	static std::unique_ptr<PixelSampler> CreateStratified(int64_t xPixelSamples, int64_t yPixelSamples);

	virtual void StartPixel(const Vector2i& p);
	Vector2f GetPixelSample(const Vector2i& pRaster);
	virtual bool StartNextSample();
	virtual std::unique_ptr<PixelSampler> Clone(int Seed);
private:
	PixelSampler(std::unique_ptr<SampleGenerator> InGenerator, int64_t samplesPerPixel)
		: Generator(std::move(InGenerator))
		, samplesPerPixel(samplesPerPixel)
	{}
private:
	Vector2i currentPixel;
	int64_t currentPixelSampleIndex;

	const int64_t samplesPerPixel;

	std::unique_ptr<SampleGenerator> Generator;
};