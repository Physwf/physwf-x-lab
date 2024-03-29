#include <windows.h>
#include <functional>

void OutputDebug(const char* Format)
{
	OutputDebugStringA(Format);
}

LRESULT CALLBACK WindowProc(HWND hWnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam);

HWND g_hWind = NULL;

//unsigned int* BackBuffer;
BYTE* BackBuffer;
int W = 500, H = 500;
const int numSample = 100000;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//extern void SetUpScene();
	//SetUpScene();

	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	//extern void InitFixedThreadPool();
	//extern void Render(int& W, int& H, int iNumSample, unsigned int** BackBuffer,std::function<void()>);
	//InitFixedThreadPool();
	BackBuffer = new BYTE[500 * 500 * 3]();

	wc.cbSize = sizeof WNDCLASSEX;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpszClassName = L"WindowClass1";

	RegisterClassEx(&wc);

	RECT wr = { 0,0,500,500 };
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

	g_hWind = CreateWindowEx
	(
		NULL,
		L"WindowClass1",
		L"dx11demo",
		WS_OVERLAPPEDWINDOW,
		300,
		300,
		wr.right - wr.left,
		wr.bottom - wr.top,
		NULL,
		NULL,
		hInstance,
		NULL
	);



	ShowWindow(g_hWind, nCmdShow);

	//Render(W, H, numSample, &BackBuffer, [=]() { InvalidateRect(g_hWind, &wr,false); });
	void StartGround();
	StartGround();

	MSG msg;
	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			if (msg.message == WM_QUIT)
			{
				break;
			}
		}
		Sleep(10);
	}

	//FiniFixedThreadPool();

	return msg.wParam;
}

void Display(BYTE* Data, size_t size, int width, int height)
{
	memcpy(BackBuffer, Data, size);
	RECT wr = { 0,0,500,500 };
	InvalidateRect(g_hWind, &wr, false);
}

#include <stdio.h>
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc;   // DC(可画图的内存对象) 的句柄

		// 通过窗口句柄获取该窗口的 DC
		hdc = BeginPaint(hWnd, &ps);
		//extern bool SavePixelsToPNG(const WCHAR * pFileName, int W, int H, unsigned int* pPixels);
		//WCHAR FileName[128] = { 0 };
		//wsprintf(FileName, L".\\PT%d.png", numSample);
		//SavePixelsToPNG(FileName, W, H, BackBuffer);
		for (int i = 0; i < H; i++)
		{
			for (int j = 0; j < W; ++j)
			{
// 				unsigned char R = (BackBuffer[i*W + j] & 0xFF0000) >> 16;
// 				unsigned char G = (BackBuffer[i*W + j] & 0x00FF00) >> 8;
// 				unsigned char B = BackBuffer[i*W + j] & 0x0000FF;
				unsigned char R = (BackBuffer[(i*W + j)*3 + 0]);
				unsigned char G = (BackBuffer[(i*W + j)*3 + 1]);
				unsigned char B = (BackBuffer[(i*W + j)*3 + 2]);
				SetPixel(hdc, j, H -i, RGB(B, G , R));
				//SetPixel(hdc, j, i, 0x0000FF);
			}
		}
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}break;
	}

	return	DefWindowProc(hWnd, message, wParam, lParam);
}
