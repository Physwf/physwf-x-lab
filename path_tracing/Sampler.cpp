#include "Sampler.h"

std::shared_ptr<Sampler> Sampler::CreateStratified(int64_t xPixelSamples, int64_t yPixelSamples, int nSampledDimensions)
{
	std::unique_ptr<SamplerContext> context = std::make_unique<StratifiedSamplerContext>(xPixelSamples, yPixelSamples, nSampledDimensions);
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

float Sampler::Get1D()
{
	context->Get1D(currentPixelSampleIndex);
}

Vector2f Sampler::Get2D()
{
	context->Get2D(currentPixelSampleIndex);
}

