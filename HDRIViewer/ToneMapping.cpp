#include "ToneMapping.h"
extern "C"
{
#include "bmp.h"
}

#include <vector>
#include <numeric>
#include <cmath>

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

void RGB2XYZ(float R, float G, float B, float& X, float& Y, float& Z)
{
	X = 0.4124564f * R + 0.3575761f * G + 0.1804375f * B;
	Y = 0.2126729f * R + 0.7151522f * G + 0.0721750f * B;
	Z = 0.0193339f * R + 0.1191920f * G + 0.9503041f * B;
}

void XYZ2RGB(float X, float Y, float Z, float& R, float& G, float& B)
{
	R = 3.2404542f * X + -1.5371385f * Y + -0.4985314f * Z;
	G = -0.9692660f * X + 1.8760108f * Y + 0.0415560f * Z;
	B = 0.0556434f * X + -0.2040259f * Y + 1.0572252f * Z;
}

void XYZ2Yxy(float X, float Y, float Z, float& Yxyx, float& Yxyy, float& Yxyz)
{
	float inv = 1.0f / (X + Y + Z);
	Yxyx = Y;
	Yxyy = X * inv;
	Yxyz = Y * inv;
}

void Yxy2XYZ(float Yxyx, float Yxyy, float Yxyz, float& X, float& Y, float& Z)
{
	X = Yxyx * Yxyy / Yxyz;
	Y = Yxyx;
	Z = Yxyx * (1.0f - Yxyy - Yxyz) / Yxyz;
}

void RGB2Yxy(float R, float G, float B, float& Yxyx, float& Yxyy, float& Yxyz)
{
	float X, Y, Z;
	RGB2XYZ(R, G, B, X, Y, Z);
	XYZ2Yxy(X, Y, Z, Yxyx, Yxyy, Yxyz);
}

void Yxy2RGB(float Yxyx, float Yxyy, float Yxyz, float& R, float& G, float& B)
{
	float X, Y, Z;
	Yxy2XYZ(Yxyx, Yxyy, Yxyz, X, Y, Z);
	XYZ2RGB(X, Y, Z, R, G, B);
}

void Linear2Gamma(float LinearR, float LinearG, float LinearB, float& GammaR, float& GammaG, float& GammaB)
{
	GammaR = std::powf(LinearR, 1.0f / 2.2f);
	GammaG = std::powf(LinearG, 1.0f / 2.2f);
	GammaB = std::powf(LinearB, 1.0f / 2.2f);
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

float Reinhard(float x) {
	return x / (1.0f + x);
}

float Reinhard2(float x) {
	const float L_white = 4.0;
	return (x * (1.0f + x / (L_white * L_white))) / (1.0f + x);
}

void ToneMapping_Reinhard(const float* InputRGB, int Width, int Height, unsigned char* OutputData)
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
			Yxyx = Reinhard(lp);
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

void ToneMapping_Reinhard2(const float* InputRGB, int Width, int Height, unsigned char* OutputData)
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
			Yxyx = Reinhard2(lp);
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
		BMP_WritePixels_Gray(luminace, LuminaceLDR.data(), Width, Height);
		fclose(luminace);
	}
	{
		FILE* rgb;
		fopen_s(&rgb, "rgb.bmp", "w+b");
		BMP_WritePixels_RGB24(rgb, RGBLDR.data(), Width, Height);
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
		ToneMapping_Reinhard(InputRGB, Width, Height, OutputData);
		break;
	}
	case TMA_REINHARD2:
	{
		ToneMapping_Reinhard2(InputRGB, Width, Height, OutputData);
		break;
	}
	default:
		break;
	}
}


#include "HDRIFile.h"
#include "hdrloader.h"

int main(int argc, char** argv)
{
	HRDIFile f;
	f.Load("./hdri/wide_street_01_1k.hdr");

	std::vector<unsigned char> Output;
	Output.resize(f.Width() * f.Height() * 3);
	ToneMapping(f.GetData(), f.Width(), f.Height(), Output.data(), TMA_REINHARD2);
	{
		FILE* RGB;
		fopen_s(&RGB, "wide_street_01_1k.bmp", "w+b");
		BMP_WritePixels_RGB24(RGB, Output.data(), f.Width(), f.Height());
		fclose(RGB);
	}
// 	HDRLoader Loader;
// 	HDRLoaderResult Result;
// 	Loader.load("./hdri/wide_street_01_1k.hdr", Result);
// 	for (int y = 0; y < Result.height; ++y)
// 	{
// 		for (int x = 0; x < Result.width; ++x)
// 		{
// 			printf("%f \n", Result.cols[y * Result.width + x]);
// 		}
// 		printf("\n");
// 	}
}