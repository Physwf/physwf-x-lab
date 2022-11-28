#pragma once
#include <Windows.h>

class InfBitmap;

class InfImage
{
public:
	InfImage();
	InfImage(InfBitmap* pBitmap);

	int X() const { return mX; }
	int& X() { return mX; }
	int Y() const { return mY; }
	int& Y() { return mY; }
	unsigned long long Width() const { return mWidth * mScale; }
	unsigned long long Height() const { return mHeight * mScale; }
	double Scale() const { return mScale; }
	double& Scale() { return mScale; }
public:
	bool HitTest(int X, int Y);
	void Render(HWND hWnd);
protected:
	int mX;
	int mY;
	unsigned long long mWidth;
	unsigned long long mHeight;
	double mScale;
	InfBitmap* mBitmap;
};

