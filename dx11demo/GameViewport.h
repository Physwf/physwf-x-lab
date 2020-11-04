#pragma once

class GameViewport
{
public:
	void Tick(float fDeltaTime);

	void OnKeyDown(unsigned int KeyCode);
	void OnKeyUp(unsigned int KeyCode);
	void OnMouseDown(int X, int Y);
	void OnMouseUp(int X, int Y);
	void OnRightMouseDown(int X, int Y);
	void OnRightMouseUp(int X, int Y);
	void OnMouseMove(int X, int Y);
};