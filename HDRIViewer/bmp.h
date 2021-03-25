#pragma once

#include <stdio.h>

void BMP_WritePixels_RGB24(FILE* stream, unsigned char* rgbbuf, int width, int height); 
void BMP_WritePixels_Gray(FILE* stream, unsigned char* rgbbuf, int width, int height);
