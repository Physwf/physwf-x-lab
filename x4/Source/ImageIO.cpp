#include "../Include/ImageIO.h"

extern "C" {
#include "rgbe.h"
};
#include "bmp.h"

#include <vector>

bool X4_API ReadImageHDR(const char* filename, void* Buffer, int* width, int* height)
{
	FILE* FileHandle;
	errno_t e = fopen_s(&FileHandle, filename, "rb");
	if (e != 0)
	{
		return false;
	}

	int ImageW, ImageH;
	rgbe_header_info Header;
	if (RGBE_RETURN_SUCCESS != RGBE_ReadHeader(FileHandle, &ImageW, &ImageH, NULL))
	{
		return false;
	}

	if (width != nullptr) *width = ImageW;
	if (height != nullptr) *height = ImageH;

	if (Buffer != nullptr)
	{
		long pos = ftell(FileHandle);
		int NumPixels = ImageW * ImageH;
		std::vector<float> Data;
		Data.resize(NumPixels * 3);
		if (RGBE_RETURN_SUCCESS != RGBE_ReadPixels_RLE(FileHandle, Data.data(), ImageW, ImageH))
		{
			return false;
		}

		memcpy(Buffer, Data.data(), NumPixels * 3 * sizeof(float));
	}
	return true;
}

bool X4_API WriteImageHDR(const char* filename, void* Buffer, int width, int height)
{
	return false;

}

bool X4_API ReadImageBMP(const char* filename, void* Buffer, int* width, int* height)
{
	return false;
}

bool X4_API WriteImageBMP(const char* filename, void* Buffer, int width, int height) 
{
	return false;
}

