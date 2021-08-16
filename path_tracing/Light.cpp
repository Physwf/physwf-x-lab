#include "Light.h"
#include "Sampling.h"
#include "Scene.h"
#include "ParallelFor.h"

bool VisibilityTester::Unoccluded(const Scene& scene) const
{
	return !scene.IntersectP(p0.SpawnRayTo(p1));
}

Light::Light(int flags, const Transform& LightToWorld, int nSamples)
	:flags(flags),
	nSamples(std::max(1,nSamples)),
	LightToWorld(LightToWorld),
	WorldToLight(Inverse(LightToWorld))
{

}

LinearColor Light::Le(const Ray& r) const
{
	return LinearColor(0.f);
}

AreaLight::AreaLight(const Transform& LightToWorld, int nSamples)
	: Light((int)LightFlags::Area, LightToWorld, nSamples) 
{
}

LinearColor PointLight::Sample_Li(const Interaction& ref, const Vector2f& u, Vector3f* wi, float* pdf, VisibilityTester* vis) const
{
	*wi = Normalize(pLight - ref.p);
	*pdf = 1.f;
	*vis = VisibilityTester(ref,Interaction(pLight));
	return I / DistanceSquared(pLight , ref.p);
}

LinearColor PointLight::Power() const
{
	return 4 * PI * I;
}

float PointLight::Pdf_Li(const Interaction& ref, const Vector3f& wi) const
{
	return 0;
}

LinearColor PointLight::Sample_Le(const Vector2f& u1, const Vector2f& u2, Ray* ray, Vector3f* nLight, float* pdfPos, float* pdfDir) const
{
	*ray = Ray(pLight, UniformSampleSphere(u1));
	*nLight = ray->d;
	*pdfPos = 1;
	*pdfDir = UniformSpherePdf();
	return I;
}

void PointLight::Pdf_Le(const Ray& ray, const Vector3f& nLight, float* pdfPos, float* pdfDir) const
{
	*pdfPos = 0;
	*pdfDir = UniformSpherePdf();
}

UniformLightDistribution::UniformLightDistribution(const Scene& scene)
{
	std::vector<float> prob(scene.lights.size(), 1.f);
	distrib.reset(new Distribution1D(prob.data(),(int)prob.size()));
}

const Distribution1D* UniformLightDistribution::Lookup(const Vector3f& p) const
{
	return distrib.get();
}

std::unique_ptr<LightDistribution> CreateLightDistribution(const std::string& name, const Scene& scene)
{
	if (name == "uniform" || scene.lights.size() == 1)
		return std::unique_ptr<LightDistribution>{ new UniformLightDistribution(scene) };
	return nullptr;
}

LinearColor DisffuseAreaLight::L(const Interaction& intr, const Vector3f& w) const
{
	return (bTwoSide || Dot(intr.n, w) > 0) ? Lemit :LinearColor(0.f);
}

LinearColor DisffuseAreaLight::Power() const
{
	return (bTwoSide ? 2 : 1) * Lemit * area * PI;
}

LinearColor DisffuseAreaLight::Sample_Li(const Interaction& ref, const Vector2f& u, Vector3f* wi, float* pdf, VisibilityTester* vis) const
{
	Interaction pShape = shape->Sample(ref, u, pdf);
	if (*pdf == 0 || (pShape.p - ref.p).LengthSquared() == 0)
	{
		*pdf = 0;
		return 0.f;
	}
	*wi = Normalize(pShape.p - ref.p);
	*vis = VisibilityTester(ref, pShape);
	return L(pShape, -*wi);
}

float DisffuseAreaLight::Pdf_Li(const Interaction& ref, const Vector3f& wi) const
{
	return shape->Pdf(ref, wi);
}

LinearColor DisffuseAreaLight::Sample_Le(const Vector2f& u1, const Vector2f& u2, Ray* ray, Vector3f* nLight, float* pdfPos, float* pdfDir) const
{
	Interaction pShape = shape->Sample(u1, pdfPos);
	*nLight = pShape.n;
	Vector3f w;
	if (bTwoSide)
	{
		Vector2f u = u2;
		if (u[0] < .5f)
		{
			u[0] = std::min(u[0] * 2, OneMinusEpsilon);
			w = CosineSampleHemisphere(u);
		}
		else
		{
			u[0] = std::min((u[0] - 0.5f) * 2, OneMinusEpsilon);
			w = CosineSampleHemisphere(u);
			w.Z *= -1;
		}
		*pdfDir = 0.5f * CosineHemispherePdf(std::abs(w.Z));
	}
	else
	{
		w = CosineSampleHemisphere(u2);
		*pdfDir = CosineHemispherePdf(w.Z);
	}
	Vector3f v1, v2, n(pShape.n);
	CoordinateSystem(n, &v1, &v2);
	w = w.X * v1 + w.Y * v2 + w.Z * n;
	*ray = pShape.SpawnRay(w);
	return L(pShape, w);
}

void DisffuseAreaLight::Pdf_Le(const Ray& ray, const Vector3f& n, float* pdfPos, float* pdfDir) const
{
	Interaction it(ray.o, n, Vector3f(n));
	*pdfPos = shape->Pdf(it);
	*pdfDir = bTwoSide ? (.5 * CosineHemispherePdf(AbsDot(n, ray.d))) : CosineHemispherePdf(Dot(n, ray.d));
}

