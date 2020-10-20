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
public:

	Vector Eye;
	Vector FaceDir;
	Vector Up;

	float Near;
	float Far;
	float fViewportWidth = 720.f;
	float fViewportHeight = 720.f;

	int DragStartX;
	int DragStartY;
	bool bDraging;

	Vector StartFaceDir;
	int RotateStartX;
	int RotateStartY;
	bool bRotating;
};