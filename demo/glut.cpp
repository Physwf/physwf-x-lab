#include "glut.h"
#include "gl_pipeline.h"

unsigned char* pFrameBuffer;
extern HWND g_hWind;

LONG X;
LONG Y;
LONG Width;
LONG Height;

void glutInit(int x, int y, int w, int h,unsigned char* fb)
{
	X = x;
	Y = y;
	Width = w;
	Height = h;
	pFrameBuffer = fb;
}

void glutPresent()
{
	gl_copy_front_buffer(pFrameBuffer);
	RECT rect = { X,Y,Width,Height };
	InvalidateRect(g_hWind, &rect, true);
	UpdateWindow(g_hWind);
}

