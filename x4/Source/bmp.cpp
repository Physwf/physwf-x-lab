#include "bmp.h"
#include <Windows.h>

void FillRGBHeader(int width, int height, BITMAPFILEHEADER* pbfh, BITMAPINFOHEADER* pbih)
{
	pbfh->bfType = 0x4d42;
	pbfh->bfReserved1 = 0;
	pbfh->bfReserved2 = 0;
	pbfh->bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + width * height * 3;
	pbfh->bfOffBits = 0x36;

	pbih->biSize = sizeof(BITMAPINFOHEADER);
	pbih->biWidth = width;
	pbih->biHeight = height;
	pbih->biPlanes = 1;
	pbih->biBitCount = 24;
	pbih->biCompression = 0;
	pbih->biSizeImage = 0;
	pbih->biXPelsPerMeter = 0;
	pbih->biYPelsPerMeter = 0;
	pbih->biClrUsed = 0;
	pbih->biClrImportant = 0;
}

unsigned int GetRGBHeaderSize()
{
	return sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
}

unsigned int GetGrayHeaderSize()
{
	return sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 256;
}

void BMP_WritePixels_RGB24(FILE* stream, unsigned char* rgbbuf, int width, int height)
{
	BITMAPFILEHEADER bfh;
	BITMAPINFOHEADER bih;

	FillRGBHeader(width, height, &bfh, &bih);

	/*Write headers*/ 
	fwrite(&bfh, sizeof(bfh), 1, stream);
	fwrite(&bih, sizeof(bih), 1, stream);
	fwrite(rgbbuf, width * height * 3, 1, stream);
}

void BMP_WritePixels_RGB24(void* stream, unsigned char* rgbbuf, int width, int height)
{
	BITMAPFILEHEADER bfh;
	BITMAPINFOHEADER bih;

	FillRGBHeader(width, height, &bfh, &bih);

	/*Write headers*/
	memcpy_s(stream, sizeof(bfh), &bfh, sizeof(bfh));
	memcpy_s((char*)stream + sizeof(bfh), sizeof(bih), &bih, sizeof(bih));
	memcpy_s((char*)stream + sizeof(bfh) + sizeof(bih), width * height * 3, rgbbuf, width * height * 3);
}

void FillGreyHeader(int width, int height, BITMAPFILEHEADER* pbfh, BITMAPINFOHEADER* pbih, RGBQUAD* prgbquad, int rgbquadsize)
{
	pbfh->bfType = 0x4d42;
	pbfh->bfReserved1 = 0;
	pbfh->bfReserved2 = 0;
	pbfh->bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + width * height;
	pbfh->bfOffBits = 0x36;

	pbih->biSize = sizeof(BITMAPINFOHEADER);
	pbih->biWidth = width;
	pbih->biHeight = height;
	pbih->biPlanes = 1;
	pbih->biBitCount = 8;
	pbih->biCompression = BI_RGB;
	pbih->biSizeImage = width * height;
	pbih->biXPelsPerMeter = 0;
	pbih->biYPelsPerMeter = 0;
	pbih->biClrUsed = 0;
	pbih->biClrImportant = 0;

	//调色板
	int i;
	for (i = 0; i < rgbquadsize; i++)
	{
		prgbquad[i].rgbBlue = i;
		prgbquad[i].rgbGreen = i;
		prgbquad[i].rgbRed = i;
		prgbquad[i].rgbReserved = 0;
	}
}

void BMP_WritePixels_Gray(FILE* stream, unsigned char* rgbbuf, int width, int height)
{
	BITMAPFILEHEADER bfh;
	BITMAPINFOHEADER bih;
	RGBQUAD rgbquad[256];

	FillGreyHeader(width,height,&bfh,&bih, rgbquad, 256);

	/*Write headers*/ 
	fwrite(&bfh, sizeof(bfh), 1, stream);
	fwrite(&bih, sizeof(bih), 1, stream);
	fwrite(&rgbquad, sizeof(rgbquad), 1, stream);
	fwrite(rgbbuf, width * height, 1, stream);
}

void BMP_WritePixels_Gray(void* stream, unsigned char* rgbbuf, int width, int height)
{
	BITMAPFILEHEADER bfh;
	BITMAPINFOHEADER bih;
	RGBQUAD rgbquad[256];

	FillGreyHeader(width, height, &bfh, &bih, rgbquad, 256);

	memcpy_s(stream, sizeof(bfh), &bfh, sizeof(bfh));
	memcpy_s((char*)stream + sizeof(bfh), sizeof(bih), &bih, sizeof(bih));
	memcpy_s((char*)stream + sizeof(bfh) + sizeof(bih), sizeof(rgbquad), rgbquad, sizeof(rgbquad));
	memcpy_s((char*)stream + sizeof(bfh) + sizeof(bih) + sizeof(rgbquad), width * height * 3, rgbbuf, width * height * 3);
}

