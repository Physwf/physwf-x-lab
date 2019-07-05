#include <windows.h>

void OutputDebug(const char* Format)
{
	OutputDebugStringA(Format);
}

LRESULT CALLBACK WindowProc(HWND hWnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam);

HWND g_hWind = NULL;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	extern void SetUpScene();
	SetUpScene();

	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(WNDCLASSEX));

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

	return msg.wParam;
}

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

		extern void Render(int &W, int &H, int iNumSample, unsigned int** Colors);
		extern bool SavePixelsToPNG(const WCHAR* pFileName, int W, int H, unsigned int* pPixels);
		int W, H;
		unsigned int* Colors;
		const int numSample = 100;
		Render(W, H, numSample, &Colors);
		WCHAR FileName[128] = { 0 };
		wsprintf(FileName, L".\\PT%d.png", numSample);
		SavePixelsToPNG(FileName, W, H, Colors);
		for (int i = 0; i < H; i++)
		{
			for (int j = 0; j < W; ++j)
			{
				unsigned char R = (Colors[i*W + j] & 0xFF0000) >> 16;
				unsigned char G = (Colors[i*W + j] & 0x00FF00) >> 8;
				unsigned char B = Colors[i*W + j] & 0x0000FF;
				SetPixel(hdc, j, i, RGB( R , G , B));
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
