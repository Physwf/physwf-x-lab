#include "pch.h"
#include "framework.h"

#include "IO.h"

#include <memory>
#include <functional>

int CombineImages(const char** files, int fileCount, const char* outname, int OverlapX, int OverlapY, int dir /*= 0*/)
{
	LONG TotalWidth = 0;
	LONG MaxHeight = 0;
	std::unique_ptr<LONG[]> WidthList = std::unique_ptr<LONG[]>(new LONG[fileCount]);
	std::unique_ptr<LONG[]> HeightList = std::unique_ptr<LONG[]>(new LONG[fileCount]);
	std::unique_ptr<size_t[]> DataSizeList = std::unique_ptr<size_t[]>(new size_t[fileCount]);
	for (int i = 0; i < fileCount; ++i)
	{
		const char* file = files[i];
		LONG Width, Height;
		WORD BitCount;
		size_t DataSize;
		if (0 == GetBitmapInfo(file, &Width, &Height, &BitCount, &DataSize))
		{
			WidthList[i] = Width;
			HeightList[i] = Height;
			DataSizeList[i] = DataSize;
			TotalWidth += Width;
			if (MaxHeight < Height) MaxHeight = Height;
		}
		else return 1;
	}
	LONG CombinedPixelWidth = TotalWidth - OverlapX * 2 * fileCount;
	TotalWidth = CombinedPixelWidth;
	if (TotalWidth % 4 != 0)
	{
		TotalWidth = (TotalWidth & (~3ull)) + 4;
	}
	MaxHeight -= OverlapY * 2;
	size_t TotalSize = (size_t)TotalWidth * (size_t)MaxHeight;
	size_t WidthOffset = 0;
	std::unique_ptr<BYTE[]> CombinedPixelData = std::unique_ptr<BYTE[]>(new BYTE[TotalSize]);
	for (int i = 0; i < fileCount; ++i)
	{
		const char* file = files[i];
		size_t DataSize = DataSizeList[i];
		std::unique_ptr<BYTE[]> PixelData = std::unique_ptr<BYTE[]>(new BYTE[DataSize]);
		if (0 == ReadBitmap(file, PixelData.get(), DataSize))
		{
			LONG Width = WidthList[i];
			LONG CopyWidth = Width - OverlapX * 2;
			LONG Height = HeightList[i];
			size_t RoundWidth = Width;
			if (RoundWidth % 4 != 0)
			{
				RoundWidth = (RoundWidth & (~3ull)) + 4;
			}
			LONG CopyHeight = Height - 2 * OverlapY;
			size_t TW = (size_t)TotalWidth;
			printf("file index=%d Width=%d CopyWidth=%d RoundWidth=%I64u Height=%d CopyHeight=%d\n",i, Width, CopyWidth, RoundWidth, Height, CopyHeight);
			for (LONG y = 0; y < CopyHeight; ++y)
			{
				size_t Y = (size_t)y;
				size_t Dst = WidthOffset + (TW * Y);
				size_t Src = OverlapX + RoundWidth * ((size_t)y + (size_t)OverlapY);
				printf("Dst=%I64u Src=%I64u\n", Dst, Src);
				memcpy(CombinedPixelData.get() + Dst, PixelData.get() + Src, CopyWidth);
				//memcpy(CombinedPixelData.get() + WidthOffset + ((size_t)TotalWidth * (size_t)y), PixelData.get() + OverlapX + RoundWidth * ((size_t)y + (size_t)OverlapY), CopyWidth);
			}
			WidthOffset += CopyWidth;
		}
		else return 2;
	}

	if (0 != WriteBitmap(outname, CombinedPixelData.get(), TotalSize, CombinedPixelWidth, MaxHeight, 0))
	{
		return 3;
	}
	return 0;
}
