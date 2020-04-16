
#include "CoreTypes.h"
#include "Windows/MinWindows.h"

extern int32 GuardedMain(const TCHAR* CmdLine, HINSTANCE hInInstance, HINSTANCE hPrevInstance, int32 nCmdShow);
extern int32 GuardedMain();

int32 WINAPI WinMain(_In_ HINSTANCE hInInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ char*, _In_ int32 nCmdShow)
{
	GuardedMain();
}