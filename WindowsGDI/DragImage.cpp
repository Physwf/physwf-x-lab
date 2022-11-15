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

extern void LoadBitmap(const char* filename, BYTE** pBuffer, BITMAPINFO** bmInfo);

void DragInit()
{
	LoadBitmap("D:/1_1_1_0_0_1_16.bmp", &bmData, &bmInfo);
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
	HDC hdc = GetDC(hWnd);
	//StretchDIBits(hdc, 0, 0, rect.right - rect.left, rect.bottom - rect.top, OffsetX + DragOffsetX, -(OffsetY + DragOffsetY), rect.right - rect.left, rect.bottom - rect.top, bmData, bmInfo, DIB_RGB_COLORS, SRCCOPY);
	SetDIBitsToDevice(hdc,
		0, 0, rect.right - rect.left, rect.bottom - rect.top,
		-(g_OffsetX + g_DragOffsetX),
		bmInfo->bmiHeader.biHeight - (rect.bottom - rect.top) + (g_OffsetY + g_DragOffsetY), /*+ rect.bottom - rect.top*//*,*/
		0,//-(OffsetY + DragOffsetY) * 4800 - (OffsetX + DragOffsetX), 
		bmInfo->bmiHeader.biHeight,//rect.bottom - rect.top, 
		bmData, bmInfo, DIB_RGB_COLORS);
}

void DragImage_BitBlt(HWND hWnd)
{
	RECT rect;
	GetWindowRect(hWnd, &rect);
	HDC hdc = GetDC(hWnd);
	HDC memHDC = CreateCompatibleDC(hdc);
	HBITMAP hbitmap = CreateCompatibleBitmap(hdc, rect.right - rect.left, rect.bottom - rect.top);
	HGDIOBJ oldbmp = SelectObject(memHDC, hbitmap);

	SetDIBitsToDevice(memHDC,
		0, 0, rect.right - rect.left, rect.bottom - rect.top,
		-(g_OffsetX + g_DragOffsetX),
		bmInfo->bmiHeader.biHeight - (rect.bottom - rect.top) + (g_OffsetY + g_DragOffsetY), /*+ rect.bottom - rect.top*//*,*/
		0,//-(OffsetY + DragOffsetY) * 4800 - (OffsetX + DragOffsetX), 
		bmInfo->bmiHeader.biHeight,//rect.bottom - rect.top, 
		bmData, bmInfo, DIB_RGB_COLORS);

	DWORD Error = GetLastError();
	BOOL bSuccess = BitBlt(hdc, 0, 0, rect.right - rect.left, rect.bottom - rect.top, memHDC, 0, 0, SRCCOPY); // blt from bitmap to screen
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
	int StartY = g_OffsetY + g_DragOffsetY;
	BYTE* BitmapBuffer = new BYTE[Width * Height]();

	if (StartX > bmInfo->bmiHeader.biWidth) return;
	if (StartY > bmInfo->bmiHeader.biHeight) return;

	if (StartX < 0) StartX = 0;
	if (StartY < 0) StartY = 0;

	int EndX = StartX + Width;
	int EndY = StartY + Height;

	if (EndX > bmInfo->bmiHeader.biWidth) EndX = bmInfo->bmiHeader.biWidth;
	if (EndY > bmInfo->bmiHeader.biHeight) EndY = bmInfo->bmiHeader.biHeight;

	DWORD bmWidth = bmInfo->bmiHeader.biWidth;
	if (bmWidth % 4 != 0)
	{
		bmWidth = (bmWidth & (~3ull)) + 4;
	}
	for (int h = 0; h < EndY - StartY; ++h)
	{
		//memcpy(BitmapBuffer + h * Width, bmData + StartX + (StartY + h) * bmInfo->bmiHeader.biWidth, EndX - StartX);
		memcpy(BitmapBuffer + h * Width, bmData + StartX + (StartY + h) * bmWidth, EndX - StartX);
		//memset(BitmapBuffer + h * Width, 255, EndX - StartX);
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
		0,
		0,
		0,
		Height,
		BitmapBuffer, info, DIB_RGB_COLORS);
	QueryPerformanceCounter(&EndCounter);
	double Interval =  (EndCounter.QuadPart - StartCounter.QuadPart) * 1000.0 / Freq.QuadPart;

	printf("%f\n", Interval);

	delete[] info;
	delete[] BitmapBuffer;
}

void DragImage(HWND hWnd)
{
	//DragImage_BitBlt(hWnd);
	//DragImage_BitBlt2(hWnd);
	DragImage_Manual(hWnd);
}
