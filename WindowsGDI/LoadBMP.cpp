#include "framework.h"
#include "Windowsx.h"

void LoadBitmap(const char* filename, BYTE** pBuffer, BITMAPINFO** bmInfo)
{
	BITMAPFILEHEADER bfh;
	//BITMAPINFOHEADER bih;
	HANDLE hFile = CreateFileA(filename, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == INVALID_HANDLE_VALUE) return;

	DWORD Size;
	DWORD FileSize = GetFileSize(hFile, NULL);
	BYTE* TempBuffer = new BYTE[FileSize];
	ReadFile(hFile, TempBuffer, FileSize, &Size, NULL);
	BYTE* TempBufferCopy = TempBuffer;
	bfh = *(BITMAPFILEHEADER*)TempBufferCopy;
	TempBufferCopy += sizeof(BITMAPFILEHEADER);
	*bmInfo = (BITMAPINFO*)TempBufferCopy;
	//bih = *(BITMAPINFOHEADER*)(TempBufferCopy);
	//TempBufferCopy += sizeof(BITMAPINFOHEADER);

	if ((*bmInfo)->bmiHeader.biBitCount == 8)
	{
		SIZE_T infoSize = sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 256;
		//*bmInfo = (BITMAPINFO*)new BYTE[infoSize];
		//memcpy(*bmInfo, TempBufferCopy, infoSize);
		TempBufferCopy += infoSize;
	}
	DWORD Width = (*bmInfo)->bmiHeader.biWidth;
	if (Width % 4 != 0)
	{
		Width = (Width & (~3ull)) + 4;
	}
	//(*bmInfo)->bmiHeader.biSizeImage = (*bmInfo)->bmiHeader.biWidth * (*bmInfo)->bmiHeader.biHeight;
	//int bytes = bih.biBitCount / 8;
	//UINT64 size = (UINT64)bih.biWidth * (UINT64)bih.biHeight * (UINT64)bih.biBitCount / 8;
	*pBuffer = TempBufferCopy;

	CloseHandle(hFile);
}


void FillGreyHeader(int width, int height, BITMAPFILEHEADER* pbfh, BITMAPINFOHEADER* pbih, RGBQUAD* prgbquad, int rgbquadsize)
{
	pbfh->bfType = 0x4d42;
	pbfh->bfReserved1 = 0;
	pbfh->bfReserved2 = 0;
	pbfh->bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * rgbquadsize + width * height;
	pbfh->bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) +  sizeof(RGBQUAD) * rgbquadsize;

	pbih->biSize = sizeof(BITMAPINFOHEADER);
	pbih->biWidth = width;
	pbih->biHeight = height;
	pbih->biPlanes = 1;
	pbih->biBitCount = 8;
	pbih->biCompression = BI_RGB;
	pbih->biSizeImage = width * height;
	pbih->biXPelsPerMeter = 0;
	pbih->biYPelsPerMeter = 0;
	pbih->biClrUsed = rgbquadsize;
	pbih->biClrImportant = 0;

	//µ÷É«°å
	int i;
	for (i = 0; i < rgbquadsize; i++)
	{
		prgbquad[i].rgbBlue = i;
		prgbquad[i].rgbGreen = i;
		prgbquad[i].rgbRed = i;
		prgbquad[i].rgbReserved = 0;
	}
}

void BMP_WritePixels_Gray(const char* filename,  int width, int height, unsigned char* rgbbuf, int buffersize)
{
	BITMAPFILEHEADER bfh;
	BITMAPINFOHEADER bih;
	RGBQUAD rgbquad[256];

	FillGreyHeader(width, height, &bfh, &bih, rgbquad, 256);

	HANDLE hFile = CreateFileA(filename, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == INVALID_HANDLE_VALUE) return;

	DWORD numOfBytesWriten = 0;
	WriteFile(hFile, &bfh, sizeof(bfh), &numOfBytesWriten, NULL);
	WriteFile(hFile, &bih, sizeof(bih), &numOfBytesWriten, NULL);
	WriteFile(hFile, &rgbquad[0], sizeof(rgbquad), &numOfBytesWriten, NULL);
	WriteFile(hFile, rgbbuf, buffersize, &numOfBytesWriten, NULL);

	CloseHandle(hFile);
}