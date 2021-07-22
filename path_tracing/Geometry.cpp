#include "Geometry.h"
#include "Mathematics.h"
#include "Sampling.h"

Sphere::Sphere(float InRadius)
	:  Radius(InRadius)
{
}

Bounds3f Sphere::ObjectBound() const
{
	return Bounds3f({ -Radius,-Radius,-Radius }, { Radius,Radius,Radius });
}

bool Sphere::Intersect(const Ray& ray, float* tHit, SurfaceInteraction* isect) const
{
	// x2+y2+z2=r2 o+d*s=(x,y,z) =>
	// (ox+dx*s)2 + (oy+dy*s)2 + (oz+dz*s)2 = r2
	// (dx2+dy2+dz2)s2 + 2*(ox*dx + oy*dy + oz*dz)* s + (ox2 + oy2 + oz2) - r2 = 0
	Ray LocalRay = (*WorldToLocal)(ray);
	float ox = LocalRay.o.X, oy = LocalRay.o.Y, oz = LocalRay.o.Z;
	float dx = LocalRay.d.X, dy = LocalRay.d.Y, dz = LocalRay.d.Z;
	float a = dx * dx + dy * dy + dz * dz;
	float b = 2 * (dx * ox + dy * oy + dz * oz);
	float c = ox * ox + oy * oy + oz * oz - Radius * Radius;

	float t0, t1;
	if (!Math::Quadratic(a, b, c, &t0, &t1)) return false;//no intersection

	if (t0 > LocalRay.tMax || t1 < 0) return false;//ray range out of sphere
	float tShapeHit = t0;
	if (t0 < 0)
	{
		tShapeHit = t1;
		if (tShapeHit > LocalRay.tMax) return false;//ray range inside of sphere
	}
	Vector3f pHit;
	pHit = LocalRay(tShapeHit);

	float Phi = std::atan2(pHit.Y,pHit.X);
	if (Phi < 0) Phi += 2 * PI;
	float Theta = std::acos(Math::Clamp(pHit.Z / Radius,-1.f,1.f));
	float u = Phi / PI * 0.5f;
	float v = Theta / PI;

	float zRadius = std::sqrt(pHit.X * pHit.X + pHit.Y * pHit.Y);
	float invZRadius = 1 / zRadius;
	float cosPhi = pHit.X * invZRadius;
	float sinPhi = pHit.Y * invZRadius;
	Vector3f dpdu(-2 * PI * pHit.Y, 2 * PI * pHit.X, 0);
	Vector3f dpdv = Vector3f(pHit.Z * cosPhi, pHit.Z * sinPhi, -Radius * std::sin(Theta));

	*isect = (*LocalToWorld)(SurfaceInteraction(pHit,Vector2f(u,v),-LocalRay.d, dpdu, dpdv, Vector3f(), Vector3f(),this));
	return true;
}

bool Sphere::IntersectP(const Ray& ray) const
{
	Ray LocalRay = (*WorldToLocal)(ray);
	float ox = LocalRay.o.X, oy = LocalRay.o.Y, oz = LocalRay.o.Z;
	float dx = LocalRay.d.X, dy = LocalRay.d.Y, dz = LocalRay.d.Z;
	float a = dx * dx + dy * dy + dz * dz;
	float b = 2 * (dx * ox + dy * oy + dz * oz);
	float c = ox * ox + oy * oy + oz * oz - Radius * Radius;

	float t0, t1;
	if (!Math::Quadratic(a, b, c, &t0, &t1)) return false;//no intersection

	if (t0 > LocalRay.tMax || t1 < 0) return false;//ray range out of sphere
	float tShapeHit = t0;
	if (t0 < 0)
	{
		tShapeHit = t1;
		if (tShapeHit > LocalRay.tMax) return false;//ray range inside of sphere
	}
	return true;
}

Interaction Sphere::Sample(const Vector2f& u, float* pdf) const
{
	Vector3f pObj = Radius * UniformSampleSphere(u);
	Interaction it;
	it.n = Normalize((*LocalToWorld)(pObj));
	it.p = (*LocalToWorld)(pObj);
	*pdf = 1 / Area();
	return it;
}

