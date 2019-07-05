#include <windows.h>
#include <gdiplus.h>
#include <stdio.h>
#include <shlwapi.h>

using namespace Gdiplus;

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes

	ImageCodecInfo* pImageCodecInfo = NULL;

	GetImageEncodersSize(&num, &size);
	if (size == 0)
		return -1;  // Failure

	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if (pImageCodecInfo == NULL)
		return -1;  // Failure

	GetImageEncoders(num, size, pImageCodecInfo);

	for (UINT j = 0; j < num; ++j)
	{
		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;  // Success
		}
	}

	free(pImageCodecInfo);
	return -1;  // Failure
}

bool SavePixelsToPNG(const WCHAR* pFileName, int W, int H, unsigned int* pPixels)
{
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	CLSID   encoderClsid;
	Status  stat;

	// Get the CLSID of the PNG encoder.
	GetEncoderClsid(L"image/jpeg", &encoderClsid);
	Bitmap* pBitmap = new Bitmap(W,H, PixelFormat32bppARGB);
	for (auto y = 0; y < H; ++y)
	{
		for (auto x = 0; x < W; ++x)
		{
			pBitmap->SetPixel(x, y, Color(pPixels[y*W + x]));
		}
	}
	stat = pBitmap->Save(pFileName, &encoderClsid, NULL);
	delete pBitmap;
	GdiplusShutdown(gdiplusToken);

	return stat == Ok;
}