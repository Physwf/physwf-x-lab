#include "HDRIFile.h"
#include "hdrloader.h"
#include "ToneMapping.h"
#include "Atmosphere.h"
#include "bmp.h"

#include <windows.h>

int commandline(int argc, char** argv)
{
	if (argc <= 1)
	{
		printf("Please specify file path and tonemaping algorisim!");
		return 1;
	}
	if (argc <= 2)
	{
		printf("Please specify tonemaping method!");
	}
	char* filename = argv[1];
	char* szAlgo = argv[2];
	char oputputName[1024];
	sprintf_s(oputputName, sizeof(oputputName), "%s-%s.bmp", filename, szAlgo);

	EToneMappingAlgo eAlgo = TMA_ACES;

	if (strcmp("REINHARD2", szAlgo) == 0)
	{
		eAlgo = TMA_REINHARD2;
	}
	else if (strcmp("ACES", szAlgo))
	{
		eAlgo = TMA_ACES;
	}
	else if (strcmp("TMA_Unreal", szAlgo))
	{
		eAlgo = TMA_Unreal;
	}

	HRDIFile f;
	if (!f.Load(filename))
	{
		printf("File dose not exist or file format illegal!");
		return 2;
	}

	std::vector<unsigned char> Output;
	Output.resize(f.Width() * f.Height() * 3);
	ToneMapping(f.GetData(), f.Width(), f.Height(), Output.data(), eAlgo);
	{
		FILE* RGB;
		fopen_s(&RGB, oputputName, "w+b");
		BMP_WritePixels_RGB24(RGB, Output.data(), f.Width(), f.Height());
		fclose(RGB);
	}
	return 0;
}

void GenerateSky()
{
	float Color[] = { 0.2f,0.4f,0.8f }; 
	std::vector<float> OutputHDR;
	int Width = 4096;
	int Height = 768;
	OutputHDR.resize(Width * Height * 3);
	GenerateCIESky(Width, Height, Color,3.14f * 0.5f * 0.5f, 1.0f, OutputHDR.data());
	std::vector<unsigned char> OutputLDR;
	OutputLDR.resize(Width * Height * 3);
	ToneMapping(OutputHDR.data(), Width, Height, OutputLDR.data(), TMA_ACES);
	FILE* RGB;
	fopen_s(&RGB, "testatmosphere.bmp", "w+b");
	BMP_WritePixels_RGB24(RGB, OutputLDR.data(), Width, Height);
	fclose(RGB);
}

HINSTANCE g_hInstance;
HWND g_hWnd;
HBITMAP g_hBitmap;
HWND g_hOpenButton;
int WindowWidth = 1024;
int WindowHeight = 512;
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
std::vector<unsigned char> ImageData;

#define IDB_OPEN     3301  
#define IDB_IMAGE    3302  

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

	RECT wr = { 0,0,WindowWidth,WindowHeight + 30 };
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

	g_hInstance = hInstance;
	g_hWnd = CreateWindowEx
	(
		NULL,
		L"WindowClass1",
		L"HDRViewer",
		WS_OVERLAPPEDWINDOW,
		WindowWidth,
		WindowHeight + 30,
		wr.right - wr.left,
		wr.bottom - wr.top,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	//GenerateSky();

	ShowWindow(g_hWnd, nCmdShow);

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
		else {
			Sleep(1);
		}

	}

	return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
	{
		if (g_hOpenButton == NULL)
		{
			g_hOpenButton = CreateWindow(TEXT("BUTTON"), TEXT("Open"), WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 0, 0, 100, 30, hWnd, (HMENU)IDB_OPEN, g_hInstance, NULL);
		}
		
		break;
	}
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case IDB_OPEN:
		{
			OPENFILENAME opfn;
			WCHAR strFilename[MAX_PATH];//存放文件名
			//初始化
			ZeroMemory(&opfn, sizeof(OPENFILENAME));
			opfn.lStructSize = sizeof(OPENFILENAME);//结构体大小
			//设置过滤
			opfn.lpstrFilter = L"所有文件\0*.*\0rgbe文件\0*.hdr\0";
			//默认过滤器索引设为1
			opfn.nFilterIndex = 1;
			//文件名的字段必须先把第一个字符设为 \0
			opfn.lpstrFile = strFilename;
			opfn.lpstrFile[0] = '\0';
			opfn.nMaxFile = sizeof(strFilename);
			//设置标志位，检查目录或文件是否存在
			opfn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
			//opfn.lpstrInitialDir = NULL;
			// 显示对话框让用户选择文件
			if (GetOpenFileName(&opfn))
			{
				char filepath[512] = { 0 };
				ZeroMemory(&filepath, sizeof(filepath));
				size_t NumBytesConverted = 0;
				wcstombs_s(&NumBytesConverted, filepath, opfn.lpstrFile, 512);
				HRDIFile Loader;
				if (!Loader.Load(filepath))
				{
					MessageBox(NULL, TEXT("Error!"), TEXT(""), MB_OK);
				}
				
				ImageData.resize(Loader.Width() * Loader.Height() * 3);
				ToneMapping(Loader.GetData(), Loader.Width(), Loader.Height(), ImageData.data(), TMA_ACES);

				if (ImageData.size() > 0)
				{
					BITMAPINFO bmi;
					ZeroMemory(&bmi, sizeof(BITMAPINFO));
					bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
					bmi.bmiHeader.biWidth = Loader.Width();
					bmi.bmiHeader.biHeight = Loader.Height();
					bmi.bmiHeader.biPlanes = 1;
					bmi.bmiHeader.biBitCount = 24;
					bmi.bmiHeader.biCompression = BI_RGB;

					unsigned char* Data = NULL;
					g_hBitmap = CreateDIBSection(GetDC(hWnd), &bmi, DIB_RGB_COLORS, (VOID**)(&Data), NULL, 0);
					memcpy_s(Data, ImageData.size(), ImageData.data(), ImageData.size());
				}

				RECT rect = { 0,30,Loader.Width(),Loader.Height()+30 };
				InvalidateRect(hWnd, &rect, false);
				UpdateWindow(hWnd);
			}
			break;
		}
		default:
			break;
		}
	}
	case WM_PAINT:
	{
		if (ImageData.size() > 0)
		{
			PAINTSTRUCT ps;// DC(可画图的内存对象) 的句柄
			HDC hdc;   // 通过窗口句柄获取该窗口的 DC
			hdc = BeginPaint(hWnd, &ps);
			HDC hDCMem = CreateCompatibleDC(hdc);
			HGDIOBJ hbmOld = SelectObject(hDCMem, g_hBitmap);
			BITMAP bm;
			GetObject(g_hBitmap, sizeof(bm), &bm);
			BitBlt(hdc, 0, 30, bm.bmWidth, bm.bmHeight, hDCMem, 0, 0, SRCCOPY);
			SelectObject(hDCMem, hbmOld);
			DeleteDC(hDCMem);
			EndPaint(hWnd, &ps);
		}

		break;
	}
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}

	}
	return	DefWindowProc(hWnd, message, wParam, lParam);
}