Interaction Sphere::Sample(const Interaction& ref, const Vector2f& u, float* pdf) const
{
	return Interaction();
}

float Sphere::Pdf(const Interaction& ref, const Vector3f& wi) const
{
	return 0.f;
}

Disk::Disk(float InRadius)
	: Radius(InRadius)
{
}

Bounds3f Disk::ObjectBound() const
{
	return Bounds3f(Vector3f(-Radius, -Radius, 0), Vector3f(Radius, Radius, 0));
}

bool Disk::Intersect(const Ray& r, float* tHit, SurfaceInteraction* isect) const
{
	Ray ray = (*WorldToLocal)(r);
	if (ray.d.Z == 0) return false;
	float tShapeHit = -ray.o.Z / ray.d.Z;
	if (tShapeHit <= 0 || tShapeHit >= ray.tMax) return false;

	Vector3f pHit = ray(tShapeHit);
	float dist2 = pHit.X * pHit.X + pHit.Y * pHit.Y;
	if (dist2 > Radius * Radius) return false;

	float phi = std::atan2(pHit.Y, pHit.X);
	if (phi < 0) phi += 2 * PI;

	float u = phi / (2 * PI);
	float rHit = std::sqrt(dist2);
	float v = (Radius - rHit) / Radius;

	Vector3f dpdu(-2 * PI * pHit.Y, 2 * PI * pHit.X, 0);
	Vector3f dpdv = Vector3f(pHit.X,pHit.Y,0.f) * (-Radius) / rHit;
	
	pHit.Z = 0;

	*isect = (*LocalToWorld)(SurfaceInteraction(pHit, Vector2f(u, v), -ray.d, dpdu, dpdv, Vector3f(), Vector3f(), this));
	*tHit = tShapeHit;

	return true;
}

bool Disk::IntersectP(const Ray& r) const
{
	Ray ray = (*WorldToLocal)(r);
	if (ray.d.Z == 0) return false;
	float tShapeHit = -ray.o.Z / ray.d.Z;
	if (tShapeHit <= 0 || tShapeHit >= ray.tMax) return false;

	Vector3f pHit = ray(tShapeHit);
	float dist2 = pHit.X * pHit.X + pHit.Y * pHit.Y;
	if (dist2 > Radius * Radius) return false;

	return true;
}

float Disk::Area() const
{
	return 2 * PI * Radius * Radius;
}

Interaction Disk::Sample(const Vector2f& u, float* pdf) const
{
	Vector2f pd = ConcentricSampleDisk(u);
	Vector3f pObj(pd.X * Radius, pd.Y * Radius, 0);
	Interaction it;
	it.n = Normalize((*LocalToWorld).Normal(Vector3f(0, 0, 1)));
	it.p = (*LocalToWorld)(pObj);
	*pdf = 1 / Area();
	return it;
}

Triangle::Triangle(class MeshObject* InMesh, int triNumber) : mesh(InMesh)
{
	v = &mesh->Indices[3 * triNumber];
}

Bounds3f Triangle::ObjectBound() const
{
	const Vector3f& p0 = mesh->p[v[0]];
	const Vector3f& p1 = mesh->p[v[1]];
	const Vector3f& p2 = mesh->p[v[2]];
	return Union(Bounds3f(p0, p1), p2);
}

Bounds3f Triangle::WorldBound() const
{
	const Vector3f& p0 = mesh->p[v[0]];
	const Vector3f& p1 = mesh->p[v[1]];
	const Vector3f& p2 = mesh->p[v[2]];
	return Union(Bounds3f((*WorldToLocal)(p0), (*WorldToLocal)(p1)), (*WorldToLocal)(p2));
}

