#include "Engine.h"

#include "StaticMesh.h"

void JEngine::Init()
{
	FFbxImporter::ImportStaticMesh("./4gikevqde8-IronMan/ironman.FBX", NULL);
}

void JEngine::Tick()
{

}

