#pragma once

#include <stdio.h>

unsigned int GetRGBHeaderSize();
unsigned int GetGrayHeaderSize();
void BMP_WritePixels_RGB24(FILE* stream, unsigned char* rgbbuf, int width, int height);
void BMP_WritePixels_RGB24(void* stream, unsigned char* rgbbuf, int width, int height);
void BMP_WritePixels_Gray(FILE* stream, unsigned char* rgbbuf, int width, int height);
void BMP_WritePixels_Gray(void* stream, unsigned char* rgbbuf, int width, int height);
