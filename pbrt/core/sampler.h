#pragma once

#include "pbrt.h"
#include "geometry.h"
#include "rng.h"


class Sampler
{
public:
	Sampler(int64_t samplesPerPixel) : samplesPerPixel(samplesPerPixel) {}

	virtual void StartPixel(const Point2i& p);
	virtual Float Get1D() = 0;
	virtual Point2f Get2D() = 0;
	CameraSample GetCameraSample(const Point2i& pRaster);
	void Requeset1DArray(int n);
	void Requeset2DArray(int n);
	const int64_t samplesPerPixel;
	virtual int RoundCount(int n) const { return n; }
	const Float* Get1DArray(int n);
	const Point2f* Get2DArray(int n);
	virtual bool StartNextSample();
	virtual std::unique_ptr<Sampler> Clone(int seed) = 0;
	virtual bool SetSamplerNumber(int64_t sampleNum);
protected:
	Point2i currentPixel;
	int64_t currentPixelSampleIndex;
	std::vector<int> samples1DArraySizes, samples2DArraySizes;
	std::vector<std::vector<Float>> samplesArray1D;
	std::vector<std::vector<Point2f>> samplesArray2D;
 private:
	 size_t array1DOffset, array2DOffset;
};

class PixelSampler : public Sampler
{
public:
	PixelSampler(int64_t samplesPerPixel, int nSampledDeimisions);
	virtual bool StartNextSample();
	virtual bool SetSampleNumber(int64_t sampleNum);
	virtual Float Get1D();
	virtual Point2f Get2D();
protected:
	std::vector<std::vector<Float>> samples1D;
	std::vector<std::vector<Point2f>> samples2D;
	int current1DDimension = 0, current2DDimension = 0;
	RNG rng;
};

class GlobalSampler : public Sampler
{
public:
	GlobalSampler(int64_t samplesPerPixel) : Sampler(samplesPerPixel) {}
	virtual int64_t GetIndexForSample(int64_t sampleNum) const = 0;
	virtual Float SampleDimension(int64_t index, int demension) const = 0;
	virtual void StartPixel(const Point2i& p) override;
	virtual bool StartNextSample() override;
	virtual bool SetSamplerNumber(int64_t sampleNum) override;
	virtual Float Get1D() override;
	virtual Point2f Get2D() override;
private:
	int dimension;
	int64_t intervalSampleIndex;
	static const int arrayStartDim = 5;
	int arrayEndDim;
};

