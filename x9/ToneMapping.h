#include "X9.h"

enum EToneMappingAlgo
{
	TMA_BILATERAL_FILTERING,
	TMA_REINHARD,
	TMA_REINHARD2,
	TMA_ACES,
	TMA_Unreal,
};

void X9_API ToneMapping(const float* InputData, int Width, int Height, unsigned char* OutputData, EToneMappingAlgo Algo = TMA_BILATERAL_FILTERING);