#include "InfBitmap.h"


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

void BMP_WritePixels_Gray(const char* filename, int width, int height, unsigned char* rgbbuf, int buffersize)
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

class InfMipMap
{
public:
	InfMipMap(UINT64 Width, UINT64 Height, BYTE* Data)
		: m_uWidth(Width)
		, m_uHeight(Height)
		, m_Data(Data) { }

	InfMipMap(UINT64 Width, UINT64 Height, std::unique_ptr<BYTE[]> Data)
		: m_uWidth(Width)
		, m_uHeight(Height)
		, m_Data(std::move(Data)) { }

	UINT64 Width() const { return m_uWidth; }
	UINT64 Height() const { return m_uHeight; }
	BYTE* Data() const { return m_Data.get(); }

	BYTE Pixel(UINT64 X, UINT64 Y) const
	{
		return m_Data.get()[Y * m_uWidth + X];
	}

	BYTE& Pixel(UINT64 X, UINT64 Y)
	{
		return m_Data.get()[Y * m_uWidth + X];
	}
private:
	UINT64 m_uWidth;
	UINT64 m_uHeight;
	std::unique_ptr<BYTE[]> m_Data;
};

InfBitmap::InfBitmap()
{
	m_vecMips.push_back(new InfMipMap(1,1,new BYTE[1]));
}

InfBitmap::InfBitmap(UINT64 Width, UINT64 Height)
{
	if (Width % 4 != 0)
	{
		Width = (Width & (~3ull)) + 4;
	}
	m_vecMips.push_back(new InfMipMap(Width , Height, new BYTE[Width * Height]));
}

InfBitmap::InfBitmap(UINT64 Width, UINT64 Height, BYTE* Data)
{
	Reconstruct(Width, Height, std::unique_ptr<BYTE[]>(Data));
}


InfBitmap::InfBitmap(UINT64 Width, UINT64 Height, std::unique_ptr<BYTE[]> Data)
{
	Reconstruct(Width, Height, std::move(Data));
}

InfBitmap::~InfBitmap()
{

}

InfBitmap* InfBitmap::CreateFromBmp(const char* filename)
{
	BITMAPFILEHEADER bfh;
	//BITMAPINFOHEADER bih;
	HANDLE hFile = CreateFileA(filename, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == INVALID_HANDLE_VALUE) return nullptr;

	DWORD Size;
	DWORD FileSize = GetFileSize(hFile, NULL);
	BYTE* FileData = new BYTE[FileSize];
	if (ReadFile(hFile, FileData, FileSize, &Size, NULL))
	{
		CloseHandle(hFile);
	}

	BYTE* TempHeader = FileData;
	bfh = *(BITMAPFILEHEADER*)TempHeader;
	TempHeader += sizeof(BITMAPFILEHEADER);
	BITMAPINFO* bmInfo;
	bmInfo = (BITMAPINFO*)TempHeader;
	//bih = *(BITMAPINFOHEADER*)(TempBufferCopy);
	//TempBufferCopy += sizeof(BITMAPINFOHEADER);

	if ((bmInfo)->bmiHeader.biBitCount == 8)
	{
		SIZE_T infoSize = sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 256;
		//*bmInfo = (BITMAPINFO*)new BYTE[infoSize];
		//memcpy(*bmInfo, TempBufferCopy, infoSize);
		TempHeader += infoSize;
	}
	DWORD Width = (bmInfo)->bmiHeader.biWidth;
	DWORD Height = (bmInfo)->bmiHeader.biHeight;
	if (Width % 4 != 0)
	{
		Width = (Width & (~3ull)) + 4;
	}
	BYTE* PixelData = TempHeader;
	BYTE* SwapBuffer = new BYTE[Width];
	for (DWORD h = 0; h < Height; ++h)
	{
		DWORD hh = Height - h - 1;
		if(h >= hh) break;
		memcpy(SwapBuffer, PixelData + h * Width, Width);
		memcpy(PixelData + h * Width, PixelData + hh * Width, Width);
		memcpy(PixelData + hh * Width, SwapBuffer, Width);
	}
	delete[] SwapBuffer;

	return new InfBitmap(bmInfo->bmiHeader.biWidth, bmInfo->bmiHeader.biHeight, PixelData);
}

