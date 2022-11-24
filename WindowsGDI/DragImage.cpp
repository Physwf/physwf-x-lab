#include "framework.h"
#include "Windowsx.h"
#include <assert.h>
#include <uxtheme.h>
#pragma comment (lib, "uxtheme.lib")

#include <stdio.h>

BYTE* bmData;
BITMAPINFO* bmInfo;
extern int g_OffsetX, g_OffsetY;
extern int g_DargStartX, g_DragStartY;
extern int g_DragOffsetX, g_DragOffsetY;
extern bool g_bDraging;
extern double g_dbScale;

extern void LoadBitmap(const char* filename, BYTE** pBuffer, BITMAPINFO** bmInfo);

void DragInit()
{
	//LoadBitmap("D:/1_1_1_0_0_1_16.bmp", &bmData, &bmInfo);
	//LoadBitmap("D:/1-11.bmp", &bmData, &bmInfo);
	LoadBitmap("D:/jianbian.bmp", &bmData, &bmInfo);
	//LoadBitmap("D:/1_1_1_0_0_1_16-1.bmp", &bmData, &bmInfo);
	//LoadBitmap("D:/remap.bmp", &bmData, &bmInfo);
	//LoadBitmap("D:/big.bmp", &bmData, &bmInfo);
	//LoadBitmap("D:/normal.bmp", &bmData, &bmInfo);

	AllocConsole(); //#include "winsock2.h"
	//wchar_t title[] =  L"Debug Output";
	SetConsoleTitleA("Debug Output"); // 设置控制台窗口的标题
	FILE* F;
	freopen_s(&F, "CONOUT$", "w", stdout); // 重定向输出
	printf("%s\n\n", "*** ** *** Console *** ** ***");
}

void DragImage_SetDIBitsToDevice(HWND hWnd)
{
	RECT rect;
	GetWindowRect(hWnd, &rect);
	LONG Width = rect.right - rect.left;
	LONG Height = rect.bottom - rect.top;
	HDC hdc = GetDC(hWnd);
	//StretchDIBits(hdc, 0, 0, rect.right - rect.left, rect.bottom - rect.top, OffsetX + DragOffsetX, -(OffsetY + DragOffsetY), rect.right - rect.left, rect.bottom - rect.top, bmData, bmInfo, DIB_RGB_COLORS, SRCCOPY);
	SetDIBitsToDevice(hdc,
		0, 0, Width, Height,
		-(g_OffsetX + g_DragOffsetX),//xSrc
		bmInfo->bmiHeader.biHeight - Height + (g_OffsetY + g_DragOffsetY), //ySrc
		0,
		bmInfo->bmiHeader.biHeight,//rect.bottom - rect.top, 
		bmData, bmInfo, DIB_RGB_COLORS);
}

void DragImage_BitBlt(HWND hWnd)
{
	RECT rect;
	GetWindowRect(hWnd, &rect);
	LONG Width = rect.right - rect.left;
	LONG Height = rect.bottom - rect.top;
	HDC hdc = GetDC(hWnd);
	HDC memHDC = CreateCompatibleDC(hdc);
	HBITMAP hbitmap = CreateCompatibleBitmap(hdc, Width, Height);
	HGDIOBJ oldbmp = SelectObject(memHDC, hbitmap);

	SetDIBitsToDevice(memHDC,
		0, 0, Width, Height,
		-(g_OffsetX + g_DragOffsetX),
		bmInfo->bmiHeader.biHeight - Width + (g_OffsetY + g_DragOffsetY), /*+ rect.bottom - rect.top*//*,*/
		0,
		bmInfo->bmiHeader.biHeight,//rect.bottom - rect.top, 
		bmData, bmInfo, DIB_RGB_COLORS);

	DWORD Error = GetLastError();
	BOOL bSuccess = BitBlt(hdc, 0, 0, Width, Height, memHDC, 0, 0, SRCCOPY); // blt from bitmap to screen
	SelectObject(memHDC, oldbmp);
	DeleteObject(hbitmap);
	DeleteDC(memHDC);
}

