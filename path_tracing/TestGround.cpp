#include "Scene.h"
#include "PathTracing.h"
#include <Windows.h>
#include "ToneMapping.h"
#include "ThreadPool.h"
#include "Light.h"
#include "Geometry.h"

struct MeshData
{
	std::vector<Vector3f> Ps;
	std::vector<Vector3f> Ns;
	std::vector<Vector2f> UVs;
	std::vector<int> indices;
};

float zOffset = 0;
float fSize = 1.f;

std::shared_ptr<Scene> BuildTestScene()
{
	std::vector<std::shared_ptr<SceneObject>> objects;
	std::vector<std::shared_ptr<Light>> lights;

	static MeshData md;

	md.Ps = { {-1.f * fSize,1.f * fSize,0.f},{1.f * fSize,1.f * fSize,0.f},{1.f * fSize,-1.f * fSize,0.f},{-1.f * fSize,-1.f * fSize,0.f}, };
	md.Ns = { {0.f,0.f,1.f}, {0.f,0.f,1.f}, {0.f,0.f,1.f}, {0.f,0.f,1.f}, };
	md.UVs = { {0,0}, {1.f,0}, {1.f,1.0f},  {0.f,1.0f} };
	md.indices = {0,1,2,2,3,0};

	Transform leftWallR = Transform::Rotate(0, -PI_2, 0);
	Transform leftWallT = Transform::Translate(-fSize, 0, zOffset);
	LinearColor pink(1.0f,0.1,0.4);
	std::shared_ptr<Material> leftWallMat = std::make_shared<MatteMaterial>(pink);
	std::shared_ptr<SceneObject> leftWall = std::make_shared<MeshObject>(leftWallR * leftWallT, leftWallMat, 2, 4, md.Ps.data(), md.Ns.data(), md.UVs.data(), md.indices);

	Transform rightWallR = Transform::Rotate(0, PI_2, 0);
	Transform rightWallT = Transform::Translate(fSize, 0, zOffset);
	LinearColor blue(0.0f, 0.3, 0.9);
	std::shared_ptr<Material> rightWallMat = std::make_shared<MatteMaterial>(blue);
	std::shared_ptr<SceneObject> rightWall = std::make_shared<MeshObject>(rightWallR * rightWallT, rightWallMat, 2, 4, md.Ps.data(), md.Ns.data(), md.UVs.data(), md.indices);

	Transform topWallR = Transform::Rotate(-PI_2, 0, 0);
	Transform topWallT = Transform::Translate(0, fSize, zOffset);
	LinearColor gray(0.6f, 0.6, 0.6);
	std::shared_ptr<Material> topWallMat = std::make_shared<MatteMaterial>(gray);
	std::shared_ptr<SceneObject> topWall = std::make_shared<MeshObject>(topWallR * topWallT, topWallMat, 2, 4, md.Ps.data(), md.Ns.data(), md.UVs.data(), md.indices);

	Transform backWallR = Transform::Rotate(0, 0, PI_2);
	Transform backWallT = Transform::Translate(0, 0, fSize);
	std::shared_ptr<SceneObject> backWall = std::make_shared<MeshObject>(backWallT, topWallMat, 2, 4, md.Ps.data(), md.Ns.data(), md.UVs.data(), md.indices);

	Transform bottomWallR = Transform::Rotate(PI_2, 0, 0);
	Transform bottomWallT = Transform::Translate(0, -fSize, zOffset);
	std::shared_ptr<SceneObject> bottomWall = std::make_shared<MeshObject>(bottomWallR * bottomWallT, topWallMat, 2, 4, md.Ps.data(), md.Ns.data(), md.UVs.data(), md.indices);

	Transform ballT = Transform::Translate(0, -75.f, 0);
	std::shared_ptr<Material> ballMat = std::make_shared<MatteMaterial>(gray);
	std::shared_ptr<Shape> sphere = std::make_shared<Sphere>(25.f);
	std::shared_ptr<SceneObject> ball = std::make_shared<GeometryObject>(ballT, ballMat, sphere);

	objects.push_back(ball);
	objects.push_back(backWall);
	objects.push_back(leftWall);
	objects.push_back(rightWall);
	objects.push_back(topWall);
	objects.push_back(bottomWall);

	Transform lightR = Transform::Rotate(PI_2, 0, 0);
	Transform lightT = Transform::Translate(0, 0.90f * fSize, zOffset);
	LinearColor White(10.f);
	std::shared_ptr<Light> pl = std::make_shared<PointLight>(lightT, White);
	std::shared_ptr<Shape> d = std::make_shared<Disk>(0.2f);
	std::shared_ptr<Light> dl = std::make_shared<DisffuseAreaLight>(lightR * lightT, White,d, 1024, false);
	//lights.push_back(pl);
	lights.push_back(dl);

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
	errno_t err = fopen_s(&stream, fileName, "w+b");
	if (err == 0)
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

Film* film;
std::shared_ptr<PathIntergrator> integrator;
std::shared_ptr<Camera> camera;
std::shared_ptr<Sampler> sampler;
std::shared_ptr<Scene> scene;

std::unique_ptr<BYTE[]> buf;

void OnPathTracingProgress(Vector2i tile)
{
	std::unique_ptr<float[]> buffer = film->GetBuffer();

	ToneMapping(buffer.get(), film->fullResolution.X, film->fullResolution.Y, buf.get(), TMA_ACES);

	void Display(BYTE* Data, size_t size, int width, int height);
	Display(buf.get(), film->fullResolution.X * film->fullResolution.Y * 3, film->fullResolution.X, film->fullResolution.Y);

	//WriteImageBMP("conabox.bmp", film->fullResolution.X, film->fullResolution.Y, buf.get());
}

void Test_PathTracing()
{
	Transform cameraD = Transform::LookAt({ 0,0,zOffset -2.f * fSize }, { 0,0,1 }, { 0,1,0 });
	film = new Film(Vector2i(500, 500),std::make_unique<GaussianFilter>(Vector2f(.5f, 0.5f),0.1f));
	camera = std::make_shared<PerspectiveCamera>(Inverse(cameraD), film, PI_2, 1.f,1000.f);
	sampler = Sampler::CreateStratified(8, 8, 1);
	buf = std::unique_ptr<BYTE[]>(new BYTE[film->fullResolution.X * film->fullResolution.Y * 3]);

	float rrThreshold = 00.1f;
	integrator = std::make_shared<PathIntergrator>(8, camera, sampler, rrThreshold);
	scene = BuildTestScene();
	integrator->SetProgressListener(OnPathTracingProgress);
	integrator->Render(*scene.get());
}

void StartGround()
{
	InitFixedThreadPool();
	Test_PathTracing();
}