UINT64 InfBitmap::Width() const
{
	return m_vecMips[0]->Width();
}

UINT64 InfBitmap::Height() const
{
	return m_vecMips[0]->Height();
}

void InfBitmap::Reconstruct(UINT64 Width, UINT64 Height, std::unique_ptr<BYTE[]> Data)
{
	m_vecMips.push_back(new InfMipMap(Width, Height, std::move(Data)));
	if (Width > MipMapThreshold || Height > MipMapThreshold)
	{
		GenerateMipMap();
	}
}

void InfBitmap::Render(double dbScale, int X, int Y, UINT32 ScreenW, UINT32 ScreenH, std::unique_ptr<BYTE[]>& OutData)
{
	if (dbScale > 1.0)
	{
		dbScale = 1.0;
	}
	UINT32 Level = 0;
	for (; Level < m_vecMips.size() - 1; ++Level)
	{
		double MipScale = 1.0 / (1 << Level);
		if (dbScale >= MipScale) break;
	}
	if (Level == m_vecMips.size() - 1)
	{
		dbScale = 1.0 / (1 << Level);//Min Scale
	}
	OutData.reset(new BYTE[ScreenW * ScreenH]);
	UINT64 W = dbScale * m_vecMips[0]->Width();
	UINT64 H = dbScale * m_vecMips[0]->Height();

	int SrcStartX = -X;
	int SrcStartY = -Y;
	int SrcEndX = SrcStartX + ScreenW;
	int SrcEndY = SrcStartY + ScreenH;
	if (SrcStartX < 0) SrcStartX = 0;
	if (SrcStartY < 0) SrcStartY = 0;
	if (SrcStartX > W) SrcStartX = W;
	if (SrcStartY > H) SrcStartY = H;
	if (SrcEndX > W) SrcEndX = W;
	if (SrcEndY > H) SrcEndY = H;

	int DstStartX = X;
	int DstStartY = Y;
	if (DstStartX < 0) DstStartX = 0;
	if (DstStartY < 0) DstStartY = 0;
	if (DstStartX > ScreenW) DstStartX = ScreenW;
	if (DstStartY > ScreenH) DstStartY = ScreenH;
	int DstEndX = ScreenW;
	int DstEndY = ScreenH;

	printf("SrcStartX:%d SrcStartY:%d, SrcEndX:%d, SrcEndY:%d\n", SrcStartX, SrcStartY, SrcEndX, SrcEndY);
	printf("DstStartX:%d DstStartY:%d, DstEndX:%d, DstEndY:%d\n", DstStartX, DstStartY, DstEndX, DstEndY);
	printf("----------------\n");

	if (dbScale == 1.0 / (1 << Level))
	{
		InfMipMap* MipMap = m_vecMips[Level];
		int CopyLen = SrcEndX - SrcStartX;
		for (int y = 0; y < DstEndY - DstStartY; ++y)
		{
			memcpy(OutData.get() + (ScreenH - y - DstStartY - 1) * ScreenW + DstStartX, MipMap->Data() + SrcStartX + (y + SrcStartY) * MipMap->Width(), CopyLen);
			//memset(Data.get() + y * ScreenW, 0xff, 100);
		}
		return;
	}
	double DownScale = 1.0 / (1 << Level);
	double UpScale = 1.0 / (1 << (Level - 1));

	InfMipMap* DownMipMap = m_vecMips[Level];
	InfMipMap* UpMipMap = m_vecMips[Level - 1];
	for (int y = DstStartY; y < DstEndY; ++y)
	{
		int DownY = DownMipMap->Height() * double(y - Y) / double(H);
		int UpY = UpMipMap->Height() * double(y - Y) / double(H);
		for (int x = 0; x < ScreenW; ++x)
		{
			int DownX = DownMipMap->Width() * double(x-X) / double(W);
			int UpX = UpMipMap->Width() * double(x - X) / double(W);
			if (DownX >= DownMipMap->Width() || DownY >= DownMipMap->Height()) continue;
			BYTE DownPixel = DownMipMap->Pixel(DownX, DownY);
			BYTE UpPixel = UpMipMap->Pixel(UpX, UpY);
			double t = (dbScale - DownScale) / (UpScale - DownScale);
			OutData[x + (ScreenH - y - 1) * ScreenW] = (1.0 - t) * DownPixel + t * UpPixel;
			//printf("x:%d,y:%d, pixel:%d", (int)x, (int)y, Data[x + (ScreenH - y - 1) * ScreenW]);
		}
		//printf("\n");
	}
	//BMP_WritePixels_Gray("D:/scaletest.bmp", ScreenW, ScreenH, Data.get(), ScreenW * ScreenH);
}

