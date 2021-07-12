#include "Scene.h"
#include "PathTracing.h"
#include <Windows.h>
#include "ToneMapping.h"

std::shared_ptr<Scene> BuildTestScene()
{
	std::vector<std::shared_ptr<SceneObject>> objects;
	std::vector<std::shared_ptr<Light>> lights;

	std::vector<Vector3f> Ps = { {-100.f,100.f,0.f},{100.f,100.f,0.f},{100.f,-100.f,0.f},{-100.f,-100.f,0.f}, };
	std::vector<Vector3f> Ns = { {0.f,0.f,1.f}, {0.f,0.f,1.f}, {0.f,0.f,1.f}, {0.f,0.f,1.f}, };
	std::vector<Vector2f> UVs = { {0,0}, {1.f,0}, {1.f,1.0f},  {0.f,1.0f} };
	std::vector<int> indices = {0,1,2,2,3,0};

	Transform leftWallR = Transform::Rotate(0, -PI * 0.25f, 0);
	Transform leftWallT = Transform::Translate(-100.f, 0, 0);
	LinearColor pink;
	std::shared_ptr<Material> leftWallMat = std::make_shared<MatteMaterial>(pink);
	std::shared_ptr<SceneObject> leftWall = std::make_shared<MeshObject>(leftWallT * leftWallR, leftWallMat,2,4,Ps.data(),Ns.data(),UVs.data(), indices);

	Transform rightWallR = Transform::Rotate(0, PI * 0.25f, 0);
	Transform rightWallT = Transform::Translate(100.f, 0, 0);
	LinearColor blue;
	std::shared_ptr<Material> rightWallMat = std::make_shared<MatteMaterial>(blue);
	std::shared_ptr<SceneObject> rightWall = std::make_shared<MeshObject>(rightWallT * rightWallR, rightWallMat, 2, 4, Ps.data(), Ns.data(), UVs.data(), indices);

	Transform topWallR = Transform::Rotate(-PI * 0.25f, 0, 0);
	Transform topWallT = Transform::Translate(0, 100.f, 0);
	LinearColor gray;
	std::shared_ptr<Material> topWallMat = std::make_shared<MatteMaterial>(gray);
	std::shared_ptr<SceneObject> topWall = std::make_shared<MeshObject>(topWallT * topWallR, topWallMat, 2, 4, Ps.data(), Ns.data(), UVs.data(), indices);

	Transform backWallT = Transform::Translate(0, 0 , 100.f);
	std::shared_ptr<SceneObject> backWall = std::make_shared<MeshObject>(backWallT, topWallMat, 2, 4, Ps.data(), Ns.data(), UVs.data(), indices);

	Transform bottomWallR = Transform::Rotate(PI * 0.25f, 0, 0);
	Transform bottomWallT = Transform::Translate(0, -100.f, 0.f);
	std::shared_ptr<SceneObject> bottomWall = std::make_shared<MeshObject>(bottomWallT * bottomWallR, topWallMat, 2, 4, Ps.data(), Ns.data(), UVs.data(), indices);

	Transform ballT = Transform::Translate(0, -75.f, 0);
	std::shared_ptr<Material> ballMat = std::make_shared<MatteMaterial>(gray);
	std::shared_ptr<Shape> sphere = std::make_shared<Sphere>(25.0f);
	std::shared_ptr<SceneObject> ball = std::make_shared<GeometryObject>(ballT, ballMat, sphere);


	objects.push_back(leftWall);
	objects.push_back(rightWall);
	objects.push_back(topWall);
	objects.push_back(backWall);
	objects.push_back(bottomWall);
	objects.push_back(ball);

	Transform lightT = Transform::Translate(0, 100.f, 0);
	LinearColor White(1000.f);
	std::shared_ptr<Light> pl = std::make_shared<PointLight>(lightT, White);
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
	Film* film = new Film(Vector2i(500, 500),std::make_unique<GaussianFilter>(Vector2f(0.1f,0.1f),0.5f));
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