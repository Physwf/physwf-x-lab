#include "pch.h"

#include "IO.h"
#include "Combine.h"

extern "C" _declspec(dllexport) int Combine(const char** files, int fileCount, const char* outname)
{
	return CombineImages(files, fileCount, outname, 0, 0);
}

extern "C" _declspec(dllexport) int CombineWithOverlap(const char** files, int fileCount, const char* outname, int OverlapX, int OverlapY)
{
	return CombineImages(files, fileCount, outname, OverlapX, OverlapY);
}