#pragma once

#include "Vector.h"
#include "RNG.h"
#include "Sampling.h"

#include <memory>
#include <vector>

class SampleGenerator1D
{
public:
	virtual ~SampleGenerator1D() {};
	virtual float Get(int64_t index) = 0;
};

class SampleGenerator2D
{
public:
	virtual ~SampleGenerator2D() {};
	virtual Vector2f Get(int64_t index) = 0;
};

class SampleArrayGenerator1D
{
public:
	SampleArrayGenerator1D(int n, int64_t samplesPerPixel) : sampleArray1D(n* samplesPerPixel) {}
	const float* GetArray(int n, int64_t index)
	{
		return &sampleArray1D[index * n];
	}
protected:
	std::vector<float> sampleArray1D;
};

class SampleArrayGenerator2D
{
public:
	SampleArrayGenerator2D(int n, int64_t samplesPerPixel) : sampleArray2D(n* samplesPerPixel) {}
	const Vector2f* GetArray(int n, int64_t index)
	{
		return &sampleArray2D[index * n];
	}
protected:
	std::vector<Vector2f> sampleArray2D;
};

class StratifiedSampleGenerator1D : public SampleGenerator1D
{
public:
	StratifiedSampleGenerator1D(int64_t nSamples)
		: samples1D(nSamples) { }

	void Generate(int64_t nSamples, RNG& rng)
	{
		StratifiedSample1D(&samples1D[0], nSamples, rng);
		Shuffle(&samples1D[0], nSamples, 1, rng);
	}
	virtual float Get(int64_t index) override
	{
		return samples1D[index];
	}
private:
	std::vector<float> samples1D;
};

class StratifiedSampleGenerator2D : public SampleGenerator2D
{
public:
	StratifiedSampleGenerator2D(int64_t xSamples, int64_t ySamples)
		: samples2D(xSamples* ySamples) {}

	void Generate(int64_t xSamples, int64_t ySamples, RNG& rng)
	{
		StratifiedSample2D(&samples2D[0], xSamples, ySamples, rng);
		Shuffle(&samples2D[0], xSamples * ySamples, 1, rng);
	}
	virtual Vector2f Get(int64_t index) override
	{
		return samples2D[index];
	}
private:
	std::vector<Vector2f> samples2D;
};

class StratifiedSampleArrayGenerator1D : public SampleArrayGenerator1D
{
public:
	StratifiedSampleArrayGenerator1D(int64_t nSamples, int count)
		: SampleArrayGenerator1D(count, nSamples) { }

	void Generate(int64_t nSamples, int count, RNG& rng)
	{
		for (int64_t j = 0; j < nSamples; ++j)
		{
			StratifiedSample1D(&sampleArray1D[j * count], count, rng);
		}
	}
};

class LatinHypercubeSampleArrayGenerator2D : public SampleArrayGenerator2D
{
public:
	LatinHypercubeSampleArrayGenerator2D(int64_t nSamples, int count)
		: SampleArrayGenerator2D(count, nSamples) { }

	void Generate(int64_t nSamples, int count, RNG& rng)
	{
		for (int64_t j = 0; j < nSamples; ++j)
		{
			LatinHypercube(&sampleArray2D[j * count].X, count, 2, rng);
		}
	}
};

class SamplerContext
{
public:
	virtual ~SamplerContext() {}
	virtual void Generate() = 0;
	virtual void StartNextSample() = 0;
	virtual Vector2f GetPixel(int64_t index) = 0;
	virtual float Get1D(int64_t index) = 0;
	virtual Vector2f Get2D(int64_t index) = 0;
	virtual std::unique_ptr<SamplerContext> Clone() = 0;

	RNG rng;
};

class StratifiedSamplerContext : public SamplerContext
{
public:
	StratifiedSamplerContext(int64_t xPixelSamples, int64_t yPixelSamples, int nSampledDimensions)
		: SamplerContext()
		, xPixelSamples(xPixelSamples)
		, yPixelSamples(yPixelSamples)
	{
		PixelGenerator = std::make_shared<StratifiedSampleGenerator2D>(xPixelSamples, yPixelSamples);
		for (int i = 0; i < nSampledDimensions; ++i) 
		{
			Generator1Ds.push_back(std::make_shared<StratifiedSampleGenerator1D>(xPixelSamples * yPixelSamples));
			Generator2Ds.push_back(std::make_shared<StratifiedSampleGenerator2D>(xPixelSamples , yPixelSamples));
		}
	}

	virtual void Generate() override
	{
		PixelGenerator->Generate(xPixelSamples, yPixelSamples, rng);
		for (size_t i = 0; i < Generator1Ds.size(); ++i)
		{
			Generator1Ds[i]->Generate(xPixelSamples * yPixelSamples, rng);
		}
		for (size_t i = 0; i < Generator2Ds.size(); ++i)
		{
			Generator2Ds[i]->Generate(xPixelSamples , yPixelSamples, rng);
		}
		
	}
	virtual void StartNextSample() override
	{
		current1DDimension = 0;
		current2DDimension = 0;
	}
	virtual Vector2f GetPixel(int64_t index) override
	{
		return PixelGenerator->Get(index);
	}
	virtual float Get1D(int64_t index) override
	{
		if (current1DDimension < Generator1Ds.size())
			return Generator1Ds[current1DDimension++]->Get(index);
		else 
			return rng.NextFloat();
	}
	virtual Vector2f Get2D(int64_t index) override
	{
		if (current2DDimension < Generator2Ds.size())
			return Generator2Ds[current2DDimension++]->Get(index);
		else
			return Vector2f(rng.NextFloat(),rng.NextFloat());
	}

	virtual std::unique_ptr<SamplerContext> Clone() override
	{
		return std::unique_ptr<SamplerContext>(new StratifiedSamplerContext(*this));
	}
private:
	std::shared_ptr<StratifiedSampleGenerator2D> PixelGenerator;
	std::vector<std::shared_ptr<StratifiedSampleGenerator1D>> Generator1Ds;
	std::vector<std::shared_ptr<StratifiedSampleGenerator2D>> Generator2Ds;
	std::vector<std::shared_ptr<StratifiedSampleArrayGenerator1D>> ArrayGenerator1Ds;
	std::vector<std::shared_ptr<LatinHypercubeSampleArrayGenerator2D>> ArrayGenerator2Ds;
	
	const int64_t xPixelSamples, yPixelSamples;
	int current1DDimension = 0, current2DDimension = 0;
};

class Sampler
{
public:
	static std::shared_ptr<Sampler> CreateStratified(int64_t xPixelSamples, int64_t yPixelSamples, int nSampledDimensions);
	
	float Get1D();
	Vector2f Get2D();

	virtual void StartPixel(const Vector2i& p);
	Vector2f GetPixelSample(const Vector2i& pRaster);
	virtual bool StartNextSample();
	virtual std::unique_ptr<Sampler> Clone(int Seed);
	
	Sampler(int64_t samplesPerPixel, std::unique_ptr<SamplerContext> context)
		: currentPixel(0,0)
		, currentPixelSampleIndex(0)
		, samplesPerPixel(samplesPerPixel)
		, context(std::move(context))
	{}

	Sampler(const Sampler& rhs) 
		: currentPixel(0, 0)
		, currentPixelSampleIndex(0)
		, samplesPerPixel(rhs.samplesPerPixel)
	{
		context = rhs.context->Clone();
	}
private:

	Vector2i currentPixel;
	int64_t currentPixelSampleIndex;

	const int64_t samplesPerPixel;

	std::unique_ptr<SamplerContext> context;
};