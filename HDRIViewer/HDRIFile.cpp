extern "C" {
 #include "rgbe.h"
};
#include <cstdio>
#include "HDRIFile.h"



HRDIFile::HRDIFile()
	: ImageWidth(0)
	, ImageHeight(0)
{
}

HRDIFile::~HRDIFile()
{
}

bool HRDIFile::Load(const char* FileName)
{
	FILE* FileHandle;
	errno_t e = fopen_s(&FileHandle, FileName, "rb");
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
	long pos = ftell(FileHandle);
	int NumPixels = ImageW * ImageH;

	Data.resize(NumPixels * 3);
	if (RGBE_RETURN_SUCCESS != RGBE_ReadPixels_RLE(FileHandle, Data.data(), ImageW, ImageH))
	{
		return false;
	}

	ImageWidth = ImageW;
	ImageHeight = ImageH;

	return true;
}
