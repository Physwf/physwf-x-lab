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

ViewMatrices::ViewMatrices(const Vector& InViewOrigin, const Matrix& InViewMatrix, const Matrix& InProjectionMatrix)
{
	Vector LocalViewOrigin = InViewOrigin;
	Matrix ViewRotationMatrix = InViewMatrix;

	ViewMatrix = TranslationMatrix(-LocalViewOrigin) * ViewProjectionMatrix;
	ProjectionMatrix = InProjectionMatrix;
	InvProjectionMatrix = ProjectionMatrix.Inverse();

	ViewProjectionMatrix = GetViewMatrix() * GetProjectionMatrix();
	
	InvViewMatrix = ViewRotationMatrix.GetTransposed() * TranslationMatrix(LocalViewOrigin);
	InvViewProjectionMatrix = InvProjectionMatrix * InvViewMatrix;

	bool bApplyPreViewTranslation = true;

	if (true)
	{
		ViewOrigin = LocalViewOrigin;
	}

	Matrix LocalTranslatedViewMatrix = ViewRotationMatrix;
	Matrix LocalInvTranslatedViewMatrix = LocalTranslatedViewMatrix.GetTransposed();

	if (bApplyPreViewTranslation)
	{
		PreViewTranslation = -Vector(LocalViewOrigin);
	}

	TranslatedViewMatrix = LocalTranslatedViewMatrix;
	InvTranslatedViewMatrix = LocalInvTranslatedViewMatrix;

	OverriddenTranslatedViewMatrix = TranslationMatrix(-GetPreViewTranslation()) * GetViewMatrix();
	OverriddenInvTranslatedViewMatrix = GetInvViewMatrix() * TranslationMatrix(GetPreViewTranslation());

	TranslatedViewProjectionMatrix = LocalTranslatedViewMatrix * ProjectionMatrix;
	InvTranslatedViewProjectionMatrix = InvProjectionMatrix * LocalInvTranslatedViewMatrix;
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

void ViewMatrices::UpdateViewMatrix(const Vector& ViewLocation)
{
	ViewOrigin = ViewLocation;

	Matrix ViewRotationMatrix;
	ViewRotationMatrix.SetIndentity();

	ViewMatrix = TranslationMatrix(-ViewLocation) * ViewRotationMatrix;

	ViewProjectionMatrix = GetViewMatrix() * GetProjectionMatrix();

	InvViewMatrix = ViewRotationMatrix.GetTransposed() * TranslationMatrix(ViewLocation);
	InvViewProjectionMatrix = GetInvProjectionMatrix() * GetInvViewMatrix();

	PreViewTranslation = -ViewOrigin;

	TranslatedViewMatrix = ViewRotationMatrix;
	InvTranslatedViewMatrix = TranslatedViewMatrix.GetTransposed();
	OverriddenTranslatedViewMatrix = TranslationMatrix(-PreViewTranslation) * ViewMatrix;
}

Vector4 CreateInvDeviceZToWorldZTransform(const Matrix& ProjMatrix)
{
	/*
	00, 01, 02, 03
	10, 11, 12, 13
	20, 21, 22, 23
	30, 31, 32, 33
	*/
	float DepthMul = ProjMatrix.M[2][2];
	float DepthAdd = ProjMatrix.M[3][2];

	bool bIsPerspectiveProjection = ProjMatrix.M[3][3] < 1.0f;

	if (bIsPerspectiveProjection)
	{
		float SubtractValue = DepthMul / DepthAdd;

		// Subtract a tiny number to avoid divide by 0 errors in the shader when a very far distance is decided from the depth buffer.
		// This fixes fog not being applied to the black background in the editor.
		SubtractValue -= 0.00000001f;

		return Vector4(
			0.0f,
			0.0f,
			1.0f / DepthAdd,
			SubtractValue
		);
	}
	else
	{
		return Vector4(
			1.0f / ProjMatrix.M[2][2],
			-ProjMatrix.M[3][2] / ProjMatrix.M[2][2] + 1.0f,
			0.0f,
			1.0f
		);
	}
}
