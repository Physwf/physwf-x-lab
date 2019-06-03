#pragma once

#include <DirectXMath.h>

class Camera
{
public:
	Camera() {};
	~Camera() {};

	void SetViewport(int iWidth, int iHeight);
	void SetFarPlan();
private:
	XMFLOAT4X4 Matrix;

	int iViewportX;
	int iViewportY;

};