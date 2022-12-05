#pragma once

int GetBitmapInfo(const char* filename, long* Width, long* Height,unsigned short* BitCount, unsigned long long* DataSize);
int ReadBitmap(const char* filename, void* Buffer, size_t BufferSize);
int WriteBitmap(const char* filename, void* Buffer, size_t BufferSize, long Width, long Height, int Format);