#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // 从 Windows 头文件中排除极少使用的内容
// Windows 头文件
#include <windows.h>
#include "Resource.h"

#define MAX_LOADSTRING 100

class JApplication
{
public:
	JApplication(HINSTANCE hInstance);

	~JApplication()
	{}

	static JApplication& Get();

	void Initialize();

	void ShowSplash();
	void ShowMainWindow(int nCmdShow);

	int Loop();
private:
	WCHAR mszTitle[MAX_LOADSTRING];                  // 标题栏文本
	WCHAR mszWindowClass[MAX_LOADSTRING];            // 主窗口类名
	HINSTANCE mhInstance;
	HWND mMainWindow;
	HACCEL mhAccelTable;
};


