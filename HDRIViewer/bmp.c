#include "bmp.h"
#include <Windows.h>

void BMP_WritePixels_RGB24(FILE* stream, unsigned char* rgbbuf, int width, int height)
{
	BITMAPFILEHEADER bfh;
	BITMAPINFOHEADER bih;

	bfh.bfType = 0x4d42;
	bfh.bfReserved1  = 0;
	bfh.bfReserved2  = 0;
	bfh.bfSize  = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + width * height * 3;
	bfh.bfOffBits  = 0x36;

	bih.biSize  = sizeof(BITMAPINFOHEADER);
	bih.biWidth  = width;
	bih.biHeight  = height;
	bih.biPlanes  = 1;
	bih.biBitCount  = 24;
	bih.biCompression  = 0;
	bih.biSizeImage  = 0;
	bih.biXPelsPerMeter  = 0;
	bih.biYPelsPerMeter  = 0;
	bih.biClrUsed  = 0;
	bih.biClrImportant  = 0;

	/*Write headers*/ 
	fwrite(&bfh, sizeof(bfh), 1, stream);
	fwrite(&bih, sizeof(bih), 1, stream);
	fwrite(rgbbuf, width * height * 3, 1, stream);
}

void BMP_WritePixels_Gray(FILE* stream, unsigned char* rgbbuf, int width, int height)
{
	BITMAPFILEHEADER bfh;
	BITMAPINFOHEADER bih;

	bfh.bfType = 0x4d42;
	bfh.bfReserved1 = 0;
	bfh.bfReserved2 = 0;
	bfh.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + width * height;
	bfh.bfOffBits = 0x36;

	bih.biSize = sizeof(BITMAPINFOHEADER);
	bih.biWidth = width;
	bih.biHeight = height;
	bih.biPlanes = 1;
	bih.biBitCount = 8;
	bih.biCompression = BI_RGB;
	bih.biSizeImage = width * height;
	bih.biXPelsPerMeter = 0;
	bih.biYPelsPerMeter = 0;
	bih.biClrUsed = 0;
	bih.biClrImportant = 0;

	//调色板
	RGBQUAD rgbquad[256];
	int i;
	for (i = 0; i < 256; i++)
	{
		rgbquad[i].rgbBlue = i;
		rgbquad[i].rgbGreen = i;
		rgbquad[i].rgbRed = i;
		rgbquad[i].rgbReserved = 0;
	}

	/*Write headers*/ 
	fwrite(&bfh, sizeof(bfh), 1, stream);
	fwrite(&bih, sizeof(bih), 1, stream);
	fwrite(&rgbquad, sizeof(rgbquad), 1, stream);
	fwrite(rgbbuf, width * height, 1, stream);
}

