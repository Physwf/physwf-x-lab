// WindowsGDI.cpp : Defines the entry point for the application.
//

#include "Windows.h"
#include "Windowsx.h"
#include "Resource.h"

#include "InfBitmap.h"

#include <stdio.h>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

int g_OffsetX = 0, g_OffsetY = 0;
int g_DargStartX = 0, g_DragStartY = 0;
int g_DragOffsetX = 0, g_DragOffsetY = 0;
bool g_bDraging = false;
double g_dbScale = 1.0;
InfBitmap* gBitmap;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);

	char output[256] = {0};
	sprintf_s(output, "%ws", lpCmdLine);
	printf(output);
	const char* args[10] = { 0 };
	bool bSkiped = true;
	int argcount = 0;
	int len = (int)strlen(output);
	for (int j = 0; j < len; ++j)
	{
		if (output[j] == ' ')
		{
			bSkiped = true;
			output[j] = '\0';
			continue;
		}
		if (bSkiped)
		{
			bSkiped = false;
			args[argcount++] = &output[j];
		}
	}

	AllocConsole(); //#include "winsock2.h"
	SetConsoleTitleA("Debug Output"); // 设置控制台窗口的标题
	FILE* F;
	freopen_s(&F, "CONOUT$", "w", stdout); // 重定向输出
	printf("%s\n\n", "*** ** *** Console *** ** ***");

	//gBitmap = InfBitmap::CreateFromBmp("D:/jianbian2.bmp");
	gBitmap = InfBitmap::CreateFromBmp("D:/1-11.bmp");
    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINDOWSGDI, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSGDI));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSGDI));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WINDOWSGDI);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 1000, 1000, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   HDC hDC = GetDC(hWnd);
   int nBITSPIXEL = GetDeviceCaps(hDC, BITSPIXEL);
   int nHORZRES = GetDeviceCaps(hDC, HORZRES);
   int nVERTRES = GetDeviceCaps(hDC, VERTRES);
   int nHORZSIZE = GetDeviceCaps(hDC, HORZSIZE);
   int nVERTSIZE = GetDeviceCaps(hDC, VERTSIZE);
   float nHorzRes = (float)nHORZRES / (float)nHORZSIZE;
   float nVertRes = (float)nVERTRES / (float)nVERTSIZE;
   int nNUMCOLORS = GetDeviceCaps(hDC, NUMCOLORS);

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}


// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_CREATE:
	{
		break;
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		RECT rect;
		GetWindowRect(hWnd, &rect);
		UINT64 Width, Height;
		std::unique_ptr<BYTE[]> Data;
		DWORD ScreenW = rect.right - rect.left;
		DWORD ScreenH = rect.bottom - rect.top;
		gBitmap->Scale(g_dbScale, 0, 0, ScreenW, ScreenH, Width, Height, Data);
		BITMAPINFO* info = (BITMAPINFO*)new BYTE[sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 256];
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
		SetDIBitsToDevice(hdc,
			0, 0, ScreenW, ScreenH,
			0,//xSrc
			0,//ySrc
			0,
			ScreenH,
			Data.get(), info, DIB_RGB_COLORS);
		delete[] info;
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_LBUTTONDOWN:
	{
		g_bDraging = true;
		g_DargStartX = GET_X_LPARAM(lParam);
		g_DragStartY = GET_Y_LPARAM(lParam);
	}
	break;
	case WM_LBUTTONUP:
	{
		g_OffsetX += g_DragOffsetX;
		g_OffsetY += g_DragOffsetY;
		g_DragOffsetX = 0;
		g_DragOffsetY = 0;
		g_bDraging = false;
		//UpdateWindow(hWnd);
		RECT rect;
		GetWindowRect(hWnd, &rect);
		InvalidateRect(hWnd, &rect, FALSE);
	}
	break;
	case WM_MOUSEMOVE:
	{
		static LARGE_INTEGER Freq, StartCounter, EndCounter;
		QueryPerformanceFrequency(&Freq);
		QueryPerformanceCounter(&EndCounter);
		double Interval = (EndCounter.QuadPart - StartCounter.QuadPart) * 1000.0 / Freq.QuadPart;
		//if(Interval < 20) break;
		StartCounter = EndCounter;
		//printf("Interval:%f\n", Interval);
		if (g_bDraging)
		{
			int X = GET_X_LPARAM(lParam);
			int Y = GET_Y_LPARAM(lParam);
			g_DragOffsetX = X - g_DargStartX;
			g_DragOffsetY = Y - g_DragStartY;
			//UpdateWindow(hWnd);
			//RedrawWindow(hWnd,Rect(0,0,100,100),)
			RECT rect;
			GetWindowRect(hWnd, &rect);
			InvalidateRect(hWnd, &rect, FALSE);
		}
		break;
	}
	case WM_MOUSEWHEEL:
	{
		int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		double DeltaScale = 0.01 * (double)zDelta / WHEEL_DELTA;
		g_dbScale += DeltaScale;
		if (g_dbScale < 0.01) g_dbScale = 0.01;
		if (g_dbScale > 10.0) g_dbScale = 10.0;
		printf("zDelta:%d DeltaScale:%f g_dbScale:%f \n", zDelta, DeltaScale, g_dbScale);
		RECT rect;
		GetWindowRect(hWnd, &rect);
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
