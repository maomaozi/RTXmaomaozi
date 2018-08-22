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

	virtual bool getIntersection(const Point3 &emitPoint, const Vec3 &rayVec, Intersection &Intersection, bool isInMedium) = 0;
	virtual bool getIntersection(const Point3 &emitPoint, const Point3 &endPoint, Intersection &Intersection, bool isInMedium) = 0;

	virtual void calcReflectionRay(const Point3 &reflectionPoint, const Vec3 &rayVec, Vec3 &reflectionRay) = 0;
	virtual bool calcRefractionRay(const Point3 &refractionPoint, const Vec3 &rayVec, Vec3 &refractionRay, bool isEntry) = 0;

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


	bool getIntersection(const Point3 &emitPoint, const Vec3 &rayVec, Intersection &Intersection, bool isInMedium)
	{

		Vec3 sphereDist = center - emitPoint;
		//Vec3 rayDirect = rayVec.normalize();

		float sphereDistProjectOnRay = rayVec * sphereDist;

		if (sphereDistProjectOnRay < 0) return false;

		float sphereDistSquare = sphereDist * sphereDist;
		float sphereRayDistSquare = sphereDistSquare - sphereDistProjectOnRay * sphereDistProjectOnRay;

		if (sphereRayDistSquare >= radiusSquare) return false;

		float intersectionDist = sphereDistProjectOnRay + (isInMedium ? 1 : -1) * sqrt(radiusSquare - sphereRayDistSquare);

		Intersection.obj = this;
		Intersection.entryPoint = emitPoint + rayVec * intersectionDist;

		return true;
	}


	bool getIntersection(const Point3 &emitPoint, const Point3 &endPoint, Intersection &Intersection, bool isInMedium)
	{
		// if a line section intersection with Sphere

		if (!getIntersection(emitPoint, (endPoint - emitPoint).normalize(), Intersection, isInMedium)) return false;

		//if ((Intersection.entryPoint - endPoint) * (Intersection.entryPoint - emitPoint) > 0) return false;

		return true;
	}


	void calcReflectionRay(const Point3 &reflectionPoint, const Vec3 &rayVec, Vec3 &reflectionRay)
	{
		Vec3 normVec = (reflectionPoint - center).normalize();
		//Vec3 rayVecNorm = rayVec.normalize();

		// R = I - 2 * (I * N) * N
		reflectionRay = rayVec - normVec * 2 * (rayVec * normVec);
	}


	bool calcRefractionRay(const Point3 &refractionPoint, const Vec3 &rayVec, Vec3 &refractionRay, bool isInMedium)
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