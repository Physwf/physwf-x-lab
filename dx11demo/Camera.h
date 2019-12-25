#pragma once

#include "Math.h"
#include "D3D11RHI.h"


struct CAMERA_CBUFFER
{
	Matrix ViewMatrix;
	Matrix ProjMatrix;
};

class Camera
{
public:
	Camera();
	~Camera() {};

	void SetPostion(Vector Position);
	void LookAt(Vector Target);
	void SetViewport(float fWidth, float fHeight);
	void SetLen(float fNear, float fFar);

	void InitResource();
	void ReleaseResource();
	void Render();
private:
	void UpdateViewMatrix();
	void UpdateProjMatrix();
private:
	CAMERA_CBUFFER VSConstBuffer;

	Vector Eye;
	Vector LookAtTarget;
	Vector Up;

	float Near;
	float Far;
	float fViewportWidth;
	float fViewportHeight;

	ID3D11Buffer* ConstantBuffer;
};