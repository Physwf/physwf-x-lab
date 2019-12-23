#pragma once

class UPrimitiveComponent;

class FSceneInterface
{
public:
	FSceneInterface()
	{}

	// FSceneInterface interface

	/**
	 * Adds a new primitive component to the scene
	 *
	 * @param Primitive - primitive component to add
	 */
	virtual void AddPrimitive(UPrimitiveComponent* Primitive) = 0;
	/**
	 * Removes a primitive component from the scene
	 *
	 * @param Primitive - primitive component to remove
	 */
	virtual void RemovePrimitive(UPrimitiveComponent* Primitive) = 0;
};