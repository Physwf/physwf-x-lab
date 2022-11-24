// WindowsGDI.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "WindowsGDI.h"
#include "Windowsx.h"

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
	
	if (strcmp("combine", args[0]) == 0)
	{
		if (argcount >= 4)
		{
			const char* file1 = args[1];
			const char* file2 = args[2];
			const char* outfile = args[3];
			extern bool CombineImages(const char* filename1, const char* filename2, const char* outname, int dir = 0);//dir=0,horizontal;dir=1,vertical
			//CombineImages(file1, file2, outfile, 0);
		}
	}
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
      CW_USEDEFAULT, 0, 1400, 1000, nullptr, nullptr, hInstance, nullptr);

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
		extern void DragInit();
		DragInit();
		break;
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		extern void DragImage(HWND hWnd);
		DragImage(hWnd);
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
		double DeltaScale = 0.1 * (double)zDelta / WHEEL_DELTA;
		g_dbScale += DeltaScale;
		if (g_dbScale < 0.1) g_dbScale = 0.1;
		if (g_dbScale > 10.0) g_dbScale = 10.0;
		printf("zDelta:%d DeltaScale:%f g_dbScale:%f \n", zDelta, DeltaScale, g_dbScale);
		RECT rect;
		GetWindowRect(hWnd, &rect);
		InvalidateRect(hWnd, &rect, FALSE);
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
