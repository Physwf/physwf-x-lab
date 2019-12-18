#include "Engine.h"

#include "StaticMesh.h"

void JEngine::Init()
{
	(new FFbxImporter())->ImportStaticMesh("./4gikevqde8-IronMan/ironman.FBX", NULL);
}

void JEngine::Tick()
{

}

