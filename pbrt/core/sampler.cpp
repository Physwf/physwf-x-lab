#include "sampler.h"
#include "sampling.h"
#include "camera.h"

void Sampler::StartPixel(const Point2i& p)
{
	currentPixel = p;
	currentPixelSampleIndex = 0;
	array1DOffset = array2DOffset = 0;
}

CameraSample Sampler::GetCameraSample(const Point2i& pRaster)
{
	CameraSample cs;
	cs.pFilm = (Point2f)pRaster + Get2D();
	cs.time = Get1D();
	cs.pLens = Get2D();
	return cs;
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

bool Sampler::StartNextSample()
{
	array1DOffset = array2DOffset = 0;
	return ++currentPixelSampleIndex < samplesPerPixel;
}

bool Sampler::SetSamplerNumber(int64_t sampleNum)
{
	currentPixelSampleIndex = sampleNum;
	return currentPixelSampleIndex > samplesPerPixel;
}

PixelSampler::PixelSampler(int64_t samplesPerPixel, int nSampledDeimisions) : Sampler(samplesPerPixel)
{
	for (int i = 0; i < nSampledDeimisions; ++i)
	{
		samples1D.push_back(std::vector<Float>(samplesPerPixel));
		samples2D.push_back(std::vector<Point2f>(samplesPerPixel));
	}
}

bool PixelSampler::StartNextSample()
{
	current1DDimension = current2DDimension = 0;
	return Sampler::StartNextSample();
}

bool PixelSampler::SetSampleNumber(int64_t sampleNum)
{
	current1DDimension = current2DDimension = 0;
	return Sampler::SetSamplerNumber(sampleNum);
}

Float PixelSampler::Get1D()
{
	if (current1DDimension < (int)samples1D.size())
		return samples1D[current1DDimension++][currentPixelSampleIndex];
	else
		return rng.UniformFloat();
}

Point2f PixelSampler::Get2D()
{
	if (current2DDimension < (int)samples2D.size())
		return samples2D[current2DDimension++][currentPixelSampleIndex];
	else
		return Point2f(rng.UniformFloat(), rng.UniformFloat());
}

void GlobalSampler::StartPixel(const Point2i& p)
{
	Sampler::StartPixel(p);
	dimension = 0;
	intervalSampleIndex = GetIndexForSample(0);
	arrayEndDim = arrayStartDim + samplesArray1D.size() + 2 * samplesArray2D.size();
	for (size_t i = 0; i < samples1DArraySizes.size(); ++i)
	{
		int nSamples = samples1DArraySizes[i] * samplesPerPixel;
		for (int j = 0; j < nSamples; ++j)
		{
			int64_t index = GetIndexForSample(j);
			samplesArray1D[i][j] = SampleDimension(index, arrayStartDim + i);
		}
	}

	int dim = arrayStartDim + samples1DArraySizes.size();
	for (size_t i = 0; i < samples2DArraySizes.size(); ++i)
	{
		int nSamples = samples2DArraySizes[i] * samplesPerPixel;
		for (int j = 0; j < nSamples; ++j)
		{
			int64_t index = GetIndexForSample(j);
			samplesArray2D[i][j].x = SampleDimension(index, dim);
			samplesArray2D[i][j].y = SampleDimension(index, dim + 1);
		}
		dim += 2;
	}
}

bool GlobalSampler::StartNextSample()
{
	dimension = 0;
	intervalSampleIndex = GetIndexForSample(currentPixelSampleIndex + 1);
	return Sampler::StartNextSample();
}

bool GlobalSampler::SetSamplerNumber(int64_t sampleNum)
{
	dimension = 0;
	intervalSampleIndex = GetIndexForSample(sampleNum);
	return Sampler::SetSamplerNumber(sampleNum);
}

Float GlobalSampler::Get1D()
{
	if (dimension >= arrayStartDim && dimension < arrayEndDim)
		dimension = arrayEndDim;
	return SampleDimension(intervalSampleIndex, dimension++);
}

Point2f GlobalSampler::Get2D()
{
	if (dimension + 1 >= arrayStartDim && dimension < arrayEndDim)
		dimension = arrayEndDim;
	Point2f p(SampleDimension(intervalSampleIndex, dimension), SampleDimension(intervalSampleIndex, dimension + 1));
	dimension += 2;
	//printf("Get2D=%f,%f\n", p.x, p.y);
	return p;
}
