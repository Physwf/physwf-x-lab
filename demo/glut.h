#include <Windows.h>
#include "gl.h"

void glutInit(int x, int y, int w, int h, unsigned char* fb);
void glutPresent();
float* glutMatrixOrthoLH(float* pOut, float t, float b, float r, float l, float zn, float zf);
