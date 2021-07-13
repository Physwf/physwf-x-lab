#include "Sampler.h"

std::shared_ptr<Sampler> Sampler::CreateStratified(int64_t xPixelSamples, int64_t yPixelSamples, int nSampledDimensions)
{
	std::shared_ptr<SamplerContext> context = std::make_shared<StratifiedSamplerContext>(xPixelSamples, yPixelSamples, nSampledDimensions);
	return std::make_shared<Sampler>(xPixelSamples * yPixelSamples, context);
}

void Sampler::StartPixel(const Vector2i& p)
{
	currentPixel = p;
	currentPixelSampleIndex = 0;
	context->Generate();
}

Vector2f Sampler::GetPixelSample(const Vector2i& pRaster)
{
	return (Vector2f)pRaster + context->GetPixel(currentPixelSampleIndex);
}

bool Sampler::StartNextSample()
{
	return ++currentPixelSampleIndex < samplesPerPixel;
}

std::unique_ptr<Sampler> Sampler::Clone(int s)
{
	Sampler* sampler = new Sampler(*this);
	sampler->context->rng.Seed(s);
	return std::unique_ptr<Sampler>(sampler);
}

float Sampler::Get1D()
{
	return context->Get1D(currentPixelSampleIndex);
}

Vector2f Sampler::Get2D()
{
	return context->Get2D(currentPixelSampleIndex);
}

