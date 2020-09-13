#include "shape.h"
#include "lowdiscrepancy.h"

Shape::Shape(const Transform* ObjectToWorld, const Transform*WorldToObject, bool reverseOrientation)
	:ObjectToWorld(ObjectToWorld),
	WorldToObject(WorldToObject),
	reverseOrientation(reverseOrientation),
	transformSwapsHandedness(ObjectToWorld->SwapHandedness())
{
	
}

Shape::~Shape()
{
}

Bounds3f Shape::WorldBound() const
{
	return (*ObjectToWorld)(ObjectBound());
}

Interaction Shape::Sample(const Interaction& ref, const Point2f& u, Float* pdf) const
{
	return Sample(u,pdf);
}

Float Shape::Pdf(const Interaction& ref, const Vector3f & wi) const
{
	//������ཻ��pdfΪ0
	Ray ray = ref.SpawnRay(ray.d);
	Float tHit;
	SurfaceInteraction isectLight;
	if (!Intersect(ray, &tHit, &isectLight, false)) return 0;
	//�����������pdfת���ɻ�������ǵ�pdf
	//d��i/dA = cos��o/r2
	Float pdf = DistanceSquared(ref.p, isectLight.p) / (AbsDot(isectLight.n, -wi) * Area());
	return pdf;
}

Float Shape::SolidAngle(const Point3f& p, int nSamples /*= 512*/) const
{
	return Float();
}
