#pragma once

#include "pbrt.h"

static const int sampledLambdaStart = 400;
static const int sampledLambdaEnd = 700;
static const int nSpectralSamples = 60;

extern bool SpectrumSamplesSorted(const Float* Lambda, const Float* vals, int n);
extern void SortSpectrumSamples(Float* lambda, Float *vals, int n);
//求指定光谱(lambda,vals,n)在(lambdaStart,lambdaEnd)区间内的平均值
extern Float AverageSpectrumSamples(const Float* lambda, const Float* vals, int n, Float lambdaStart, Float lambdaEnd);

inline void XYZToRGB(const Float xyz[3], Float rgb[3])
{
	rgb[0] = 3.240479f * xyz[0] - 1.537150f * xyz[1] - 0.498535f * xyz[2];
	rgb[1] = -0.969256f * xyz[0] + 1.875991f * xyz[1] + 0.041556f * xyz[2];
	rgb[2] = 0.055648f * xyz[0] - 0.204043f * xyz[1] + 1.057311f * xyz[2];
}
inline void RGBToXYZ(const Float rgb[3], Float xyz[3])
{
	xyz[0] = 0.412453f * rgb[0] + 0.357580f * rgb[1] + 0.180423f * rgb[2];
	xyz[1] = 0.212671f * rgb[0] + 0.715160f * rgb[1] + 0.072169f * rgb[2];
	xyz[2] = 0.019334f * rgb[0] + 0.119193f * rgb[1] + 0.950227f * rgb[2];
}

enum class SpectrumType { Reflectance, Illuminant };
//根据波长l在光谱(lambda,vals,n)中求出对应的value插值
extern Float InterpolateSpecturmSamples(const Float* lambda, const Float* vals, int n, float l);
extern void Blackbody(const Float* lambda, int n, Float T, float *Le);
extern void BlackbodyNormalized(const Float* lambda, int n, Float T, Float* vals);

static const int nCIESamples = 471;
extern const Float CIE_X[nCIESamples];
extern const Float CIE_Y[nCIESamples];
extern const Float CIE_Z[nCIESamples];
extern const Float CIE_lambda[nCIESamples];
static const Float CIE_Y_integral = 106.856895;
static const int nRGB2SpectSamples = 32;
extern const Float RGBRefl2SpectLambda[nSpectralSamples];
extern const Float RGBRefl2SpectWhite[nSpectralSamples];
extern const Float RGBRefl2SpectCyan[nSpectralSamples];
extern const Float RGBRefl2SpectMagenta[nRGB2SpectSamples];
extern const Float RGBRefl2SpectYellow[nRGB2SpectSamples];
extern const Float RGBRefl2SpectRed[nRGB2SpectSamples];
extern const Float RGBRefl2SpectGreen[nRGB2SpectSamples];
extern const Float RGBRefl2SpectBlue[nRGB2SpectSamples];
extern const Float RGBIllum2SpectWhite[nRGB2SpectSamples];
extern const Float RGBIllum2SpectCyan[nRGB2SpectSamples];
extern const Float RGBIllum2SpectMagenta[nRGB2SpectSamples];
extern const Float RGBIllum2SpectYellow[nRGB2SpectSamples];
extern const Float RGBIllum2SpectRed[nRGB2SpectSamples];
extern const Float RGBIllum2SpectGreen[nRGB2SpectSamples];
extern const Float RGBIllum2SpectBlue[nRGB2SpectSamples];

