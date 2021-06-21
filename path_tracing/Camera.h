#pragma once

#include "Transform.h"
#include "Bounds.h"

class Camera
{
public:
	Camera(const Transform& InCameraToWorld):CameraToWorld(InCameraToWorld){}
	virtual float GenerateRay(uint32_t x, uint32_t y, Ray* OutRay) = 0;
protected:
	Transform CameraToWorld;
};

class PerspectiveCamera : public Camera
{
public:
	PerspectiveCamera(const Transform& InCameraToWorld,int ScreenWidth, int ScreenHeight, float fov,float zNeer,float zFar)
		: Camera(InCameraToWorld), 
		CameraToNDC(Perspective(fov, (float)ScreenWidth / (float)ScreenHeight, zNeer, zFar)),
		Viewport(Vector2i(0, 0),Vector2i(ScreenWidth, ScreenHeight))
	{
		Vector2i Resolution = Viewport.Diagonal();
		NDCToScreen = Scale(Resolution.X / 2.f, Resolution.Y / 2.f,0.f) * Translate(Vector3f(-1.f,-1.f,0.f));
		ScreenToNDC = Inverse(NDCToScreen);
		ScreenToCamera = Inverse(CameraToNDC) * ScreenToNDC;
	}
	virtual float GenerateRay(uint32_t x, uint32_t y, Ray* OutRay) override;
private:
	Transform CameraToNDC;
	Transform NDCToScreen;
	Transform ScreenToNDC;
	Transform ScreenToCamera;
	Bounds2i Viewport;
	float FocalLength;
};