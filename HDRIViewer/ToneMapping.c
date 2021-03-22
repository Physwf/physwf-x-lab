#include "ToneMapping.h"


void ToneMapping_BilateralFiltering(float* InputData, int Width, int Height, unsigned char* OutputData)
{

}

void ToneMapping(float* InputData, int Width, int Height, unsigned char* OutputData, EToneMappingAlgo Algo /*= TMA_BILATERAL_FILTERING*/)
{
	if (Width <= 0 || Height <= 0) return;

	switch (Algo)
	{
	case TMA_BILATERAL_FILTERING:
	{
		ToneMapping_BilateralFiltering(InputData, Width, Height, OutputData);
		break;
	}
	default:
		break;
	}
}

