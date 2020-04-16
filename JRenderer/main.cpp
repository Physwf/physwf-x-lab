#include "framework.h"
#include "d3d12demo.h"
#include "Application.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	JApplication App(hInstance);
	App.Initialize();
	App.ShowMainWindow(nCmdShow);
	int nParam = 0;
	while (true)
	{
		nParam = App.Loop();
	}
	return nParam;
}