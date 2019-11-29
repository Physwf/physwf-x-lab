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
// 	for (LONG y = 0; y < Height; ++y)
// 	{
// 		for (LONG x = 0; x < Width; ++x)
// 		{
// 			if (x == y)
// 			{
// 				pFrameBuffer[4 * y * Width + x * 4 + 0] = 255;
// 				pFrameBuffer[4 * y * Width + x * 4 + 1] = 0;
// 				pFrameBuffer[4 * y * Width + x * 4 + 2] = 0;
// 			}
// 		}
// 	}
	RECT rect = { X,Y,Width,Height };
	InvalidateRect(g_hWind, &rect, false);
	UpdateWindow(g_hWind);
}

float* glutMatrixOrthoLH(float* pOut, float t, float b, float l, float r, float zn, float zf)
{
	float result[]
	{
		1/(r-l),	0.0f,		0.0f,			-l,
		0.0f,		1/(t-b),	0.0f,			-b,
		0.0f,		0.0f,		1/(zf-zn),		-zn/(zf-zn),
		0.0f,		0.0f,		0.0f,			1.0f,
	};
	for (int i = 0; i < 16; ++i)
	{
		pOut[i] = result[i];
	}
	return pOut;
}