void DragImage_BitBlt2(HWND hWnd)
{
	RECT rect;
	GetWindowRect(hWnd, &rect);
	HDC hdc = GetDC(hWnd);

	RECT rc = {  0,  0,  rect.right - rect.left,  rect.bottom - rect.top};

	HDC memdc;
	auto hbuff = BeginBufferedPaint(hdc, &rc, BPBF_COMPATIBLEBITMAP, NULL, &memdc);
	LARGE_INTEGER Freq, StartCounter, EndCounter;
	QueryPerformanceFrequency(&Freq);
	QueryPerformanceCounter(&StartCounter);
	SetDIBitsToDevice(memdc,
		0, 0, rect.right - rect.left, rect.bottom - rect.top,
		-(g_OffsetX + g_DragOffsetX),
		bmInfo->bmiHeader.biHeight - (rect.bottom - rect.top) + (g_OffsetY + g_DragOffsetY), /*+ rect.bottom - rect.top*//*,*/
		0,//-(OffsetY + DragOffsetY) * 4800 - (OffsetX + DragOffsetX), 
		bmInfo->bmiHeader.biHeight,//rect.bottom - rect.top, 
		bmData, bmInfo, DIB_RGB_COLORS);

	EndBufferedPaint(hbuff, TRUE);

	QueryPerformanceCounter(&EndCounter);
	double Interval = (EndCounter.QuadPart - StartCounter.QuadPart) * 1000.0 / Freq.QuadPart;
	printf("%f\n", Interval);
}

void DragImage_StretchBlt(HWND hWnd)
{
	RECT rect;
	GetWindowRect(hWnd, &rect);
	LONG Width = rect.right - rect.left;
	LONG Height = rect.bottom - rect.top;
	int StartX = -(g_OffsetX + g_DragOffsetX);
	int StartY = -(g_OffsetY + g_DragOffsetY);

	LONG SrcWitdh = g_dbScale * Width;
	LONG SrcHeight = g_dbScale * Height;
	HDC hdc = GetDC(hWnd);
	HDC memHDC = CreateCompatibleDC(hdc);
	HBITMAP hbitmap = CreateCompatibleBitmap(hdc, SrcWitdh, SrcHeight);
	HGDIOBJ oldbmp = SelectObject(memHDC, hbitmap);

	SetDIBitsToDevice(memHDC,
		0, 0, SrcWitdh, SrcHeight,
		-(g_OffsetX + g_DragOffsetX),
		bmInfo->bmiHeader.biHeight - Width + (g_OffsetY + g_DragOffsetY), /*+ rect.bottom - rect.top*//*,*/
		0,
		bmInfo->bmiHeader.biHeight,//rect.bottom - rect.top, 
		bmData, bmInfo, DIB_RGB_COLORS);

	DWORD Error = GetLastError();
	BOOL bSuccess = StretchBlt(hdc, 0, 0, Width, Height, memHDC, 0, 0, SrcWitdh, SrcHeight, SRCCOPY); // blt from bitmap to screen
	SelectObject(memHDC, oldbmp);
	DeleteObject(hbitmap);
	DeleteDC(memHDC);
}

