#include "Application.h"

LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

static JApplication* GApplication;

JApplication::JApplication(HINSTANCE hInstance) :mhInstance(hInstance)
{
	GApplication = this;
}

JApplication& JApplication::Get()
{
	return *GApplication;
}

void JApplication::Initialize()
{
	// 初始化全局字符串
	LoadStringW(mhInstance, IDS_APP_TITLE, mszTitle, MAX_LOADSTRING);
	LoadStringW(mhInstance, IDC_DEMO, mszWindowClass, MAX_LOADSTRING);

	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = mhInstance;
	wcex.hIcon = LoadIcon(mhInstance, MAKEINTRESOURCE(IDI_DEMO));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_DEMO);
	wcex.lpszClassName = mszWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	RegisterClassExW(&wcex);

	mMainWindow = CreateWindowW(mszWindowClass, mszTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, mhInstance, nullptr);

	mhAccelTable = LoadAccelerators(mhInstance, MAKEINTRESOURCE(IDC_DEMO));
}

void JApplication::ShowSplash()
{

}

void JApplication::ShowMainWindow(int nCmdShow)
{
	ShowWindow(mMainWindow, nCmdShow);
	UpdateWindow(mMainWindow);
}

int JApplication::Loop()
{
	MSG msg;
	GetMessage(&msg, nullptr, 0, 0);
	{
		if (!TranslateAccelerator(msg.hwnd, mhAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}