template <int nSpectrumSamples> class CoefficientSpectrum
{
public:
	CoefficientSpectrum(Float v = 0.f)
	{
		for (int i = 0; i < nSpectrumSamples; ++i)
		{
			c[i] = v;
		}
	}
	CoefficientSpectrum& operator+=(const CoefficientSpectrum& &s2)
	{
		DOCHECK(!s2.HasNaNs());
		for (int i = 0; i < nSpectrumSamples; ++i)
		{
			c[i] += s2.c[i];
		}
		return *this;
	}
	CoefficientSpectrum operator+(const CoefficientSpectrum& &s2)
	{
		DOCHECK(!s2.HasNaNs());
		CoefficientSpectrum ret = *this;
		for (int i = 0; i < nSpectrumSamples; ++i)
		{
			ret.c[i] += s2.c[i];
		}
		return ret;
	}
	CoefficientSpectrum operator-(const CoefficientSpectrum &s2)
	{
		DOCHECK(!s2.HasNaNs());
		CoefficientSpectrum ret = *this;
		for (int i = 0; i < nSpectrumSamples; ++i)
		{
			ret.c[i] -= s2.c[i];
		}
		return ret;
	}
	CoefficientSpectrum operator/(const CoefficientSpectrum &s2)
	{
		DOCHECK(!s2.HasNaNs());
		CoefficientSpectrum ret = *this;
		for (int i = 0; i < nSpectrumSamples; ++i)
		{
			DOCHECK(s2.c[i] != 0);
			ret.c[i] /= s2.c[i];
		}
		return ret;
	}
	CoefficientSpectrum operator*(const CoefficientSpectrum &s2)
	{
		DOCHECK(!s2.HasNaNs());
		CoefficientSpectrum ret = *this;
		for (int i = 0; i < nSpectrumSamples; ++i)
		{
			ret.c[i] *= s2.c[i];
		}
		return ret;
	}
	CoefficientSpectrum& operator*=(const CoefficientSpectrum& &s2)
	{
		DOCHECK(!s2.HasNaNs());
		for (int i = 0; i < nSpectrumSamples; ++i)
		{
			c[i] *= s2.c[i];
		}
		return *this;
	}
	CoefficientSpectrum operator*(Float a)
	{
		CoefficientSpectrum ret = *this;
		for (int i = 0; i < nSpectrumSamples; ++i)
		{
			ret.c[i] *= a;
		}
		DOCHECK(!ret.HasNaNs());
		return ret;
	}
	CoefficientSpectrum& operator*=(Float a)
	{
		for (int i = 0; i < nSpectrumSamples; ++i)
		{
			c[i] *= a;
		}
		DOCHECK(!HasNaNs());
		return *this;
	}
	CoefficientSpectrum operator/(Float a)
	{
		DOCHECK(a != 0);
		DOCHECK(!std::isnan(a));
		CoefficientSpectrum ret = *this;
		for (int i = 0; i < nSpectrumSamples; ++i)
		{
			ret.c[i] /= a;
		}
		DOCHECK(!ret.HasNaNs());
		return ret;
	}
	CoefficientSpectrum& operator/=(Float a)
	{
		DOCHECK(a != 0);
		DOCHECK(!std::isnan(a));
		for (int i = 0; i < nSpectrumSamples; ++i)
		{
			c[i] /= a;
		}
		DOCHECK(!ret.HasNaNs());
		return *this;
	}
	bool operator==(const CoefficientSpectrum& &sp) const
	{
		for (int i = 0; i < nSpectrumSamples; ++i)
		{
			if (c[i] != sp.c[i]) return false;
		}
		return true;
	}
	bool operator!=(const CoefficientSpectrum& &sp) const
	{
		return !(*this == sp);
	}
	bool IsBlack() const
	{
		for (int i = 0; i < nSpectrumSamples; ++i)
		{
			if (c[i] != 0.f) return false;
		}
		return true;
	}
	friend inline CoefficientSpectrum operator*(Float a, const CoefficientSpectrum &s)
	{
		return s * a;
	}
	friend CoefficientSpectrum Sqrt(const CoefficientSpectrum& s)
	{
		CoefficientSpectrum ret;
		for (int i = 0; i < nSpectrumSamples; ++i)
		{
			ret.c[i] += std::sqrt(s.c[i]);
		}
		DOCHECK(!ret.HasNaNs());
		return ret;
	}
	template<int n>
	friend inline CoefficientSpectrum<n> Pow(const CoefficientSpectrum<n> &s, Float e);
	friend CoefficientSpectrum Exp(const CoefficientSpectrum &s)
	{
		CoefficientSpectrum ret;
		for (int i = 0; i < nSpectrumSamples; ++i) ret.c[i] = std::exp(s.c[i]);
		DOCHECK(!ret.HasNaNs());
		return ret;
	}
	CoefficientSpectrum Clamp(Float Low = 0, Float High = Infinity)
	{
		CoefficientSpectrum ret;
		for (int i = 0; i < nSpectrumSamples; ++i)
		{
			ret.c[i] = Clamp(c[i], Low, High);
		}
		return ret;
	}
	Float MaxComponentValue() const {
		Float m = c[0];
		for (int i = 1; i < nSpectrumSamples; ++i)
			m = std::max(m, c[i]);
		return m;
	}
	bool HasNaNs() const {
		for (int i = 0; i < nSpectrumSamples; ++i)
			if (std::isnan(c[i])) return true;
		return false;
	}
	Float &operator[](int i) {
		DOCHECK(i >= 0 && i < nSpectrumSamples);
		return c[i];
	}
	Float operator[](int i) const {
		DOCHECK(i >= 0 && i < nSpectrumSamples);
		return c[i];
	}

	static const int nSamples = nSpectrumSamples;

protected:
	Float c[nSpectrumSamples];
};

