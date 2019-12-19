#include "Engine.h"

#include "FbxImporter/FbxImporter.h"

void JEngine::Init()
{
	(new FFbxImporter())->ImportStaticMesh("./4gikevqde8-IronMan/ironman.FBX", NULL);
}

void JEngine::Tick()
{

}