bool Triangle::Intersect(const Ray& ray, float* tHit, SurfaceInteraction* isect) const
{
	Ray LocalRay = (*WorldToLocal)(ray);
	float a, b, c, d;
	const Vector3f& p0 = mesh->p[v[0]];
	const Vector3f& p1 = mesh->p[v[1]];
	const Vector3f& p2 = mesh->p[v[2]];
	if (!Math::Plane(p0, p1, p2, &a, &b, &c, &d)) return false;

	float t0 = -(a * LocalRay.o.X + b * LocalRay.o.Y + c * LocalRay.o.Z + d) / (a * LocalRay.d.X + b * LocalRay.d.Y + c * LocalRay.d.Z);
	if (t0 < 0 || t0 > LocalRay.tMax) return false;

	Vector3f pHit;
	pHit = LocalRay(t0);

	if (!Math::IsInsideTriangle(p0, p1, p2, pHit)) return false;

	*tHit = t0;

	Vector3f dpdu, dpdv;
	Vector2f uv[3];
	GetUVs(uv);
	Vector2f duv02 = uv[0] - uv[2], duv12 = uv[1] - uv[2];
	Vector3f dp02 = p0 - p2, dp12 = p1 - p2;
	float determinant = duv02[0] * duv12[1] - duv02[1] * duv12[0];
	bool degenerateUV = std::abs(determinant) < 1e-8;
	if (!degenerateUV) {
		float invdet = 1 / determinant;
		dpdu = (duv12[1] * dp02 - duv02[1] * dp12) * invdet;
		dpdv = (-duv12[0] * dp02 + duv02[0] * dp12) * invdet;
	}

	float w0 = Cross((p1 - pHit), (p2 - pHit)).Length() / Cross((p1 - p0), (p2 - p0)).Length();
	float w1 = Cross((p2 - pHit), (p0 - pHit)).Length() / Cross((p2 - p1), (p0 - p1)).Length();
	float w2 = 1.f - w0 - w1;
	
	Vector2f uvHit = w0 * uv[0] + w1 * uv[1] + w2 * uv[2];

	*isect = (*LocalToWorld)(SurfaceInteraction(pHit, uvHit, -LocalRay.d, dpdu, dpdv, Vector3f(0, 0, 0), Vector3f(0, 0, 0),this));

	return true;
}

bool Triangle::IntersectP(const Ray& ray) const
{
	Ray LocalRay = (*WorldToLocal)(ray);
	float a, b, c, d;
	const Vector3f& p0 = mesh->p[v[0]];
	const Vector3f& p1 = mesh->p[v[1]];
	const Vector3f& p2 = mesh->p[v[2]];
	if (!Math::Plane(p0, p1, p2, &a, &b, &c, &d)) return false;

	float t0 = -(a * LocalRay.o.X + b * LocalRay.o.Y + c * LocalRay.o.Z + d) / (a * LocalRay.d.X + b * LocalRay.d.Y + c * LocalRay.d.Z);
	if (t0 < 0 || t0 > LocalRay.tMax) return false;

	Vector3f pHit;
	pHit = LocalRay(t0);

	if (!Math::IsInsideTriangle(p0, p1, p2, pHit)) return false;

	return true;
}

float Triangle::Area() const
{
	const Vector3f& p0 = mesh->p[v[0]];
	const Vector3f& p1 = mesh->p[v[1]];
	const Vector3f& p2 = mesh->p[v[2]];
	return 0.5f * Cross(p1 - p0, p2 - p0).Length();
}

Interaction Triangle::Sample(const Vector2f& u, float* pdf) const
{
	Vector2f b = UniformSampleTriangle(u);
	const Vector3f& p0 = mesh->p[v[0]];
	const Vector3f& p1 = mesh->p[v[1]];
	const Vector3f& p2 = mesh->p[v[2]];

	Interaction it;
	it.p = b[0] * p0 + b[1] * p1 + (1 - b[0] - b[1]) * p2;
	it.n = Normalize(Cross(p1-p0,p2-p0));
	if (mesh->n)
	{
		Vector3f ns(b[0] * mesh->n[v[0]] + b[1] * mesh->n[v[1]] + (1 - b[0] - b[1]) * mesh->n[v[2]]);
		it.n = Faceforward(it.n, ns);
	}

	*pdf = 1 / Area();
	return it;
}