InfiniteAreaLight::InfiniteAreaLight(const Transform& LightToWorld, const LinearColor& L, int nSamples, const std::string& texturefile) 
	: Light((int)LightFlags::Infinite, LightToWorld)
{
	Vector2i resolution;
	std::unique_ptr<LMSColor[]> texels(nullptr);
	if (texturefile != "")
	{
		texels = ReadImage(texturefile, &resolution);
	}

	Lmap.reset(new MipMap<LMSColor>(resolution, texels.get()));

	int width = 2 * Lmap->Width(), height = Lmap->Height();
	std::unique_ptr<float[]> img(new float[width * height]);
	float fwidth = 0.5f / std::min(width, height);
	ParallelFor([&](int64_t v) 
		{
			float vp = (v + 0.5f) / (float)height;
			float sinTheta = std::sin(PI * (v + 0.5f) / height);
			for (int u = 0; u < width; ++u)
			{
				float up = (u + .5f) / (float)width;
				img[u + v * width] = Lmap->LookUp(Vector2f(up,vp), fwidth).y();
				img[u + v * width] *= sinTheta;
			}
		}, height, 32);

	distribution.reset(new Distribution2D(img.get(), width, height));
}

LinearColor InfiniteAreaLight::Sample_Li(const Interaction& ref, const Vector2f& u, Vector3f* wi, float* pdf, VisibilityTester* vis) const
{
	float mapPdf;
	Vector2f uv = distribution->SampleContinuous(u, &mapPdf);
	if (mapPdf == 0) return 0;
	
	float theta = uv[1] * PI, phi = uv[0] * 2 * PI;
	float cosTheta = std::cos(theta), sinTheta = std::sin(theta);
	float sinPhi = std::sin(phi), cosPhi = std::cos(phi);
	*wi = LightToWorld.Vector(Vector3f(sinTheta*cosPhi,sinTheta*sinPhi,cosTheta));
	*pdf = mapPdf / (2 * PI * PI * sinTheta);
	if (sinTheta == 0) *pdf = 0;

	*vis = VisibilityTester(ref, Interaction(ref.p + *wi * (2 * worldRadius)));

	return Lmap->LookUp(uv);
}

LinearColor InfiniteAreaLight::Power() const
{
	return PI * worldRadius * worldRadius * LinearColor(Lmap->LookUp(Vector2f(0.5f, 0.5f), 0.5f));
}

LinearColor InfiniteAreaLight::Le(const Ray& r) const
{
	Vector3f w = Normalize(WorldToLight.Vector(r.d));
	Vector2f st(SphericalPhi(w) * INV_2PI, SphericalTheta(w) * INV_PI);
	return LinearColor(Lmap->LookUp(st));
}

float InfiniteAreaLight::Pdf_Li(const Interaction& ref, const Vector3f& wiW) const
{
	Vector3f wi = WorldToLight.Vector(wiW);
	float theta = SphericalTheta(wi), phi = SphericalPhi(wi);
	float sinTheta = std::sin(theta);
	if (sinTheta == 0) return 0;
	return distribution->Pdf(Vector2f(phi * INV_2PI, theta * INV_PI) / (2 * PI * PI * sinTheta));
}

LinearColor InfiniteAreaLight::Sample_Le(const Vector2f& u1, const Vector2f& u2, Ray* ray, Vector3f* nLight, float* pdfPos, float* pdfDir) const
{
	Vector2f u = u1;
	float mapPdf;
	Vector2f uv = distribution->SampleContinuous(u, &mapPdf);
	if (mapPdf == 0) return 0;
	float theta = uv[1] * PI, phi = uv[0] * 2 * PI;
	float cosTheta = std::cos(theta), sinTheta = std::sin(theta);
	float sinPhi = std::sin(phi), cosPhi = std::cos(phi);
	Vector3f d = -LightToWorld.Vector(Vector3f(sinTheta * cosPhi, sinTheta * sinPhi, cosTheta));
	*nLight = d;

	Vector3f v1, v2;
	CoordinateSystem(-d, &v1, &v2);
	Vector2f cd = ConcentricSampleDisk(u2);
	Vector3f pDisk = worldCenter + worldRadius * (cd.X * v1 + cd.Y * v2);
	*ray = Ray(pDisk + worldRadius * -d, d);

	*pdfDir = sinTheta == 0 ? 0 : mapPdf / (2 * PI * PI * sinTheta);
	*pdfPos = 1 / (PI * worldRadius * worldRadius);

	return Lmap->LookUp(uv);
}

void InfiniteAreaLight::Pdf_Le(const Ray& ray, const Vector3f& nLight, float* pdfPos, float* pdfDir) const
{
	Vector3f d = -WorldToLight.Vector(ray.d);
	float theta = SphericalTheta(d), phi = SphericalPhi(d);
	Vector2f uv(phi * INV_2PI, theta * INV_PI);
	float mapPdf = distribution->Pdf(uv);
	*pdfDir = mapPdf / (2 * PI * PI * std::sin(theta));
	*pdfPos = 1 / (PI * worldRadius * worldRadius);
}
