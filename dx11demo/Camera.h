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

	void Walk(float fStep);
	void Side(float fStep);
	void Back(float fStep);
	void Lift(float fStep);

	void StartDrag(int X, int Y);
	void Drag(int X, int Y);
	void StopDrag(int X, int Y);

	void StartRotate(int X, int Y);
	void Rotate(int X, int Y);
	void StopRotate(int X, int Y);

	void Rotate(const Vector& R);

	void InitResource();
	void ReleaseResource();
	void Render();
private:
	void UpdateViewMatrix();
	void UpdateProjMatrix();
private:
	CAMERA_CBUFFER VSConstBuffer;

	Vector Eye;
	Vector FaceDir;
	Vector Up;

	float Near;
	float Far;
	float fViewportWidth;
	float fViewportHeight;

	int DragStartX;
	int DragStartY;
	bool bDraging;

	Vector StartFaceDir;
	int RotateStartX;
	int RotateStartY;
	bool bRotating;

	ID3D11Buffer* ConstantBuffer;
};