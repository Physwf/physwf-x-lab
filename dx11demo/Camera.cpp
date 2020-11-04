#include "Camera.h"
#include "log.h"

Camera::Camera():Near(1.0f), Far(1000.0f)
{
	bDraging = false;
	bRotating = false;
	Up = { 0, 1, 0 };
}

void Camera::SetPostion(Vector Position)
{
	Eye = Position;
	//UpdateViewMatrix();
}

const Vector& Camera::GetPosition() const
{
	return Eye;
}

void Camera::LookAt(Vector Target)
{
	FaceDir = Target - Eye;
	FaceDir.Normalize();
	//UpdateViewMatrix();
}

void Camera::SetViewport(float fWidth, float fHeight)
{
	fViewportWidth = fWidth;
	fViewportHeight = fHeight;
	//UpdateProjMatrix();
}

void Camera::SetLen(float fNear, float fFar)
{
	Near = fNear;
	Far = fFar;
	//UpdateProjMatrix();
}

void Camera::Walk(float fStep)
{
	Vector Step = fStep * FaceDir;
	Eye += Step;
	//UpdateViewMatrix();
}

void Camera::Side(float fStep)
{
	Vector SideDir = FaceDir ^ Up;
	SideDir.Normalize();
	Vector Step = fStep * SideDir;
	Eye += Step;
	//UpdateViewMatrix();
}

void Camera::Back(float fStep)
{
	Vector Step = fStep * (-FaceDir);
	Eye += Step;
	//UpdateViewMatrix();
}

void Camera::Lift(float fStep)
{
	Vector Step = fStep * Up;
	Eye += Step;
	//UpdateViewMatrix();
}

void Camera::StartDrag(int X, int Y)
{
	bDraging = true;
	DragStartX = X;
	DragStartY = Y;
	StartFaceDir = FaceDir;
}

void Camera::Drag(int X, int Y)
{
	if (bDraging)
	{
		int Dx = X - DragStartX;
		int Dy = Y - DragStartY;
		float fDx = Dx / fViewportWidth;
		float fDy = Dy / fViewportHeight;
		Matrix Rotation = Matrix::DXFormRotation({ fDy, fDx, 0.0f});
		FaceDir = Rotation.Transform(StartFaceDir);
		//UpdateViewMatrix();
	}
}

void Camera::StopDrag(int X, int Y)
{
	if (bDraging)
	{
		int Dx = X - DragStartX;
		int Dy = Y - DragStartY;
		float fDx = Dx / fViewportWidth;
		float fDy = Dy / fViewportHeight;
		Matrix Rotation = Matrix::DXFormRotation({ fDy, fDx, 0.0f });
		FaceDir = Rotation.Transform(StartFaceDir);
		//UpdateViewMatrix();
	}
	bDraging = false;
}

void Camera::StartRotate(int X, int Y)
{
	if (bRotating) return;
	bRotating = true;
	RotateStartX = X;
	RotateStartY = Y;
	StartFaceDir = FaceDir;
}

void Camera::Rotate(const Vector& R)
{
	Matrix Rotation = Matrix::DXFormRotation(R);
	FaceDir = Rotation.Transform(StartFaceDir);
	//UpdateViewMatrix();
}

void Camera::Rotate(int X, int Y)
{
	if (bRotating)
	{
		int Dx = X - RotateStartX;
		int Dy = Y - RotateStartY;
		float fDy = -Dx / fViewportWidth;
		float fDx = Dy / fViewportHeight;
		Rotate({ fDx ,fDy, 0.0f });
	}
}

void Camera::StopRotate(int X, int Y)
{
	if (!bRotating) return;
	bRotating = false;
	RotateStartX = X;
	RotateStartY = Y;
}

