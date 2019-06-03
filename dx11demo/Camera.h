#pragma once

#include "DirectXMath.h"

using namespace DirectX;

class Camera
{
public:
	Camera();
	~Camera() {};

	void SetPostion(XMVECTOR Position);
	void LookAt(XMVECTOR Target);
	void SetViewport(float fWidth, float fHeight);
	void SetLen(float fNear, float fFar);
private:
	void UpdateViewMatrix();
	void UpdateProjMatrix();
private:
	XMMATRIX ViewMatrix;
	XMMATRIX ProjMatrix;

	XMVECTOR Eye;
	XMVECTOR LookDir;
	XMVECTOR Up;

	float Near;
	float Far;
	float fViewportWidth;
	float fViewportHeight;
};