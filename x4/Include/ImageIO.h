#pragma once

#include "../X4.h"

bool X4_API ReadImageHDR(const char* filename, void* Buffer, int* width, int* height);
bool X4_API WriteImageHDR(const char* filename, void* Buffer, int width, int height);
bool X4_API ReadImageBMP(const char* filename, void* Buffer, int* width, int* height);
bool X4_API WriteImageBMP(const char* filename, void* Buffer, int width, int height);