class SampledSpectrum : public CoefficientSpectrum<nSpectralSamples>
{
public:
	SampledSpectrum(Float v = 0.f) :CoefficientSpectrum(v) {}
	SampledSpectrum(const CoefficientSpectrum<nSpectralSamples> &v) : CoefficientSpectrum<nSpectralSamples>(v) {}

	static SampledSpectrum FromSampled(const Float* lambda, const Float *v, int n)
	{
		if (!SpectrumSamplesSorted(lambda, v, n))
		{
			std::vector<Float> slambda(&lambda[0], &lambda[1]);
			std::vector<Float> sv(&v[0], &v[n]);
			SortSpectrumSamples(&slambda[0], &sv[0], n);
			return FromSampled(&slambda[0], &sv[0], n);
		}
		SampledSpectrum r;
		for (int i = 0; i < nSpectralSamples; ++i)
		{
			Float lambda0 = Lerp(Float(i) / Float(nSpectralSamples), sampledLambdaStart, sampledLambdaEnd);
			Float lambda1 = Lerp(Float(i+1) / Float(nSpectralSamples), sampledLambdaStart, sampledLambdaEnd);
			r.c[i] = AverageSpectrumSamples(lambda, v, n, lambda0, lambda1);
		}
	}

	static void Init()
	{
		for (int i = 0; i < nSpectralSamples; ++i)
		{
			Float wl0 = Lerp(Float(i) / Float(nSpectralSamples), sampledLambdaStart, sampledLambdaEnd);
			Float wl1 = Lerp(Float(i + 1) / Float(nSpectralSamples), sampledLambdaStart, sampledLambdaEnd);
			X.c[i] = AverageSpectrumSamples(CIE_lambda, CIE_X, nCIESamples, wl0, wl1);
			Y.c[i] = AverageSpectrumSamples(CIE_lambda, CIE_Y, nCIESamples, wl0, wl1);
			Z.c[i] = AverageSpectrumSamples(CIE_lambda, CIE_Z, nCIESamples, wl0, wl1);
		}


	}

	void ToXYZ(Float xyz[3])
	{
		xyz[0] = xyz[1] = xyz[2] = 0.0f;
		for (int i = 0; i < nSpectralSamples; ++i)
		{
			xyz[0] += X.c[i] * c[i];
			xyz[1] += Y.c[i] * c[i];
			xyz[2] += Z.c[i] * c[i];
		}
		Float scale = Float(sampledLambdaEnd - sampledLambdaStart) / Float(CIE_Y_integral * nSpectralSamples);
		xyz[0] *= scale;
		xyz[1] *= scale;
		xyz[2] *= scale;
	}

	Float y() const
	{
		Float yy = 0.f;
		for (int i = 0; i < nSpectralSamples; ++i)
		{
			yy += Y.c[i] * c[i];
		}
		return yy * Float(sampledLambdaEnd - sampledLambdaStart) / Float(CIE_Y_integral * nSpectralSamples);
	}

	void ToRBG(Float rgb[3])
	{
		Float xyz[3];
		ToXYZ(xyz);
		XYZToRGB(xyz, rgb);
	}

	RGBSpectrum ToRGBSpectrum() const
	{

	}

	static SampledSpectrum FromRGB(const Float rgb[3], SpectrumType type = SpectrumType::Illuminant);

