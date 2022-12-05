#include "pch.h"
#include "framework.h"

#include <memory>
#include <functional>

void FillGreyHeader(int width, int height, BITMAPFILEHEADER* pbfh, BITMAPINFOHEADER* pbih, RGBQUAD* prgbquad, int rgbquadsize)
{
	pbfh->bfType = 0x4d42;
	pbfh->bfReserved1 = 0;
	pbfh->bfReserved2 = 0;
	pbfh->bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * rgbquadsize + width * height;
	pbfh->bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * rgbquadsize;

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

int GetBitmapInfo(const char* filename, long* Width, long* Height, unsigned short* BitCount, unsigned long long* DataSize)
{
	std::unique_ptr <void, std::function<void(HANDLE)>> FileHandle(nullptr, [](HANDLE h) { CloseHandle(h); });
	HANDLE hFile = CreateFileA(filename, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == INVALID_HANDLE_VALUE) return 1;
	FileHandle.reset(hFile);

	size_t Size = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFO);
	DWORD bytesRead = 0;
	std::unique_ptr<BYTE[]> TempBuffer = std::unique_ptr<BYTE[]>(new BYTE[Size]);
	BOOL bSuccess = ReadFile(hFile, TempBuffer.get(), (DWORD)Size, &bytesRead, NULL);
	if (!bSuccess) return 2;

	BYTE* TempBufferCopy = TempBuffer.get();
	TempBufferCopy += sizeof(BITMAPFILEHEADER);
	BITMAPINFO bmInfo = *(BITMAPINFO*)TempBufferCopy;
	*Width = bmInfo.bmiHeader.biWidth;
	*Height = bmInfo.bmiHeader.biHeight;
	*BitCount = bmInfo.bmiHeader.biBitCount;

	DWORD RoundWidth = *Width;
	if (RoundWidth % 4 != 0)
	{
		RoundWidth = (RoundWidth & (~3ull)) + 4;
	}
	*DataSize = (size_t)RoundWidth * (size_t)*Height * (size_t)(*BitCount / 8);
	return 0;
}

int ReadBitmap(const char* filename, void* Buffer, size_t BufferSize)
{
	//BITMAPFILEHEADER bfh;
	std::unique_ptr <void, std::function<void(HANDLE)>> FileHandle(nullptr, [](HANDLE h) {CloseHandle(h); });
	HANDLE hFile = CreateFileA(filename, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == INVALID_HANDLE_VALUE) return 1;
	FileHandle.reset(hFile);

	DWORD FileSize = GetFileSize(hFile, NULL);
	DWORD Size = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	DWORD bytesRead = 0;
	std::unique_ptr<BYTE[]> TempBuffer = std::unique_ptr<BYTE[]>(new BYTE[Size]);
	BOOL bSuccess = ReadFile(hFile, TempBuffer.get(), Size, &bytesRead, NULL);
	if (!bSuccess) return 2;

	BYTE* TempBufferCopy = TempBuffer.get();
	TempBufferCopy += sizeof(BITMAPFILEHEADER);
	BITMAPINFOHEADER bmiHeader = *(BITMAPINFOHEADER*)TempBufferCopy;
	size_t Width = bmiHeader.biWidth;
	size_t Height = bmiHeader.biHeight;
	size_t BitCount = bmiHeader.biBitCount;

	size_t Offset = Size;
	if (BitCount == 8)
	{
		SIZE_T infoSize = sizeof(RGBQUAD) * 256;
		Offset += infoSize;
	}
	if (FileSize - Offset > BufferSize) return 2;

	SetFilePointer(hFile, (LONG)Offset, NULL, FILE_BEGIN);
	bSuccess = ReadFile(hFile, Buffer, FileSize - (DWORD)Offset, &Size, NULL);
	if (!bSuccess) return 3;
	
	return 0;
}

int WriteBitmap(const char* filename, void* Buffer, size_t BufferSize, long Width, long Height, int Format)
{
	BITMAPFILEHEADER bfh;
	BITMAPINFOHEADER bih;
	RGBQUAD rgbquad[256];

	FillGreyHeader(Width, Height, &bfh, &bih, rgbquad, 256);


	std::unique_ptr <void, std::function<void(HANDLE)>> FileHandle(nullptr, [](HANDLE h) {CloseHandle(h); });
	HANDLE hFile = CreateFileA(filename, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == INVALID_HANDLE_VALUE) return 1;
	FileHandle.reset(hFile);

	DWORD numOfBytesWriten = 0;
	WriteFile(hFile, &bfh, sizeof(bfh), &numOfBytesWriten, NULL);
	WriteFile(hFile, &bih, sizeof(bih), &numOfBytesWriten, NULL);
	WriteFile(hFile, &rgbquad[0], sizeof(rgbquad), &numOfBytesWriten, NULL);
	WriteFile(hFile, Buffer, (DWORD)BufferSize, &numOfBytesWriten, NULL);

	return 0;
}
