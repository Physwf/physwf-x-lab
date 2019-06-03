#include "Camera.h"

Camera::Camera()
{

}

void Camera::SetPostion(XMVECTOR Position)
{
	Eye = Position;
}

void Camera::LookAt(XMVECTOR Target)
{
	LookDir = Target - Eye;
}

void Camera::SetViewport(float fWidth, float fHeight)
{
	fViewportWidth = fWidth;
	fViewportHeight = fHeight;
}

void Camera::SetLen(float fNear, float fFar)
{
	Near = fNear;
	Far = fFar;
}

void Camera::UpdateViewMatrix()
{
	Up /= (Up * Up);
	LookDir /= (LookDir * LookDir);
	XMVECTOR X = XMVector2Cross(Up, LookDir);
	X /= (X * X);
	Up = XMVector2Cross(LookDir, X);
	uint32_t Zero[] = { 0,0,0,1 };
	ViewMatrix = XMMATRIX(X, Up, LookDir, XMLoadInt4(Zero));
}

void Camera::UpdateProjMatrix()
{
	ProjMatrix = XMMATRIX
	(
		2*Near/fViewportWidth,	0,						0,						0,
		0,						2*Near/fViewportHeight,	0,						0,
		0,						0,						Far/(Far-Near),			-(Near*Far)/ (Far - Near),
		0,						0,						1,						0
	);
}
