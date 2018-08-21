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

	Object(float reflectionFactor, float refractionFactor) : reflectionFactor(reflectionFactor), refractionFactor(refractionFactor) 
	{
		;
	}

	virtual bool getIntersection(const Point3 &emitPoint, const Vec3 &rayVec, Intersection &IntersectionPoint) = 0;
	virtual bool getIntersection(const Point3 &emitPoint, const Point3 &endPoint, Intersection &IntersectionPoint) = 0;

	virtual float calcReflectionRay(const Point3 &emitPoint, const Vec3 &rayVec, Vec3 &reflectionRay) = 0;
	virtual float calcRefractionRay(const Point3 &emitPoint, const Vec3 &rayVec, Vec3 &refractionRay) = 0;

	float getReflectionFactor()
	{
		return reflectionFactor;
	}

	float getRefractionFactor()
	{
		return refractionFactor;
	}

private:
	float reflectionFactor;
	float refractionFactor; 
};



class sphere : public Object 
{
public:

	sphere(Point3 center, Color color, float radius, float reflectionFactor, float refractionFactor) :
		Object(reflectionFactor, refractionFactor), 
		center(center),
		color(color), 
		radius(radius)
	{
		radiusSquare = radius * radius;
	}


	bool getIntersection(const Point3 &emitPoint, const Vec3 &rayVec, Intersection &IntersectionPoint)
	{

		Vec3 sphereDist = center - emitPoint;
		Vec3 rayDirect = rayVec.normalize();

		float sphereDistProjectOnRay = rayDirect * sphereDist;

		if (sphereDistProjectOnRay < 0) return false;

		float sphereDistSquare = sphereDist * sphereDist;
		float sphereRayDistSquare = sphereDistSquare - sphereDistProjectOnRay * sphereDistProjectOnRay;

		if (sphereRayDistSquare >= radiusSquare) return false;

		float intersectionDist = sphereDistProjectOnRay - sqrt(radiusSquare - sphereRayDistSquare);

		IntersectionPoint.obj = this;
		IntersectionPoint.entryPoint = emitPoint + rayDirect * intersectionDist;

		return true;
	}


	bool getIntersection(const Point3 &emitPoint, const Point3 &endPoint, Intersection &IntersectionPoint)
	{
		// if a line section intersection with sphere

		if (!getIntersection(emitPoint, endPoint - emitPoint, IntersectionPoint)) return false;

		if ((IntersectionPoint.entryPoint - endPoint) * (IntersectionPoint.entryPoint - emitPoint) > 0) return false;

		return true;
	}


	float calcReflectionRay(const Point3 &emitPoint, const Vec3 &rayVec, Vec3 &reflectionRay)
	{
		return 0.0f;
	}


	float calcRefractionRay(const Point3 &emitPoint, const Vec3 &rayVec, Vec3 &refractionRay)
	{
		return 0.0f;
	}

private:
	Point3 center;
	Color color;
	float radius;
	float radiusSquare;
};