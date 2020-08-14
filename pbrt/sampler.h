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
	virtual bool StartNextPixel();
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
protected:

};