#include "framework.h"
#include "Windowsx.h"
#include <assert.h>
#include <uxtheme.h>
#pragma comment (lib, "uxtheme.lib")

#include <stdio.h>


extern void LoadBitmap(const char* filename, BYTE** pBuffer, BITMAPINFO** bmInfo);
extern void BMP_WritePixels_Gray(const char* filename, int width, int height, unsigned char* rgbbuf, int buffersize);

bool CombineImages(const char* filename1, const char* filename2, const char* outname, int dir = 0)//dir=0,horizontal;dir=1,vertical
{
	BYTE *Buffer1, *Buffer2;
	BITMAPINFO *bmInfo1, *bmInfo2;
	LoadBitmap(filename1, &Buffer1, &bmInfo1);
	LoadBitmap(filename2, &Buffer2, &bmInfo2);
	if (dir == 0 && bmInfo1->bmiHeader.biHeight == bmInfo2->bmiHeader.biHeight)
	{
		UINT64 W1 = bmInfo1->bmiHeader.biWidth;
		UINT64 W2 = bmInfo2->bmiHeader.biWidth;
		UINT64 CombinedImageWidth = W1 + W2;
		UINT64 CombinedImageHeight = bmInfo1->bmiHeader.biHeight;
		UINT64 CombinedMemoryWitdh = CombinedImageWidth;
		if (CombinedImageWidth % 4 != 0)
		{
			CombinedMemoryWitdh = (CombinedImageWidth & (~3ull)) + 4;
		}
		BYTE* CombinedBuffer = new BYTE[CombinedMemoryWitdh * CombinedImageHeight];
		if (CombinedBuffer)
		{
			for (LONG h = 0; h < CombinedImageHeight; ++h)
			{
				memcpy(CombinedBuffer + CombinedImageWidth * h, Buffer1 + W1 * h, W1);
				memcpy(CombinedBuffer + CombinedImageWidth * h + W1, Buffer2 + W2 * h, W2);
			}
		}

		BMP_WritePixels_Gray(outname, CombinedImageWidth, CombinedImageHeight, CombinedBuffer, CombinedMemoryWitdh * CombinedImageHeight);
		return true;

	}
	else if (dir == 1 && bmInfo1->bmiHeader.biWidth != bmInfo2->bmiHeader.biWidth)
	{

	}
	return false;
}