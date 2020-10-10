#include "Scene.h"
#include "log.h"
#include "D3D11RHI.h"

void Scene::InitResource()
{

}

void Scene::ReleaseResource()
{

}

void Scene::Setup()
{

}
// 
// void Scene::OnKeyDown(unsigned int KeyCode)
// {
// 	switch (KeyCode)
// 	{
// 	case 'W':
// 	case 'w':
// 		if (CurrentCamera)
// 		{
// 			CurrentCamera->Walk(20.0f);
// 		}
// 		break;
// 	case 'A':
// 	case 'a':
// 		if (CurrentCamera)
// 		{
// 			CurrentCamera->Side(20.0f);
// 		}
// 		break;
// 	case 'D':
// 	case 'd':
// 		if (CurrentCamera)
// 		{
// 			CurrentCamera->Side(-20.0f);
// 		}
// 		break;
// 	case 'S':
// 	case 's':
// 		if (CurrentCamera)
// 		{
// 			CurrentCamera->Back(20.0f);
// 		}
// 		break;
// 	}
// }
// 
// void Scene::OnKeyUp(unsigned int KeyCode)
// {
// 
// }
// 
// void Scene::OnMouseDown(int X, int Y)
// {
// 	if (CurrentCamera)
// 	{
// 		CurrentCamera->StartDrag(X,Y);
// 	}
// }
// 
// void Scene::OnMouseUp(int X, int Y)
// {
// 	if (CurrentCamera)
// 	{
// 		CurrentCamera->StopDrag(X, Y);
// 	}
// }
// 
// void Scene::OnRightMouseDown(int X, int Y)
// {
// 	if (CurrentCamera)
// 	{
// 		CurrentCamera->StartRotate(X, Y);
// 	}
// }
// 
// void Scene::OnRightMouseUp(int X, int Y)
// {
// 	if (CurrentCamera)
// 	{
// 		CurrentCamera->StopRotate(X, Y);
// 	}
// }
// 
// void Scene::OnMouseMove(int X, int Y)
// {
// 	if (CurrentCamera)
// 	{
// 		CurrentCamera->Drag(X, Y);
// 		CurrentCamera->Rotate(X, Y);
// 	}
// }
