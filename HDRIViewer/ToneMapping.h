
enum EToneMappingAlgo
{
	TMA_BILATERAL_FILTERING,
};

void ToneMapping(const float* InputData, int Width, int Height, unsigned char* OutputData, EToneMappingAlgo Algo = TMA_BILATERAL_FILTERING);