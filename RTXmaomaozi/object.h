#pragma once

#include "light.h"

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

	Object(const Color &reflectionRatio, float refractionRatio, float refractionEta) :
		reflectionRatio(reflectionRatio),
		refractionRatio(refractionRatio),
		refractionEta(refractionEta)
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
};



class Sphere : public Object 
{
public:

	Sphere(Point3 center, float radius, const Color &reflectionRatio, float refractionRatio, float refractionEta) :
		Object(reflectionRatio, refractionRatio, refractionEta),
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

		if (sphereDistProjectOnRay < 0) return -1.0f;

		float sphereDistSquare = sphereDist * sphereDist;
		float sphereRayDistSquare = sphereDistSquare - sphereDistProjectOnRay * sphereDistProjectOnRay;

		if (sphereRayDistSquare >= radiusSquare) return -1.0f;

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