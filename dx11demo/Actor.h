#pragma once

#include "Math.h"

class Actor
{
public:
	Actor();
	virtual ~Actor();

	void SetPosition(float fX, float fY, float fZ);
	void SetRotation(float fRoll, float fPitch, float fYall);
	void Pitch(float fValue);
	void Yall(float fValue);
	void Roll(float fValue);
	
	void StartDrag(int X, int Y);
	void Drag(int X, int Y);
	void StopDrag(int X, int Y);

protected:
	Matrix GetWorldMatrix() 
	{ 
		Matrix R = Matrix::DXFormRotation(Rotation);
		Matrix T = Matrix::DXFromTranslation(Position);
		R.Transpose();
		T.Transpose();
		return  T * R;
	}

	Vector Position;
	Vector Rotation;

	int DragStartX;
	int DragStartY;
	bool bDraging;

	bool bTransformDirty;
};