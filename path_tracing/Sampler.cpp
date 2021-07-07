#include "Sampler.h"

float StratifiedSampleGenerator::Get1D(int64_t i)
{
	return samples1D[i];
}

Vector2f StratifiedSampleGenerator::Get2D(int64_t i)
{
	return samples2D[i];
}

std::unique_ptr<SampleGenerator> StratifiedSampleGenerator::Clone(int iSeed)
{
	std::unique_ptr<StratifiedSampleGenerator> gen = std::make_unique<StratifiedSampleGenerator>(*this);
	gen->rng.Seed(iSeed);
	return gen;
}

void StratifiedSampleGenerator::StratifiedSample1D(float* samples, int nSamples, RNG& rng, bool bJitter /*= true*/)
{
	float invNSamples = 1.f / nSamples;
	for (int i = 0; i < nSamples; ++i)
	{
		float delta = bJitter ? rng.NextFloat() : 0.5f;
		samples[i] = std::min((i + delta) * invNSamples, FloatOneMinusEpsilon);
	}
}

void StratifiedSampleGenerator::StratifiedSample2D(Vector2f* samples, int nx, int ny, RNG& rng, bool bJitter /*= true*/)
{
	float dx = 1.f / nx;
	float dy = 1.f / ny;
	for (int y = 0; y < ny; ++y)
	{
		for (int x = 0; x < nx; ++x)
		{
			float jx = bJitter ? rng.NextFloat() : 0.5f;
			float jy = bJitter ? rng.NextFloat() : 0.5f;
			samples->X = std::min((x + jx) * dx, FloatOneMinusEpsilon);
			samples->Y = std::min((y + jy) * dy, FloatOneMinusEpsilon);
			++samples;
		}
	}
}

std::unique_ptr<PixelSampler> PixelSampler::CreateStratified(int64_t xPixelSamples, int64_t yPixelSamples)
{
	std::unique_ptr<StratifiedSampleGenerator> Generator = std::make_unique<StratifiedSampleGenerator>(xPixelSamples, yPixelSamples);
	std::unique_ptr<PixelSampler> sampler = std::make_unique<PixelSampler>(Generator, xPixelSamples * yPixelSamples);
	return sampler;
}

void PixelSampler::StartPixel(const Vector2i& p)
{
	currentPixel = p;
	currentPixelSampleIndex = 0;
}

Vector2f PixelSampler::GetPixelSample(const Vector2i& pRaster)
{
	return Generator->Get2D(currentPixelSampleIndex);
}

bool PixelSampler::StartNextSample()
{
	return ++currentPixelSampleIndex < samplesPerPixel;
}

std::unique_ptr<PixelSampler> PixelSampler::Clone(int iSeed)
{
	std::unique_ptr<SampleGenerator> gen = Generator->Clone(iSeed);
	std::unique_ptr<PixelSampler> sampler = std::make_unique<PixelSampler>(gen,samplesPerPixel);
	return sampler;
}
