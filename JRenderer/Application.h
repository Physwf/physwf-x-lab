#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // �� Windows ͷ�ļ����ų�����ʹ�õ�����
// Windows ͷ�ļ�
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
	WCHAR mszTitle[MAX_LOADSTRING];                  // �������ı�
	WCHAR mszWindowClass[MAX_LOADSTRING];            // ����������
	HINSTANCE mhInstance;
	HWND mMainWindow;
	HACCEL mhAccelTable;
};


