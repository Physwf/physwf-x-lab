#include <Windows.h>
#include "gl.h"

#define X_LOG(Format,...) XLOG(Format, __VA_ARGS__)

void glutInit(int x, int y, int w, int h, unsigned char* fb);
void glutPresent();
float* glutMatrixOrthoLH(float* pOut, float t, float b, float r, float l, float zn, float zf);
void XLOG(const char* format, ...);
