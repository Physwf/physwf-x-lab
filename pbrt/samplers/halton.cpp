#include "halton.h"
#include "paramset.h"
#include "rng.h"

static constexpr int kMaxResolution = 128;
static void extendedGCD(uint64_t a, uint64_t b, int64_t *x, int64_t *y);
static uint64_t multiplicativeInverse(int64_t a, int64_t n)
{
	int64_t x, y;
	extendedGCD(a, n, &x, &y);
	return Mod(x, n);
}
//欧几里得最大公约数算法推广,求出满足d=gcd(a,b)=ax+by的整系数x,y
static void extendedGCD(uint64_t a, uint64_t b, int64_t *x, int64_t *y)
{
	if (b == 0)
	{
		*x = 1;
		*y = 0;
		return;
	}

	int64_t d = a / b, xp, yp;
	extendedGCD(b, a%b, &xp, &yp);
	*x = yp;
	*y = xp - (d*yp);
}



HaltonSampler::HaltonSampler(int samplesPerPixel, const Bounds2i& sampleBounds, bool sampleAtCenter):GlobalSampler(samplesPerPixel)
{
	if (radicalInversePermutations.size() == 0)
	{
		RNG rng;
		radicalInversePermutations = ComputeRadicalInversePermutations(rng);
	}

	Vector2i res = sampleBounds.pMax - sampleBounds.pMin;
	for (int i = 0; i < 2; ++i)
	{
		int base = (i == 0) ? 2 : 3;
		int scale = 1, exp = 0;
		while (scale < std::min(res[i], kMaxResolution))
		{
			scale *= base;
			++exp;
		}
		baseScales[i] = scale;
		baseExponents[i] = exp;
	}
	sampleStride = baseScales[0] * baseScales[1];

	multInverse[0] = multiplicativeInverse(baseScales[1], baseScales[0]);
	multInverse[1] = multiplicativeInverse(baseScales[0], baseScales[1]);
}
std::vector<uint16_t> HaltonSampler::radicalInversePermutations;
int64_t HaltonSampler::GetIndexForSample(int64_t sampleNum) const
{
	if (currentPixel != pixelForOffset)
	{
		offsetForCurrentPixel = 0;
		if (sampleStride > 1)
		{
			Point2i pm(Mod(currentPixel[0], kMaxResolution), Mod(currentPixel[1], kMaxResolution));
			for (int i = 0; i < 2; ++i)
			{
				uint64_t dimOffset = (i == 0) ? InverseRadicalInverse<2>(pm[i], baseExponents[i]) : InverseRadicalInverse<3>(pm[i], baseExponents[i]);
				offsetForCurrentPixel += dimOffset * (sampleStride / baseScales[i] * multInverse[i]);
			}
			offsetForCurrentPixel %= sampleStride;
		}
		pixelForOffset = currentPixel;
	}
	return offsetForCurrentPixel + sampleNum * sampleStride;
}

Float HaltonSampler::SampleDimension(int64_t index, int dimension) const
{
	if (dimension == 0)
	{
		return RadicalInverse(dimension, index >> baseExponents[0]);
	}
	else if (dimension == 1)
	{
		return RadicalInverse(dimension, index / baseScales[1]);
	}
	else
	{
		return ScrambledRadicalInverse(dimension, index, PermutationForDimention(dimension));
	}
}

std::unique_ptr<Sampler> HaltonSampler::Clone(int seed)
{
	return std::unique_ptr<Sampler>(new HaltonSampler(*this));
}

const uint16_t* HaltonSampler::PermutationForDimention(int dim) const
{
	//if (dim > PrimeTableSize) Severe("");
	return &radicalInversePermutations[PrimeSums[dim]];
}


HaltonSampler * CreateHaltonSampler(const ParamSet &params, const Bounds2i &sampleBounds)
{
	int nsamp = params.FindOneInt("pixelsamples", 16);
	if (PbrtOptions.quickRender) nsamp = 1;
	bool sampleAtCenter =  params.FindOneBool("samplepixelcenter", false);
	return new HaltonSampler(nsamp, sampleBounds, sampleAtCenter);
}