#include "GameViewport.h"
#include "Camera.h"
#include "Actor.h"
extern Camera MainCamera;
extern Actor* SelectedActor;

void GameViewport::Tick(float fDeltaTime)
{
	MainCamera.TryMove(fDeltaTime);
}

void GameViewport::OnKeyDown(unsigned int KeyCode)
{
	switch(KeyCode)
	{
	case 0x41:
		//MainCamera.Side(1.0f);
		MainCamera.StartMove(Vector(1.f,0,0));
		break;
	case 0x44:
		//MainCamera.Side(-1.0f);
		MainCamera.StartMove(Vector(-1.f, 0, 0));
		break;
	case 0x57:
		//MainCamera.Walk(1.0f);
		MainCamera.StartMove(Vector(0.f, 0, 1.f));
		break;
	case 0x53:
		//MainCamera.Walk(-1.0f);
		MainCamera.StartMove(Vector(0.f, 0, -1.f));
		break;
	case VK_SPACE:
		//MainCamera.Lift(1.0f);
		MainCamera.StartMove(Vector(0.f, 1, 0.f));
		break;
	}
}

void GameViewport::OnKeyUp(unsigned int KeyCode)
{
	MainCamera.StopMove();
}

void GameViewport::OnMouseDown(int X, int Y)
{
	if (SelectedActor)
	{
		SelectedActor->StartDrag(X, Y);
	}
}

void GameViewport::OnMouseUp(int X, int Y)
{
	if (SelectedActor)
	{
		SelectedActor->StopDrag(X,Y);
	}
}

void GameViewport::OnRightMouseDown(int X, int Y)
{
	MainCamera.StartDrag(X, Y);
}

void GameViewport::OnRightMouseUp(int X, int Y)
{
	MainCamera.StopDrag(X, Y);

}

void GameViewport::OnMouseMove(int X, int Y)
{
	MainCamera.Drag(X, Y);
	if (SelectedActor)
	{
		SelectedActor->Drag(X, Y);
	}
}

