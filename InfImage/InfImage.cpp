#include "InfImage.h"
#include "InfBitmap.h"

InfImage::InfImage() 
	: mX(0)
	, mY(0)
	, mWidth(1)
	, mHeight(1)
	, mScale(1.0f)
	, mBitmap(nullptr)
{}

InfImage::InfImage(InfBitmap* pBitmap) 
	: mX(0)
	, mY(0)
	, mWidth(pBitmap->Width())
	, mHeight(pBitmap->Height())
	, mScale(1.0f)
	, mBitmap(pBitmap)
{

}

bool InfImage::HitTest(int X, int Y)
{
	return (X >= mX && X < (mX + mWidth * mScale) && Y >= mY && Y < (mY + mHeight * mScale));
}

void InfImage::Render(HWND hWnd)
{
	RECT rect;
	GetWindowRect(hWnd, &rect);
	std::unique_ptr<BYTE[]> Data;
	DWORD ScreenW = rect.right - rect.left;
	DWORD ScreenH = rect.bottom - rect.top;
	extern int g_MouseX;
	extern int g_MouseY;
	double dbOffsetX = double(g_MouseX - mX) / Width();
	double dbOffsetY = double(g_MouseY - mY) / Height();
	UINT64 OutW = 0, OutH = 0;
	printf("mX:%d mY:%d dbOffsetX:%f dbOffsetY:%f g_MouseX:%d g_MouseY:%d\n", mX, mY, dbOffsetX, dbOffsetY, g_MouseX, g_MouseY);
	//mBitmap->Scale(mScale, dbOffsetX, dbOffsetY, g_MouseX,g_MouseY, ScreenW, ScreenH, OutW, OutH, Data);
	mBitmap->Render(mScale, mX, mY, ScreenW, ScreenH, Data);
	std::unique_ptr<BYTE[]> arr = std::unique_ptr<BYTE[]>(new BYTE[sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 256]);
	BITMAPINFO* info = (BITMAPINFO*)arr.get();
	//memcpy(info, bmInfo, sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 256);
	info->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	info->bmiHeader.biPlanes = 1;
	info->bmiHeader.biBitCount = 8;
	info->bmiHeader.biCompression = 0;
	info->bmiHeader.biXPelsPerMeter = 0;
	info->bmiHeader.biYPelsPerMeter = 0;
	info->bmiHeader.biClrUsed = 256;
	info->bmiHeader.biClrImportant = 0;
	info->bmiHeader.biWidth = ScreenW;
	info->bmiHeader.biHeight = ScreenH;
	info->bmiHeader.biSizeImage = ScreenW * ScreenH;
	RGBQUAD* prgbquad = info->bmiColors;
	for (int i = 0; i < 256; i++)
	{
		prgbquad[i].rgbBlue = i;
		prgbquad[i].rgbGreen = i;
		prgbquad[i].rgbRed = i;
		prgbquad[i].rgbReserved = 0;
	}
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hWnd, &ps);
	SetDIBitsToDevice(hdc,
		0, 0, ScreenW, ScreenH,
		0,//xSrc
		0,//ySrc
		0,
		ScreenH,
		Data.get(), info, DIB_RGB_COLORS);
	EndPaint(hWnd, &ps);
}