	static SampledSpectrum FromXYZ(const Float xyz[3], SpectrumType type = SpectrumType::Reflectance)
	{
		Float rgb[3];
		XYZToRGB(xyz, rgb);
		return FromRGB(rgb, type);
	}
private:
	static SampledSpectrum X, Y, Z;//X Y Z的SampledSpectrum表示
	static SampledSpectrum rgbRefl2SpectWhite, rgbRefl2SpectCyan;
	static SampledSpectrum rgbRefl2SpectMagenta, rgbRefl2SpectYellow;
	static SampledSpectrum rgbRefl2SpectRed, rgbRefl2SpectGreen;
	static SampledSpectrum rgbRefl2SpectBlue;
	static SampledSpectrum rgbIllum2SpectWhite, rgbIllum2SpectCyan;
	static SampledSpectrum rgbIllum2SpectMagenta, rgbIllum2SpectYellow;
	static SampledSpectrum rgbIllum2SpectRed, rgbIllum2SpectGreen;
	static SampledSpectrum rgbIllum2SpectBlue;

};

class RGBSpectrum : public CoefficientSpectrum<3>
{
public:
	RGBSpectrum(Float v = 0.f) : CoefficientSpectrum<3> (v) {}
	RGBSpectrum(const CoefficientSpectrum<3>& v) : CoefficientSpectrum<3>(v) {}
	RGBSpectrum(const RGBSpectrum &s, SpectrumType type = SpectrumType::Reflectance) { *this = s; }

	static RGBSpectrum FromRGB(const Float rgb[3], SpectrumType type = SpectrumType::Reflectance)
	{
		RGBSpectrum s;
		s.c[0] = rgb[0];
		s.c[1] = rgb[1];
		s.c[2] = rgb[2];
		DOCHECK(!s.HasNaNs());
		return s;
	}

	void ToRGB(Float *rgb) const
	{
		rgb[0] = c[0];
		rgb[1] = c[1];
		rgb[2] = c[2];
	}

	const RGBSpectrum& ToRGBSpectrum() const { return *this; }
	void ToXYZ(Float xyz[3]) const { RGBToXYZ(c, xyz); }
	static RGBSpectrum FromXYZ(const Float xyz[3], SpectrumType type = SpectrumType::Reflectance)
	{
		RGBSpectrum r;
		XYZToRGB(xyz, r.c);
		return r;
	}

	Float y() const
	{
		const Float YWeight[3] = { 0.212671f, 0.715160f, 0.072169f };
		return YWeight[0] * c[0] + YWeight[1] * c[1] + YWeight[2] * c[2];
	}

	static RGBSpectrum FromSampled(const Float* lambda, const Float* v, int n)
	{
		if (!SpectrumSamplesSorted(lambda, v, n))
		{
			std::vector<Float> slambda(&lambda[0], &lambda[n]);
			std::vector<Float> sv(&v[0], &v[n]);
			SortSpectrumSamples(&slambda[0], &sv[0], n);
			return FromSampled(&slambda[0], &sv[0], n);
		}
		Float xyz[3] = { 0,0,0 };
		for (int i = 0; i < nCIESamples; ++i)
		{
			Float val = InterpolateSpecturmSamples(lambda, v, n, CIE_lambda[i]);
			xyz[0] += val * CIE_X[i];
			xyz[1] += val * CIE_Y[i];
			xyz[2] += val * CIE_Z[i];
		}
		Float scale = Float(CIE_lambda[nCIESamples - 1] - CIE_lambda[0]) / CIE_Y_integral * nCIESamples;
		xyz[0] *= scale;
		xyz[1] *= scale;
		xyz[2] *= scale;
		return FromXYZ(xyz);
	}
};

template <int nSpectrumSamples>
inline CoefficientSpectrum<nSpectrumSamples> Pow(const CoefficientSpectrum<nSpectrumSamples> &s, Float e)
{
	CoefficientSpectrum<nSpectrumSamples> ret;
	for (int i = 0; i < nSpectrumSamples; ++i) ret.c[i] = std::pow(s.c[i], e);
	DOCHECK(!ret.HasNaNs());
	return ret;
}

inline RGBSpectrum Lerp(Float t, const RGBSpectrum &s1, const RGBSpectrum &s2)
{
	return (1 - t) *s1 + t * s2;
}

inline SampledSpectrum Lerp(Float t, const SampledSpectrum &s1, const SampledSpectrum &s2)
{
	return (1 - t) *s1 + t * s2;
}

void ResampleLinearSpectrum(const Float *lamdaIn, const Float* vIn, int nIn, Float lambdaMin, Float lambdaMax, int nOut, Float * vOut);