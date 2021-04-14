#include "ToneMapping.h"
#include "ToneMapping_Curves.h"
#include "colorimetry.h"

#include <vector>
#include <numeric>
#include <algorithm>


void RGB2Luminance(const float* InputRGB, int Width, int Height, float* OutputLuminace)
{
	for (int y = 0; y < Height; ++y)
	{
		for (int x = 0; x < Width; ++x)
		{
			int ReadPixelIndex = (y * Width + x) * 3;
			int WritePixelIndex = ((Height - y - 1) * Width + x);
			float R = InputRGB[ReadPixelIndex + 0];
			float G = InputRGB[ReadPixelIndex + 1];
			float B = InputRGB[ReadPixelIndex + 2];
			OutputLuminace[WritePixelIndex] = 0.2126f * R + 0.7152f * G + 0.0722f * B;
		}
	}
}

int RGB2Bin(float R, float G, float B,float MinLogL, float InverseLogLRange)
{
	float L = 0.2126f * R + 0.7152f * G + 0.0722f * B;
	if (L < 0.0005) return 0;
	float LogL = (log2f(L) - MinLogL) * InverseLogLRange;
	if (LogL > 1.0f) LogL = 1.f;
	if (LogL < 0.f) LogL = 0.f;
	return (int)(LogL * 254.f + 1.f);
}


void RGB2AverageLuminace(const float* InputRGB, int Width, int Height, float MinLogL, float LogLRange, float& OutLAvg)
{
	int Histogram[256] = { 0 };
	for (int y = 0; y < Height; ++y)
	{
		for (int x = 0; x < Width; ++x)
		{
			int ReadPixelIndex = (y * Width + x) * 3;
			float R = InputRGB[ReadPixelIndex + 0];
			float G = InputRGB[ReadPixelIndex + 1];
			float B = InputRGB[ReadPixelIndex + 2];
			int BinIndex = RGB2Bin(R, G, B, MinLogL,1.f/ LogLRange);
			Histogram[BinIndex] += 1;
		}
	}
	int NumBlackPixels = Histogram[0];
	for (int i = 0; i < 256; ++i)
	{
		Histogram[i] *= i;
	}
	int TotalH = std::accumulate(std::begin(Histogram), std::end(Histogram), 0);
	float WightedLogAverage = TotalH / (float)std::max(Width * Height - NumBlackPixels, 1) - 1.0f;
	float WightedAverageL = exp2f(((WightedLogAverage / 254.0f) * LogLRange) + MinLogL);
	OutLAvg = WightedAverageL;
}

typedef float(*CurvePtr)(float);

void ToneMapping_WithCurve(const float* InputRGB, int Width, int Height, CurvePtr Curve, unsigned char* OutputData)
{
	float Lavg;
	RGB2AverageLuminace(InputRGB, Width, Height, -10.f, 12.f, Lavg);
	for (int y = 0; y < Height; ++y)
	{
		for (int x = 0; x < Width; ++x)
		{
			int ReadPixelIndex = (y * Width + x) * 3;
			int WritePixelIndex = ((Height - y - 1) * Width + x) * 3;
			float R = InputRGB[ReadPixelIndex + 0];
			float G = InputRGB[ReadPixelIndex + 1];
			float B = InputRGB[ReadPixelIndex + 2];
			float Yxyx, Yxyy, Yxyz;
			RGB2Yxy(R, G, B, Yxyx, Yxyy, Yxyz);
			float lp = Yxyx / (9.6f * Lavg + 0.0001f);
			Yxyx = Curve(lp);
			Yxy2RGB(Yxyx, Yxyy, Yxyz, R, G, B);
			if (R < 0.f) R = 0.f;
			if (R > 1.f) R = 1.f;
			if (G < 0.f) G = 0.f;
			if (G > 1.f) G = 1.f;
			if (B < 0.f) B = 0.f;
			if (B > 1.f) B = 1.f;
			Linear2Gamma(R, G, B, R, G, B);
			OutputData[WritePixelIndex + 0] = (unsigned char)(B * 255.f);
			OutputData[WritePixelIndex + 1] = (unsigned char)(G * 255.f);
			OutputData[WritePixelIndex + 2] = (unsigned char)(R * 255.f);
		}
	}
}

