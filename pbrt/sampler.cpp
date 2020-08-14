#include "sampler.h"

void Sampler::StartPixel(const Point2i& p)
{
	currentPixel = p;
	currentPixelSampleIndex = 0;
	array1DOffset = array2DOffset = 0;
}

void Sampler::Requeset1DArray(int n)
{
	samples1DArraySizes.push_back(n);
	samplesArray1D.push_back(std::vector<Float>(n*samplesPerPixel));
}

void Sampler::Requeset2DArray(int n)
{
	samples2DArraySizes.push_back(n);
	samplesArray2D.push_back(std::vector<Point2f>(n*samplesPerPixel));
}

const Float* Sampler::Get1DArray(int n)
{
	if (array1DOffset == samplesArray1D.size())
		return nullptr;
	return &samplesArray1D[array1DOffset++][currentPixelSampleIndex*n];
}

const Point2f* Sampler::Get2DArray(int n)
{
	if (array2DOffset == samplesArray2D.size())
		return nullptr;
	return &samplesArray2D[array2DOffset++][currentPixelSampleIndex*n];
}

bool Sampler::StartNextPixel()
{
	array1DOffset = array2DOffset = 0;
	return ++currentPixelSampleIndex < samplesPerPixel;
}

bool Sampler::SetSamplerNumber(int64_t sampleNum)
{
	currentPixelSampleIndex = sampleNum;
	return currentPixelSampleIndex > samplesPerPixel;
}