void InfBitmap::GenerateMipMap()
{
	UINT64 LastMipMapWitdh = CeilToTimesOf4(m_vecMips[0]->Width());
	UINT64 LastMipMapHeight = /*CeilToTimesOf4*/(m_vecMips[0]->Height());
	UINT64 LastWitdh = m_vecMips[0]->Width();
	UINT64 LastHeight = m_vecMips[0]->Height();
	while (true)
	{
		UINT64 Level = 1;
		UINT64 MipMapWidth = CeilToTimesOf4(LastMipMapWitdh >> 1);
		UINT64 MipMapHeight = /*CeilToTimesOf4*/(LastMipMapHeight >> 1);

		if (MipMapWidth <= 0) MipMapWidth = 1;
		if (MipMapHeight <= 0) MipMapHeight = 1;

		BYTE* LastData = m_vecMips.back()->Data();
		m_vecMips.push_back(new InfMipMap(MipMapWidth, MipMapHeight, new BYTE[MipMapWidth * MipMapHeight]));
		BYTE* CurData = m_vecMips.back()->Data();
		for (UINT64 y = 0; y < MipMapHeight; ++y)
		{
			for (UINT64 x = 0; x < MipMapWidth; ++x)
			{
				UINT64 LastX = 2 * x;
				UINT64 LastY = 2 * y;
				UINT64 Last00 = LastX + LastY * LastMipMapWitdh;
				UINT64 Last10 = LastX + LastY * LastMipMapWitdh + 1;
				UINT64 Last01 = LastX + (LastY + 1) * LastMipMapWitdh;
				UINT64 Last11 = LastX + (LastY + 1) * (LastMipMapWitdh)+1;
				UINT32 Count = 1;
				UINT16 Sum = 0;
				if (LastX < LastWitdh && LastY < LastHeight)
				{
					Sum += LastData[Last00];
				}
				if (LastX + 1 < LastWitdh)
				{
					Count++;
					Sum += LastData[Last10];
				}
				if (LastY + 1 < LastHeight)
				{
					Count++;
					Sum += LastData[Last01];
				}
				if (LastX + 1 < LastWitdh && LastY + 1 < LastHeight)
				{
					Count++;
					Sum += LastData[Last11];
				}
				CurData[x + y * MipMapWidth] = Sum / Count;
			}
		}

		if (MipMapWidth < 256 && MipMapHeight < 256)
			break;

		LastMipMapWitdh = MipMapWidth;
		LastMipMapHeight = MipMapHeight;
		LastWitdh = LastWitdh >> 1;
		LastHeight = LastHeight >> 1;
	}
}

UINT64 InfBitmap::CeilToPowerOf2(UINT64 Value)
{
	int HighestBit = 63;
	while (((1ull << HighestBit) & Value) == 0)
	{
		HighestBit--;
	}
	return 1ull << (HighestBit+1);
}

UINT64 InfBitmap::CeilToTimesOf4(UINT64 Value)
{
	if (Value % 4 != 0)
	{
		Value = (Value & (~3ull)) + 4;
	}
	return Value;
}

const UINT64 InfBitmap::MipMapThreshold = 256;



