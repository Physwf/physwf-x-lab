#include "scene.h"
#include "camera.h"
#include <stdio.h>

// int main()
// {
// 	//Vec v = {0.0};
// 	//printf("%d", sizeof(v));
// 
// 	scene = new Scene();
// 	Camera* camera = new Camera();
// 	Setup(scene, camera);
// 	Render(scene, camera);
// }

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

Camera* camera;

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


	scene = new Scene();
	camera = new Camera();
	Setup(scene, camera);

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
		HDC hdc;   // DC(�ɻ�ͼ���ڴ����) �ľ��

		// ͨ�����ھ����ȡ�ô��ڵ� DC
		hdc = BeginPaint(hWnd, &ps);

		//extern bool SavePixelsToPNG(const WCHAR* pFileName, int W, int H, unsigned int* pPixels);
		const int numSample = 10000;
		Render(scene, camera);
		WCHAR FileName[128] = { 0 };
		wsprintf(FileName, L".\\PT%d.png", numSample);
		//SavePixelsToPNG(FileName, camera->screen->Width, camera->screen->Height, camera->screen->buffer);
		for (int i = 0; i < camera->screen->Height; i++)
		{
			for (int j = 0; j < camera->screen->Width; ++j)
			{
				unsigned char R = (camera->screen->buffer[i*camera->screen->Height + j] & 0xFF0000) >> 16;
				unsigned char G = (camera->screen->buffer[i*camera->screen->Height + j] & 0x00FF00) >> 8;
				unsigned char B = camera->screen->buffer[i*camera->screen->Height + j] & 0x0000FF;
				SetPixel(hdc, j, i, RGB(R, G, B));
				//SetPixel(hdc, j, i, 0x0000FF);
			}
		}
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_LBUTTONDOWN:
	{
		DWORD xPos = LOWORD(lParam);  // horizontal position of cursor 
		DWORD yPos = HIWORD(lParam);  // vertical position of cursor 
		Render(scene, camera, xPos, yPos);
	}
		break;
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}break;
	}

	return	DefWindowProc(hWnd, message, wParam, lParam);
}
