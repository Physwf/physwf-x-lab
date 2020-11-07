#pragma once

#include "Mesh.h"
#include "Light.h"
#include "Camera.h"

struct ViewMatrices
{
	ViewMatrices() 
	{
		ProjectionMatrix.SetIndentity();
		ViewMatrix.SetIndentity();
		TranslatedViewMatrix.SetIndentity();
		TranslatedViewProjectionMatrix.SetIndentity();
		InvTranslatedViewMatrix.SetIndentity();
		PreViewTranslation = Vector(0.0f, 0.0f, 0.0f);
		ViewOrigin = Vector(0.0f, 0.0f, 0.0f);
	}

	ViewMatrices(const Vector& ViewOrigin, const Matrix& InViewMatrix, const Matrix& ProjectMatrix);
private:
	Matrix ProjectionMatrix;//ViewToClip
	Matrix InvProjectionMatrix;//ClipToView
	Matrix ViewMatrix;//WorldToView
	Matrix InvViewMatrix;//ViewToWorld
	Matrix ViewProjectionMatrix;//WorldToClip
	Matrix InvViewProjectionMatrix;//ClipToWorld

	Matrix TranslatedViewMatrix;//
	Matrix InvTranslatedViewMatrix;//
	Matrix OverriddenTranslatedViewMatrix;//
	Matrix OverriddenInvTranslatedViewMatrix;//
	Matrix TranslatedViewProjectionMatrix;//
	Matrix InvTranslatedViewProjectionMatrix;//

	Vector PreViewTranslation;
	Vector ViewOrigin;

public:
	void UpdateViewMatrix(const Vector& ViewLocation);

	inline const Matrix& GetProjectionMatrix() const
	{
		return ProjectionMatrix;
	}
	inline const Matrix& GetInvProjectionMatrix() const
	{
		return InvProjectionMatrix;
	}
	inline const Matrix& GetViewMatrix() const
	{
		return ViewMatrix;
	}
	inline const Matrix& GetInvViewMatrix() const
	{
		return InvViewMatrix;
	}
	inline const Matrix& GetViewProjectionMatrix() const
	{
		return ViewProjectionMatrix;
	}
	inline const Matrix& GetInvViewProjectionMatrix() const
	{
		return InvViewProjectionMatrix;
	}

	inline const Matrix& GetTranslatedViewMatrix() const
	{
		return TranslatedViewMatrix;
	}
	inline const Matrix& GetInvTranslatedViewMatrix() const
	{
		return InvTranslatedViewMatrix;
	}
	inline const Matrix& GetOverriddenTranslatedViewMatrix() const
	{
		return OverriddenTranslatedViewMatrix;
	}
	inline const Matrix& GetOverriddenInvTranslatedViewMatrix() const
	{
		return OverriddenInvTranslatedViewMatrix;
	}
	inline const Matrix& GetTranslatedViewProjectionMatrix() const
	{
		return TranslatedViewProjectionMatrix;
	}
	inline const Matrix& GetInvTranslatedViewProjectionMatrix() const
	{
		return InvTranslatedViewProjectionMatrix;
	}

	inline const Vector& GetPreViewTranslation() const
	{
		return PreViewTranslation;
	}
	inline const Vector& GetViewOrigin() const
	{
		return ViewOrigin;
	}
};


Vector4 CreateInvDeviceZToWorldZTransform(const Matrix& ProjMatrix);

extern std::vector<MeshBatch> AllBatches;
extern ID3D11Buffer* ViewUniformBuffer;
extern DirectionalLight DirLight;
extern Camera MainCamera;
extern ViewMatrices VMs;

void InitScene();
void UpdateView();