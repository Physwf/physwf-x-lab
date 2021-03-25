#include "ToneMapping.h"
extern "C"
{
#include "bmp.h"
}

#include <vector>

void RGB2Luminance(const float* InputRGB, int Width, int Height, float* OutputLuminace)
{
	for (int y = 0; y < Height; ++y)
	{
		for (int x = 0; x < Width; ++x)
		{
			int PixelIndex = (y * Width + x ) * 3;
			float R = InputRGB[PixelIndex + 0];
			float G = InputRGB[PixelIndex + 1];
			float B = InputRGB[PixelIndex + 2];
			OutputLuminace[y * Width + x] = 0.2126f * R + 0.7152f * G + 0.0722f * B;
		}
	}
}

void ToneMapping_BilateralFiltering(const float* InputRGB, int Width, int Height, unsigned char* OutputData)
{
	std::vector<float> Luminace;
	std::vector<unsigned char> LuminaceLDR;
	std::vector<unsigned char> RGBLDR;
	Luminace.resize(Width * Height);
	LuminaceLDR.resize(Width * Height);
	RGBLDR.resize(Width * Height * 3);
	RGB2Luminance(InputRGB, Width, Height, Luminace.data());
	for (size_t i =0;i<Luminace.size();++i)
	{
		LuminaceLDR[i] = (unsigned char)(Luminace[i] * 255.0f);
	}
	for (int y = 0; y < Height; ++y)
	{
		for (int x = 0; x < Width; ++x)
		{
			int PixelIndex = (y * Width + x) * 3;
			RGBLDR[PixelIndex + 0] = (unsigned char)(InputRGB[PixelIndex + 0] * 255.f);
			RGBLDR[PixelIndex + 1] = (unsigned char)(InputRGB[PixelIndex + 1] * 255.f);
			RGBLDR[PixelIndex + 2] = (unsigned char)(InputRGB[PixelIndex + 2] * 255.f);
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

	ToneMapping(f.GetData(), f.Width(), f.Height(),NULL);

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