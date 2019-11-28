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

float* glutMatrixOrthoLH(float* pOut, float w, float h, float zn, float zf)
{
	float result[]
	{
		2 / w,	0.0f,	0.0f,		0.0f,
		0.0f,	2 / h,	0.0f,		0.0f,
		2 / w,	0.0f,	1/(zf-zn),	-zn/(zf-zn),
		2 / w,	0.0f,	0.0f,		1.0f,
	};
	for (int i = 0; i < 16; ++i)
	{
		pOut[i] = result[i];
	}
	return pOut;
}

