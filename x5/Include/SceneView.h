#pragma once

#include "RenderCore/RenderResource.h"
#include "Math/XMath.h"

struct FSceneViewProjectionData
{
	FVector VeiwOrigin;

	FMatrix ViewRotationMatrix;

	FMatrix ProjectionMatrix;

protected:
	FIntRect ViewRect;

	FIntRect ConstraintedViewRect;


};

struct FViewMatrices
{
	FViewMatrices()
	{

	}

private:
	FMatrix ProjectionMatrix;
	FMatrix ProjectionNoAAMatrix;
	FMatrix InvProjectionMatrix;
	FMatrix ViewMatrix;
	FMatrix InvViewMatrix;
	FMatrix ViewProjectionMatrix;
	FMatrix InvViewProjectionMatrix;
	FMatrix TranslatedViewMatrix;
	FMatrix InvTranslatedViewMatrix;
	//FMatrix OverriddenTranslatedViewMatrix;
	FVector PreViewTranslation;
	FVector ViewOrigin;
	FVector2D ProjectionScale;
	FVector2D TemporalAAProjectionJitter;
	float ScreenScale;

public:
	void UpdateViewMatrix(const FVector& ViewLocation, const FRotator& ViewRotation);

	inline const FMatrix& GetProjectionMatrix() const
	{
		return ProjectionMatrix;
	}
	inline const FMatrix& GetProjectionNoAAMatrix() const
	{
		return ProjectionNoAAMatrix;
	}
	inline const FMatrix& GetInvProjectionMatrix() const
	{
		return InvProjectionMatrix;
	}
	inline const FMatrix& GetViewMatrix() const
	{
		return ViewMatrix;
	}
	inline const FMatrix& GetInvViewMatrix() const
	{
		return InvViewMatrix;
	}
	inline const FMatrix& GetViewProjectionMatrix() const
	{
		return ViewProjectionMatrix;
	}
	inline const FMatrix& GetInvViewProjectionMatrix() const
	{
		return InvViewProjectionMatrix;
	}
	inline const FMatrix& GetTranslatedViewMatrix() const
	{
		return TranslatedViewMatrix;
	}
	inline const FMatrix& GetInvTranslatedViewMatrix() const
	{
		return InvTranslatedViewMatrix;
	}
	inline const FVector& GetPreViewTranslation() const
	{
		return PreViewTranslation;
	}
	inline const FVector& GetViewOrigin() const
	{
		return ViewOrigin;
	}
	inline float GetScreenScale() const
	{
		return ScreenScale;
	}
	inline const FVector2D& GetProjectionScale() const
	{
		return ProjectionScale;
	}
	inline bool IsPerspectiveProjection() const
	{
		return ProjectionMatrix.M[3][3] < 1.0f;
	}

	void SaveProjectionNoAAMatrix()
	{
		ProjectionNoAAMatrix = ProjectionMatrix;
	}

	inline const FVector2D GetTemporalAAJitter() const
	{
		return TemporalAAProjectionJitter;
	}
};

class FSceneView
{
public:
	FViewMatrices ViewMatrices;
};