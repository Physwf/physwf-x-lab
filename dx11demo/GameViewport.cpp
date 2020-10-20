#include "GameViewport.h"
#include "Camera.h"

extern Camera MainCamera;

void GameViewport::Draw()
{

}

void GameViewport::OnKeyDown(unsigned int KeyCode)
{
	switch(KeyCode)
	{
	case 0x41:
		MainCamera.Side(5.0f);
		break;
	case 0x44:
		MainCamera.Side(-5.0f);
		break;
	case 0x57:
		MainCamera.Walk(5.0f);
		break;
	case 0x53:
		MainCamera.Walk(-5.0f);
		break;
	case VK_SPACE:
		MainCamera.Lift(5.0f);
		break;
	}
}

void GameViewport::OnKeyUp(unsigned int KeyCode)
{

}

void GameViewport::OnMouseDown(int X, int Y)
{

}

void GameViewport::OnMouseUp(int X, int Y)
{

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
}

