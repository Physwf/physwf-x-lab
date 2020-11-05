#include "Actor.h"

Actor::Actor()
{
	Position = { 0, 0, 0 };
	Rotation = { 0, 0, 0 };
	bDraging = false;
	bTransformDirty = false;
}

Actor::~Actor()
{

}

void Actor::SetPosition(float fX, float fY, float fZ)
{
	Position = { fX, fY, fZ };
}

void Actor::SetRotation(float fRoll, float fPitch, float fYall)
{
	Rotation = { fRoll, fPitch, fYall };
}

void Actor::Pitch(float fValue)
{
	Rotation.X += fValue;
}

void Actor::Yall(float fValue)
{
	Rotation.Y += fValue;
}

void Actor::Roll(float fValue)
{
	Rotation.Z += fValue;
}

void Actor::StartDrag(int X, int Y)
{
	bDraging = true;
	DragStartX = X;
	DragStartY = Y;
	//StartFaceDir = FaceDir;
}

void Actor::Drag(int X, int Y)
{
	if (bDraging)
	{
		int Dx = X - DragStartX;
		int Dy = Y - DragStartY;
		if (Math::Abs(Dx) > Math::Abs(Dy))
		{
			Dy = 0;
		}
		else
		{
			Dx = 0;
		}
		float fDx = -Dx / 10000.f;
		float fDy = Dy / 10000.f;
		Rotation += { fDy, fDx, 0.0f };
		bTransformDirty = true;
// 		Matrix Rotation = Matrix::DXFormRotation();
// 		FaceDir = Rotation.Transform(StartFaceDir);
	}
}

void Actor::StopDrag(int X, int Y)
{
	if (bDraging)
	{
		int Dx = X - DragStartX;
		int Dy = Y - DragStartY;
		if (Math::Abs(Dx) > Math::Abs(Dy))
		{
			Dy = 0;
		}
		else
		{
			Dx = 0;
		}
		float fDx = -Dx / 10000.f;
		float fDy = Dy / 10000.f;
// 		Matrix Rotation = Matrix::DXFormRotation({ fDy, fDx, 0.0f });
// 		FaceDir = Rotation.Transform(StartFaceDir);
		Rotation += { fDy, fDx, 0.0f };
		bTransformDirty = true;
	}
	bDraging = false;
}

