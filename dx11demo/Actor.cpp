#include "Actor.h"

Actor::Actor()
{
	Position = { 0, 0, 0 };
	Rotation = { 0, 0, 0 };
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


