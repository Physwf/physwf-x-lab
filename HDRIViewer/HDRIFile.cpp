#include "rgbe.h"
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
	errno_t e = fopen_s(&FileHandle, FileName, "r");
	if (e != 0)
	{
		return false;
	}

	int ImageW, int ImageH;
	rgbe_header_info Header;
	if (RGBE_RETURN_SUCCESS != RGBE_ReadHeader(FileHandle, &ImageW, &ImageH, &Header))
	{
		return false;
	}

	int NumPixels = ImageW * ImageH;
	Data.resize(NumPixels);
	if (RGBE_RETURN_SUCCESS != RGBE_ReadPixels(FileHandle, Data.data(), NumPixels))
	{
		return false;
	}

	ImageWidth = ImageW;
	ImageHeight = ImageH;

	return true;
}