void Triangle::GetUVs(Vector2f uv[3]) const
{
	if (mesh->uv)
	{
		uv[0] = mesh->uv[v[0]];
		uv[1] = mesh->uv[v[1]];
		uv[2] = mesh->uv[v[2]];
	}
}

GeometryObject::GeometryObject(const Transform& InLocalToToWorld, const std::shared_ptr<Material>& Inmaterial,const std::shared_ptr<Shape>& Inshape)
	: SceneObject(InLocalToToWorld)
	, material(Inmaterial)
	, shape(Inshape)
{
	shape->SetTransform(&LocalToWorld, &WorldToLocal);
}

Bounds3f GeometryObject::WorldBound() const
{
	return shape->WorldBound();
}

bool GeometryObject::Intersect(const Ray& ray, SurfaceInteraction* isect) const
{
	float tHit;
	if (!shape->Intersect(ray, &tHit, isect)) return false;
	ray.tMax = tHit;
	isect->object = this;
	return true;
}

bool GeometryObject::IntersectP(const Ray& ray) const
{
	return shape->IntersectP(ray);
}

void GeometryObject::ComputeScatteringFunctions(SurfaceInteraction* isect, MemoryArena& arena) const
{
	material->ComputeScatteringFunctions(isect, arena);
}

MeshObject::MeshObject(const Transform& InLocalToToWorld, const std::shared_ptr<Material>& Inmaterial,
	int InnTriangles, int InnVertices, Vector3f* Inp, Vector3f* Inn, Vector2f* Inuv, std::vector<int> InIndices)
	: SceneObject(InLocalToToWorld)
	, material(Inmaterial)
	, nTriangles(InnTriangles)
	, nVertices(InnVertices)
	, p(Inp)
	, n(Inn)
	, uv(Inuv)
	, Indices(InIndices)
{
	BuildTriangle();
}

Bounds3f MeshObject::WorldBound() const
{
	return LocalToWorld(LocalBounds);
}

bool MeshObject::Intersect(const Ray& ray, SurfaceInteraction* isect) const
{
	float tHit;
	for (const auto& t : Triangles)
	{
		if (t->Intersect(ray, &tHit, isect))
		{
			ray.tMax = tHit;
			isect->object = this;
			return true;
		}
	}
	return false;
}

bool MeshObject::IntersectP(const Ray& ray) const
{
	for (const auto& t : Triangles)
	{
		if (t->IntersectP(ray))
		{
			return true;
		}
	}
	return false;
}

void MeshObject::ComputeScatteringFunctions(SurfaceInteraction* isect, MemoryArena& arena) const
{
	material->ComputeScatteringFunctions(isect, arena);
}

void MeshObject::BuildTriangle()
{
	for (int i = 0; i < nTriangles; ++i)
	{
		std::shared_ptr<Triangle> t = std::make_shared<Triangle>(this, i);
		t->SetTransform(&LocalToWorld, &WorldToLocal);
		Triangles.push_back(t);
	}
	for (int i = 0; i < nVertices; ++i)
	{
		LocalBounds = Union(LocalBounds, p[i]);
	}
}

Interaction Shape::Sample(const Interaction& ref, const Vector2f& u, float* pdf) const
{
	Interaction intr = Sample(u, pdf);
	Vector3f wi = intr.p - ref.p;
	if (wi.LengthSquared() == 0)
	{
		*pdf = 0;
	}
	else
	{
		wi = Normalize(wi);
		*pdf *= DistanceSquared(ref.p, intr.p) / AbsDot(intr.n, wi);
		if (std::isinf(*pdf)) *pdf = 0.f;
	}
	return intr;
}

float Shape::Pdf(const Interaction& ref, const Vector3f& wi) const
{
	Ray ray = ref.SpawnRay(wi);
	float tHit;
	SurfaceInteraction isectLight;

	if (!Intersect(ray, &tHit, &isectLight)) return 0;

	float pdf = DistanceSquared(ref.p, isectLight.p) / (AbsDot(isectLight.n, -wi) * Area());
	if (std::isinf(pdf)) pdf = 0.f;
	return pdf;
}
