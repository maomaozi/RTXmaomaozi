#pragma once

#include "light.h"

#define NO_INTERSECTION -1.0f


class Object;


struct Intersection
{
	Intersection(Point3 entryPoint, Object *obj) :
		entryPoint(entryPoint), 
		obj(obj)
	{
		;
	}

	Intersection() :
		entryPoint(0, 0, 0),
		obj(nullptr)
	{
		;
	}

	Point3 entryPoint;
	Object *obj;
};



class Object
{

public:

	Object(const Color &reflectionRatio, float refractionRatio, float refractionEta, float diffuseFactor) :
		reflectionRatio(reflectionRatio),
		refractionRatio(refractionRatio),
		refractionEta(refractionEta),
		diffuseFactor(diffuseFactor)
	{
		refractionEtaEntry = 1.0f / refractionEta;
	}

	virtual float getIntersection(const Point3 &emitPoint, const Vec3 &rayVec, bool isInMedium) const = 0;

	virtual void calcReflectionRay(const Point3 &reflectionPoint, const Vec3 &rayVec, Vec3 &reflectionRay) const = 0;
	virtual bool calcRefractionRay(const Point3 &refractionPoint, const Vec3 &rayVec, Vec3 &refractionRay, bool isEntry) const  = 0;

	virtual void getNormVecAt(const Point3 &point, Vec3 &norm) const = 0;

	const Color &getReflectionRatio() const
	{
		return reflectionRatio;
	}

	float getRefractionRatio() const
	{
		return refractionRatio;
	}

	float getRefractionEta() const
	{
		return refractionEta;
	}


protected:	
	Color reflectionRatio;
	float refractionRatio;
	float refractionEta;
	float refractionEtaEntry;
	float diffuseFactor;
};



class Sphere : public Object 
{
public:

	Sphere(const Point3 &center, float radius, const Color &reflectionRatio, float refractionRatio, float refractionEta, float diffuseFactor) :
		Object(reflectionRatio, refractionRatio, refractionEta, diffuseFactor),
		center(center),
		radius(radius)
	{
		radiusSquare = radius * radius;
	}


	void getNormVecAt(const Point3 &point, Vec3 &norm) const
	{
		norm = (point - center).normalize();
	}

	float getIntersection(const Point3 &emitPoint, const Vec3 &rayVec, bool isInMedium) const
	{

		Vec3 sphereDist = center - emitPoint;
		//Vec3 rayDirect = rayVec.normalize();

		float sphereDistProjectOnRay = rayVec * sphereDist;

		if (sphereDistProjectOnRay < 0) return NO_INTERSECTION;

		float sphereDistSquare = sphereDist * sphereDist;
		float sphereRayDistSquare = sphereDistSquare - sphereDistProjectOnRay * sphereDistProjectOnRay;

		if (sphereRayDistSquare >= radiusSquare) return NO_INTERSECTION;

		float intersectionDist = sphereDistProjectOnRay + (isInMedium ? 1.0f : -1.0f) * sqrt(radiusSquare - sphereRayDistSquare);

		return intersectionDist;
	}


	void calcReflectionRay(const Point3 &reflectionPoint, const Vec3 &rayVec, Vec3 &reflectionRay) const
	{
		Vec3 normVec = (reflectionPoint - center).normalize();
		//Vec3 rayVecNorm = rayVec.normalize();

		// R = I - 2 * (I * N) * N
		reflectionRay = rayVec - normVec * 2 * (rayVec * normVec);
	}


	bool calcRefractionRay(const Point3 &refractionPoint, const Vec3 &rayVec, Vec3 &refractionRay, bool isInMedium) const
	{
		// copy from Nvidia

		/*
		
		float3 refract( float3 i, float3 n, float eta )
		{
			float cosi = dot(-i, n);
			float cost2 = 1.0f - eta * eta * (1.0f - cosi * cosi);
			float3 t = eta * i + (eta * cosi - sqrt(abs(cost2))) * n;
			return t * (float3)(cost2 > 0);
		}
		*/
		float eta = isInMedium ? refractionEta : refractionEtaEntry;

		Vec3 normVec = (refractionPoint - center).normalize() * (isInMedium ? -1 : 1);
		//Vec3 rayVecNorm = rayVec.normalize();

		float cosi = -rayVec * normVec;
		float cost2 = 1.0f - eta * eta * (1.0f - cosi * cosi);
		refractionRay = rayVec * eta + normVec * (eta * cosi - sqrt(fabs(cost2)));

		return cost2 <= 0;
	}

private:
	Point3 center;
	float radius;
	float radiusSquare;
};


class Plane : public Object
{
public:

	Plane(const Vec3 &normVec, const Point3 &pointOnPlane, const Color &reflectionRatio, float diffuseFactor) :
		Object(reflectionRatio, 0, 1.0f, diffuseFactor),
		normVec(normVec.normalize()),
		pointOnPlane(pointOnPlane)
	{
		;
	}


	void getNormVecAt(const Point3 &point, Vec3 &norm) const
	{
		norm = normVec;
	}

	float getIntersection(const Point3 &emitPoint, const Vec3 &rayVec, bool isInMedium) const
	{
		float dot1 = normVec * rayVec;

		if (fabs(dot1) < 0.001) return NO_INTERSECTION;

		float t = normVec * (pointOnPlane - (emitPoint + rayVec * EPSILON)) / (normVec * rayVec);

		if (t < 0) return NO_INTERSECTION;

		return t;
		
	}


	void calcReflectionRay(const Point3 &reflectionPoint, const Vec3 &rayVec, Vec3 &reflectionRay) const
	{
		// R = I - 2 * (I * N) * N
		reflectionRay = rayVec - normVec * 2 * (rayVec * normVec);
	}


	bool calcRefractionRay(const Point3 &refractionPoint, const Vec3 &rayVec, Vec3 &refractionRay, bool isInMedium) const
	{
		return false;
	}

private:
	Vec3 normVec;
	Point3 pointOnPlane;
};