void ToneMapping_Unreal(const float* InputRGB, int Width, int Height, unsigned char* OutputData)
{
	float Lavg;
	RGB2AverageLuminace(InputRGB, Width, Height, -10.f, 12.f, Lavg);
	for (int y = 0; y < Height; ++y)
	{
		for (int x = 0; x < Width; ++x)
		{
			int ReadPixelIndex = (y * Width + x) * 3;
			int WritePixelIndex = ((Height - y - 1) * Width + x) * 3;
			float R = InputRGB[ReadPixelIndex + 0];
			float G = InputRGB[ReadPixelIndex + 1];
			float B = InputRGB[ReadPixelIndex + 2];
			float Yxyx, Yxyy, Yxyz;
			RGB2Yxy(R, G, B, Yxyx, Yxyy, Yxyz);
			float lp = Yxyx / (9.6f * Lavg + 0.0001f);
			Yxyx = Tonemap_Unreal(lp);
			Yxy2RGB(Yxyx, Yxyy, Yxyz, R, G, B);
			if (R < 0.f) R = 0.f;
			if (R > 1.f) R = 1.f;
			if (G < 0.f) G = 0.f;
			if (G > 1.f) G = 1.f;
			if (B < 0.f) B = 0.f;
			if (B > 1.f) B = 1.f;
			OutputData[WritePixelIndex + 0] = (unsigned char)(B * 255.f);
			OutputData[WritePixelIndex + 1] = (unsigned char)(G * 255.f);
			OutputData[WritePixelIndex + 2] = (unsigned char)(R * 255.f);
		}
	}
}

void ToneMapping_BilateralFiltering(const float* InputRGB, int Width, int Height, unsigned char* OutputData)
{
	std::vector<float> Luminace;
	Luminace.resize(Width * Height);
	RGB2Luminance(InputRGB, Width, Height, Luminace.data());
	float SigmaD = 3.f;
	float SigmaR = 10.f;

	const int Domain = 32;
	for (int y = 0; y < Height; ++y)
	{
		for (int x = 0; x < Width; ++x)
		{
			float W = 0;
			float L = 0;
			float LuminaceXY = Luminace[(y * Width + x)];
			for (int dy = -Domain; dy <= Domain; ++dy)
			{
				for (int dx = -Domain; dx <= Domain; ++dx)
				{
					int i = x + dx;
					int j = y + dy;

					if (i < 0 || i > Width - 1) continue;
					if (j < 0 || j > Height - 1) continue;

					float LuminaceIJ = Luminace[(j * Width + i)];
					float DeltaL = LuminaceXY - LuminaceIJ;
					float index = ((dx * dx) + (dy * dy)) / (2 * SigmaD * SigmaD) + (DeltaL * DeltaL) / (2 * SigmaR * SigmaR);
					float w = expf(-index);
					W += w;
					L += LuminaceIJ * w;
				}
			}
			OutputData[y  * Width + x] = (unsigned char)((L / W)*255.f);
		}
	}
	std::vector<unsigned char> LuminaceLDR;
	std::vector<unsigned char> RGBLDR;
	LuminaceLDR.resize(Width * Height);
	RGBLDR.resize(Width * Height * 3);
	for (size_t i =0;i<Luminace.size();++i)
	{
		LuminaceLDR[i] = (unsigned char)(Luminace[i] * 255.0f);
	}
	for (int y = 0; y < Height; ++y)
	{
		for (int x = 0; x < Width; ++x)
		{
			int ReadPixelIndex = (y * Width + x) * 3;
			int WritePixelIndex = ((Height - y  - 1) * Width + x) * 3;
			RGBLDR[WritePixelIndex + 0] = (unsigned char)(InputRGB[ReadPixelIndex + 0] * 255.f);
			RGBLDR[WritePixelIndex + 1] = (unsigned char)(InputRGB[ReadPixelIndex + 1] * 255.f);
			RGBLDR[WritePixelIndex + 2] = (unsigned char)(InputRGB[ReadPixelIndex + 2] * 255.f);
		}
	}
	{
		FILE* luminace;
		fopen_s(&luminace, "luminace.bmp", "w+b");
		//BMP_WritePixels_Gray(luminace, LuminaceLDR.data(), Width, Height);
		fclose(luminace);
	}
	{
		FILE* rgb;
		fopen_s(&rgb, "rgb.bmp", "w+b");
		//BMP_WritePixels_RGB24(rgb, RGBLDR.data(), Width, Height);
		fclose(rgb);
	}
}

void ToneMapping(const float* InputRGB, int Width, int Height, unsigned char* OutputData, EToneMappingAlgo Algo /*= TMA_BILATERAL_FILTERING*/)
{
	if (Width <= 0 || Height <= 0) return;

	switch (Algo)
	{
	case TMA_BILATERAL_FILTERING:
	{
		ToneMapping_BilateralFiltering(InputRGB, Width, Height, OutputData);
		break;
	}
	case TMA_REINHARD:
	{
		ToneMapping_WithCurve(InputRGB, Width, Height, Reinhard, OutputData);
		break;
	}
	case TMA_REINHARD2:
	{
		ToneMapping_WithCurve(InputRGB, Width, Height, Reinhard2, OutputData);
		break;
	}
	case TMA_ACES:
	{
		ToneMapping_WithCurve(InputRGB, Width, Height, Tonemap_ACES, OutputData);
		break;
	}
	case TMA_Unreal:
	{
		ToneMapping_Unreal(InputRGB, Width, Height, OutputData);
		break;
	}
	default:
		break;
	}
}


