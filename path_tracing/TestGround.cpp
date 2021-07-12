#include "Scene.h"
#include "PathTracing.h"
#include <Windows.h>
#include "ToneMapping.h"

std::shared_ptr<Scene> BuildTestScene()
{
	std::vector<std::shared_ptr<SceneObject>> objects;
	std::vector<std::shared_ptr<Light>> lights;

	std::shared_ptr<SceneObject> leftWall = std::make_shared<MeshObject>();
	std::shared_ptr<SceneObject> rightWall = std::make_shared<MeshObject>();
	std::shared_ptr<SceneObject> topWall = std::make_shared<MeshObject>();
	std::shared_ptr<SceneObject> backWall = std::make_shared<MeshObject>();
	std::shared_ptr<SceneObject> bottomWall = std::make_shared<MeshObject>();

	std::shared_ptr<SceneObject> ball = std::make_shared<GeometryObject>();

	objects.push_back(leftWall);
	objects.push_back(rightWall);
	objects.push_back(topWall);
	objects.push_back(backWall);
	objects.push_back(bottomWall);
	objects.push_back(ball);

	std::shared_ptr<Light> pl = std::make_shared<PointLight>();
	lights.push_back(pl);

	std::shared_ptr<Scene> scene = std::make_shared<Scene>(objects, lights);
	return scene;
}

void WriteImageBMP(const char* fileName, int width, int height, BYTE* rgbbuf)
{
	BITMAPFILEHEADER bfh;
	BITMAPINFOHEADER bih;
	bfh.bfType = 0x4d42;
	bfh.bfReserved1 = 0;
	bfh.bfReserved2 = 0;
	bfh.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + width * height * 3;
	bfh.bfOffBits = 0x36;

	bih.biSize = sizeof(BITMAPINFOHEADER);
	bih.biWidth = width;
	bih.biHeight = height;
	bih.biPlanes = 1;
	bih.biBitCount = 24;
	bih.biCompression = 0;
	bih.biSizeImage = 0;
	bih.biXPelsPerMeter = 0;
	bih.biYPelsPerMeter = 0;
	bih.biClrUsed = 0;
	bih.biClrImportant = 0;

	FILE* stream;
	if (0 != fopen_s(&stream, fileName, "rw"))
	{
		fwrite(&bfh, sizeof(bfh), 1, stream);
		fwrite(&bih, sizeof(bih), 1, stream);
		fwrite(rgbbuf, width * height * 3, 1, stream);
		fclose(stream);
	}
}

void WriteImageHDR(const char* fileName, int width, int height, float* buffer)
{

}

void Test_PathTracing()
{
	Transform cameraW;
	Film* film = new Film(Vector2i(500, 500),std::make_unique<GaussianFilter>());
	std::shared_ptr<Camera> camera = std::make_shared<PerspectiveCamera>(cameraW, film, 90.f,1.0f,10000.f);
	std::shared_ptr<Sampler> sampler = Sampler::CreateStratified(32,32,8);
	float rrThreshold;
	std::shared_ptr<PathIntergrator> integrator = std::make_shared<PathIntergrator>(8, camera, sampler, rrThreshold);
	std::shared_ptr<Scene> scene = BuildTestScene();
	integrator->Render(*scene.get());

	std::unique_ptr<float[]> buffer = film->GetBuffer();

	std::unique_ptr<BYTE[]> buf(new BYTE[film->fullResolution.X * film->fullResolution.Y * 3]);
	ToneMapping(buffer.get(), film->fullResolution.X, film->fullResolution.Y, buf.get());

	WriteImageBMP("conabox.bmp", film->fullResolution.X, film->fullResolution.Y, buf.get());
}