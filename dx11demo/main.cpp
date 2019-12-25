#include <windows.h>
#include "D3D11RHI.h"
#include "Scene.h"
#include "Camera.h"

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

	RECT wr = { 0,0,720,720 };
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



	//extern void EnumAdapters();
	//EnumAdapters();
	extern void CreateTriangleBuffer();
	extern void RenderTriangle();

	if (!D3D11Setup())
	{
		return 1;
	}

	//CreateTriangleBuffer();

	Scene S;
	S.InitResource();
	S.Setup();

	Camera C;
	C.SetPostion({ 0, 100, -200 });
	C.LookAt({ 0, 100, 0 });
	C.SetViewport(720, 720);
	C.SetLen(1, 400);
	C.InitResource();

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
		else
		{
			D3D11ClearViewTarget();
			//RenderTriangle();
			C.Render();
			S.Draw();
			D3D11Present();
		}
		Sleep(10);
	}

	S.ReleaseResource();
	C.ReleaseResource();

	return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}break;
	}

	return	DefWindowProc(hWnd, message, wParam, lParam);
}