void DragImage_Manual(HWND hWnd)
{
	LARGE_INTEGER Freq, StartCounter, EndCounter;
	QueryPerformanceFrequency(&Freq);
	QueryPerformanceCounter(&StartCounter);

	RECT rect;
	GetWindowRect(hWnd, &rect);
	HDC hdc = GetDC(hWnd);
	LONG Width = rect.right - rect.left;
	LONG Height = rect.bottom - rect.top;
	int StartX = -(g_OffsetX + g_DragOffsetX);
	int StartY = -(g_OffsetY + g_DragOffsetY);
	//printf("g_DragOffsetX:%05d,g_OffsetX:%05d;\n", g_DragOffsetX, g_OffsetX);
	//printf("g_DragOffsetY:%05d,g_OffsetY:%05d\n", g_DragOffsetY, g_OffsetY);

	//return;
	BYTE* BitmapBuffer = new BYTE[Width * Height]();


	//if (StartX > bmInfo->bmiHeader.biWidth) return;
	//if (StartY > bmInfo->bmiHeader.biHeight) return;

	//if (StartX < -Width) StartX = -Width;
	//if (StartY < -Height) StartY = -Height;

	int EndX = StartX + Width;
	int EndY = StartY + Height;

	//if (StartX < 0) StartX = 0;
	//if (StartY < 0) StartY = 0;

	//if (EndX > bmInfo->bmiHeader.biWidth) EndX = bmInfo->bmiHeader.biWidth;
	//if (EndY > bmInfo->bmiHeader.biHeight) EndY = bmInfo->bmiHeader.biHeight;

	DWORD bmWidth = bmInfo->bmiHeader.biWidth;
	DWORD bmHeight = bmInfo->bmiHeader.biHeight;

	if (bmWidth % 4 != 0)
	{
		bmWidth = (bmWidth & (~3ull)) + 4;
	}

	printf("StartX:%d,EndX:%d;\n", StartX,  EndX);
	printf("StartY:%d,EndY:%d\n",  StartY, EndY);

	int CopyLen = EndX - StartX;
	int CopySrcStartX = StartX, CopySrcStartY = StartY;
	int CopySrcEndX = EndX, CopySrcEndY = EndY;
	if (CopySrcStartX < 0) CopySrcStartX = 0;
	if (CopySrcStartY < 0) CopySrcStartY = 0;
	if (CopySrcEndX > bmWidth) CopySrcEndX = bmWidth;
	if (CopySrcEndY > bmHeight) CopySrcEndY = bmHeight;

	int CopyDstStartX = 0, CopyDstStartY = 0;
	int CopyDstEndX = Width, CopyDstEndY = Height;
	if (StartX < 0) CopyDstStartX = -StartX;
	//if (StartY < 0) CopyDstStartY = -StartY;
	if (EndX > bmWidth)
	{
		CopyDstEndX = StartX + bmWidth;
	}
	if (EndY > bmHeight)
	{
		CopyDstEndY = StartY + bmHeight;
		CopyDstStartY = EndY - bmHeight;
	}

	printf("CopySrcStartX:%d,CopySrcStartY:%d;\n", CopySrcStartX, CopySrcStartY);
	printf("CopySrcEndX:%d,CopySrcEndY:%d\n", CopySrcEndX, CopySrcEndY);
	printf("CopyDstStartX:%d,CopyDstStartY:%d\n", CopyDstStartX, CopyDstStartY);
	printf("CopyDstEndX:%d,CopyDstEndY:%d\n", CopyDstEndX, CopyDstEndY);
	printf("--------------\n");

	//return;
	for (int y = CopySrcStartY; y < CopySrcEndY; ++y)
	{
		int CopySize = (CopySrcEndX - CopySrcStartX);
		int dy = y - CopySrcStartY;
		memcpy(BitmapBuffer + CopyDstStartX + Width * (dy + CopyDstStartY) , bmData + CopySrcStartX + ((bmHeight - CopySrcEndY) + dy) * bmWidth, CopySize);
		//printf("CopyDstX:%d\n", CopyDstStartX + Width * y);
		//printf("CopySrcX:%d\n", CopySrcStartX + (bmHeight - CopySrcEndY + CopySrcEndY - y) * bmWidth);
		//printf("Copy Length:%d\n", CopySrcEndX - CopySrcStartX);
		//printf("====================\n");
	}
	for (int h = 0; h < EndY - StartY; ++h)
	{
		//memcpy(BitmapBuffer + (h) * Width + (Width - CopyLen), bmData + StartX + (bmHeight - EndY  + h) * bmWidth, CopyLen);
	}
	void BMP_WritePixels_Gray(const char* filename, int width, int height, unsigned char* rgbbuf, int buffersize);
	//BMP_WritePixels_Gray("D:/123.bmp", bmInfo->bmiHeader.biWidth, bmInfo->bmiHeader.biHeight, bmData, bmWidth * bmInfo->bmiHeader.biHeight);

	BITMAPINFO* info =(BITMAPINFO*)new BYTE[sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 256];
	memcpy(info, bmInfo, sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 256);
	info->bmiHeader.biWidth = Width;
	info->bmiHeader.biHeight = Height;
	info->bmiHeader.biSizeImage = Width * Height;
	
	
	SetDIBitsToDevice(hdc,
		0, 0, Width, Height,
		0,//xSrc
		0,//ySrc
		0,
		Height,
		BitmapBuffer, info, DIB_RGB_COLORS);
	QueryPerformanceCounter(&EndCounter);
	double Interval =  (EndCounter.QuadPart - StartCounter.QuadPart) * 1000.0 / Freq.QuadPart;

	//printf("%f\n", Interval);

	delete[] info;
	delete[] BitmapBuffer;
}

void DragImage(HWND hWnd)
{
	//DragImage_SetDIBitsToDevice(hWnd);
	//DragImage_BitBlt(hWnd);
	//DragImage_BitBlt2(hWnd);
	//DragImage_Manual(hWnd);
	DragImage_StretchBlt(hWnd);
}
