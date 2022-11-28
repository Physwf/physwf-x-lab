#pragma once

#include "Windows.h"
#include <memory>
#include <vector>

class InfMipMap;

class InfBitmap
{
public:
	InfBitmap();
	InfBitmap(UINT64 Width, UINT64 Height);
	InfBitmap(UINT64 Width, UINT64 Height, BYTE* Data);
	InfBitmap(UINT64 Width, UINT64 Height, std::unique_ptr<BYTE[]> Data);

	~InfBitmap();

	static InfBitmap* CreateFromBmp(const char* filename);
public:
	UINT64 Width() const;
	UINT64 Height() const;

	void Reconstruct(UINT64 Width, UINT64 Height, std::unique_ptr<BYTE[]> Data);
	void Render(double dbScale, int X, int Y, UINT32 ScreenW, UINT32 ScreenH, std::unique_ptr<BYTE[]>& OutData);
private:
	void GenerateMipMap();
	UINT64 CeilToPowerOf2(UINT64 Value);
	UINT64 CeilToTimesOf4(UINT64 Value);
public:
	static const UINT64 MipMapThreshold;
private:
	std::vector<InfMipMap*> m_vecMips;
};