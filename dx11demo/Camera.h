#pragma once

#include "DirectXMath.h"
#include "D3D11RHI.h"

using namespace DirectX;

struct CAMERA_CBUFFER
{
	XMMATRIX ViewMatrix;
	XMMATRIX ProjMatrix;
};

class Camera
{
public:
	Camera();
	~Camera() {};

	void SetPostion(XMVECTOR Position);
	void LookAt(XMVECTOR Target);
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

	XMVECTOR Eye;
	XMVECTOR LookAtTarget;
	XMVECTOR Up;

	float Near;
	float Far;
	float fViewportWidth;
	float fViewportHeight;

	ID3D11Buffer* ConstantBuffer;
};