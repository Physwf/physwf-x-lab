#include "Scene.h"
#include "PathTracing.h"
#include <Windows.h>
#include "ToneMapping.h"
#include "ThreadPool.h"
#include "Light.h"
#include "Geometry.h"

#include <thread>

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

	//LinearColor eta(0.8f, 0.6f, 0.3f);
	LinearColor eta(1.5f, 1.5f, 1.5f);
	LinearColor k(2.5f, 2.5f, 2.5f);
	std::shared_ptr<Material> metalMat = std::make_shared<MetalMaterial>(eta, k, 0.1f);

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

	Transform ballT = Transform::Translate(0, -0.0f, 0);
	std::shared_ptr<Material> ballMat = std::make_shared<MatteMaterial>(gray);

	std::shared_ptr<Shape> sphere = std::make_shared<Sphere>(0.4f);
	std::shared_ptr<SceneObject> ball = std::make_shared<GeometryObject>(ballT, metalMat, sphere);

	objects.push_back(ball);
	objects.push_back(backWall);
	objects.push_back(leftWall);
	objects.push_back(rightWall);
	objects.push_back(topWall);
	objects.push_back(bottomWall);

	Transform lightR = Transform::Rotate(PI_2, 0, 0);
	Transform lightT = Transform::Translate(0, 0.99f * fSize, zOffset);
	LinearColor White(100.f);
	std::shared_ptr<Light> pl = std::make_shared<PointLight>(lightT, White);
	std::shared_ptr<Shape> d = std::make_shared<Disk>(0.2f);
	std::shared_ptr<Light> dl = std::make_shared<DisffuseAreaLight>(lightR * lightT, White,d, 1024, false);
	//lights.push_back(pl);
	lights.push_back(dl);

	Transform Identity = Transform::Identity();
	std::shared_ptr<Light> il = std::make_shared<InfiniteAreaLight>(Identity,1.f,1024,"immenstadter_horn_1k.hdr");
	//lights.push_back(il);

	std::shared_ptr<Scene> scene = std::make_shared<Scene>(objects, lights);
	return scene;
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
}

void Test_PathTracing()
{
	Transform cameraD = Transform::LookAt({ 0,0,zOffset -2.f * fSize }, { 0,0,1 }, { 0,1,0 });
	//Transform cameraD = Transform::LookAt({ 0,zOffset - 2.f * fSize,0 }, { 0,1,0 }, { 0,0,1 });
	film = new Film(Vector2i(500, 500),std::make_unique<GaussianFilter>(Vector2f(1.0f, 1.0f),0.1f));
	camera = std::make_shared<PerspectiveCamera>(Inverse(cameraD), film, PI_2, 1.f,1000.f);
	sampler = Sampler::CreateStratified(8, 8, 1);
	buf = std::unique_ptr<BYTE[]>(new BYTE[film->fullResolution.X * film->fullResolution.Y * 3]);

	float rrThreshold = 0.001f;
	integrator = std::make_shared<PathIntergrator>(32, camera, sampler, rrThreshold);
	scene = BuildTestScene();
	integrator->SetProgressListener(OnPathTracingProgress);
	integrator->Render(*scene.get());
}

void StartGround()
{
	InitFixedThreadPool();
	(new std::thread(Test_PathTracing))->detach();
}