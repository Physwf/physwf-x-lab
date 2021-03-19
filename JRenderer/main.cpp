#include "TriangleDemo.h"
#include "ShadingModels.h"
//#include "PBRTools.h"
#include "SkyBoxDemo.h"

LRESULT CALLBACK WindowProc(HWND hWnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam);

HWND g_hWind = NULL;
LONG WindowWidth = 1920;
LONG WindowHeight = 1080;

//TriangleDemoMSAA2 Demo(g_hWind);
D3D12Demo* pDemo;

#define X_LOG(Format,...) XLOG(Format, __VA_ARGS__)

void OutputDebug(const char* Format)
{
	OutputDebugStringA(Format);
}

inline void XLOG(const char* format, ...)
{
	char buffer[16 * 1024] = { 0 };
	va_list v_list;
	va_start(v_list, format);
	vsprintf_s(buffer, format, v_list);
	va_end(v_list);
	extern void OutputDebug(const char* Format);
	OutputDebug(buffer);
}




int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

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

	RECT wr = { 0,0,WindowWidth,WindowHeight };
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

	g_hWind = CreateWindowEx
	(
		NULL,
		L"WindowClass1",
		L"JRender",
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

	//PhongShadingModel Demo(g_hWind);
	//MERL100Coordinate Demo(g_hWind);
	//SkyBoxDemo Demo(g_hWind);
	PBRShadingModelRealIBL Demo(g_hWind);
	pDemo = &Demo;
	Demo.Initialize();

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
			continue;
		}
		{
			Demo.Render();
			Sleep(1);
		}

	}

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
	}
	case WM_KEYDOWN:
	{
		pDemo->OnKeyDown(wParam);
		break;
	}
	case WM_KEYUP:
	{
		break;
	}
	case WM_LBUTTONDOWN:
	{
		break;
	}
	case WM_LBUTTONUP:
	{
		break;
	}
	case WM_RBUTTONDOWN:
	{
		break;
	}
	case WM_RBUTTONUP:
	{
		break;
	}
	case WM_MOUSEMOVE:
	{
		X_LOG("MouseX:%d,MouseY:%d\n", LOWORD(lParam), HIWORD(lParam));
		pDemo->OnMouseMove(LOWORD(lParam)/(float)WindowWidth, HIWORD(lParam) / (float)WindowHeight);
		break;
	}
	}
	return	DefWindowProc(hWnd, message, wParam, lParam);
}
