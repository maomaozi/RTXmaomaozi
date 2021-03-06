#pragma once

#include "light.h"
#include "aabb.h"

#define NO_INTERSECTION -1.0f


class Object;


struct Intersection
{
	Intersection(Point3 intersectionPoint, Object *obj) :
		intersectionPoint(intersectionPoint), 
		obj(obj)
	{
		;
	}

	Intersection() :
		intersectionPoint(0, 0, 0),
		obj(nullptr)
	{
		;
	}

	Point3 intersectionPoint;
	Object *obj;
};



class Object
{

public:

	Object(const Color &reflectionRatio, const Color & refractionRatio, float refractionEta, float diffuseFactor) :
		reflectionRatio(reflectionRatio),
		refractionRatio(refractionRatio),
		refractionEta(refractionEta),
		diffuseFactor(diffuseFactor)
	{
		refractionEtaEntry = 1.0f / refractionEta;
		totalRefractionRatio = reflectionRatio + refractionRatio;
	}

	virtual float getIntersection(const Point3 &emitPoint, const Vec3 &rayVec, bool isInMedium) const = 0;

	virtual void calcAABB(AABB &result) const = 0;

	virtual void calcReflectionRay(const Point3 &reflectionPoint, const Vec3 &rayVec, Vec3 &reflectionRay) const = 0;
	virtual bool calcRefractionRay(const Point3 &refractionPoint, const Vec3 &rayVec, bool rayInMedium, Vec3 &refractionRay) const  = 0;

	virtual void getNormVecAt(const Point3 &point, Vec3 &norm) const = 0;

	virtual const Color &getReflectionRatio(const Point3 &point) const
	{
		return reflectionRatio;
	}

	virtual const Color &getRefractionRatio(const Point3 &point) const
	{
		return refractionRatio;
	}

	virtual const Color &getTotalReflectionRatio(const Point3 &point) const 
	{
		return totalRefractionRatio;
	}

	virtual bool getIsLighting() 
	{
		return false;
	}

	float getRefractionEta() const
	{
		return refractionEta;
	}

	float getDiffuseFactor() const
	{
		return diffuseFactor;
	}


protected:
	Color reflectionRatio;
	Color refractionRatio;
	Color totalRefractionRatio;
	float refractionEta;
	float refractionEtaEntry;
	float diffuseFactor;
};



class Sphere : public Object 
{
public:

	Sphere(const Point3 &center, float radius, const Color &reflectionRatio, const Color &refractionRatio, float refractionEta, float diffuseFactor) :
		Object(reflectionRatio, refractionRatio, refractionEta, diffuseFactor),
		center(center),
		radius(radius)
	{
		radiusSquare = radius * radius;
	}


	void getNormVecAt(const Point3 &point, Vec3 &norm) const
	{
		norm = point - center;
		norm.normalize();
	}

	float getIntersection(const Point3 &emitPoint, const Vec3 &rayVec, bool isInMedium) const
	{

		Vec3 sphereDist = center - emitPoint;

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
		Vec3 normVec = reflectionPoint - center;
		normVec.normalize();

		// R = I - 2 * (I * N) * N
		reflectionRay = rayVec - normVec * 2 * (rayVec * normVec);
	}


	void calcAABB(AABB &result) const {
		Vec3 v_r(radius, radius, radius);
		result.set_top_left(center - v_r);
		result.set_down_right(center + v_r);
	}

	bool calcRefractionRay(const Point3 &refractionPoint, const Vec3 &rayVec, bool isInMedium, Vec3 &refractionRay) const
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

		Vec3 normVec = (refractionPoint - center);
		normVec.normalize();
		normVec *= (isInMedium ? -1.0f : 1.0f);


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


class LightingSphere : public Sphere 
{
public:
	using Sphere::Sphere;

	virtual bool getIsLighting()
	{
		return true;
	}
};


class Plane : public Object
{
public:

	Plane(const Vec3 &normVec, const Point3 &pointOnPlane, const Color &reflectionRatio, float diffuseFactor) :
		Object(reflectionRatio, Color(0.0f, 0.0f, 0.0f), 1.0f, diffuseFactor),
		normVec(normVec),
		pointOnPlane(pointOnPlane),
		base1(0, 0, 0), base2(0, 0, 0)
	{
		this->normVec.normalize();

		int idx;

		for (idx = 0; idx < 3; ++idx)
		{
			if (normVec[idx]) break;
		}

		for (int i = 0; i  < 3; ++i)
		{
			if (i != idx) base1[i] = 1.0f;
		}


		base1[idx] = -(normVec * base1) / normVec[idx];

		base2 = normVec.xmul(base1);

		base1.normalize();
		base2.normalize();
	}


	void getNormVecAt(const Point3 &point, Vec3 &norm) const
	{
		norm = normVec;
	}

	void calcAABB(AABB &result) const {
		// cannot calc for plan
	}


	float getIntersection(const Point3 &emitPoint, const Vec3 &rayVec, bool isInMedium) const
	{
		float dot1 = normVec * rayVec;

		if (fabs(dot1) < 0.001) return NO_INTERSECTION;
		float t = normVec * (pointOnPlane - (emitPoint + rayVec * EPSILON * 5)) / dot1;

		if (t < 0) return NO_INTERSECTION;

		return t;
		
	}

	const Color &getReflectionRatio(const Point3 &point) const
	{
		return reflectionRatio;
	}


	void calcReflectionRay(const Point3 &reflectionPoint, const Vec3 &rayVec, Vec3 &reflectionRay) const
	{
		// R = I - 2 * (I * N) * N
		reflectionRay = rayVec - normVec * 2 * (rayVec * normVec);
	}


	bool calcRefractionRay(const Point3 &refractionPoint, const Vec3 &rayVec, bool isInMedium, Vec3 &refractionRay) const
	{
		return false;
	}

protected:
	Vec3 normVec;
	Point3 pointOnPlane;

	Vec3 base1;
	Vec3 base2;

};


class CheesePlane : public Plane
{
public:

	using Plane::Plane;

	virtual const Color &getReflectionRatio(const Point3 &point) const
	{
		Vec3 partialCoords = point - pointOnPlane;

		int axis1 = roundf(fabsf(partialCoords *  base1) / 300);
		int axis2 = roundf(fabsf(partialCoords *  base2) / 300);


		if ((axis1 + axis2) % 2 == 0)
			return blackColor;
		else
			return whiteColor;

	}

private:
	Color blackColor = Color(0.2, 0.2, 0.2);
	Color whiteColor = Color(0.9, 0.9, 0.9);
};


class Triangle : public Object
{
public:

	Triangle(const Point3 &pointA, const Point3 &pointB, const Point3 &pointC, const Color &reflectionRatio, const Color &refractionRatio, float refractionEta, float diffuseFactor) :
		Object(reflectionRatio, refractionRatio, refractionEta, diffuseFactor), 
		pointA(pointA), pointB(pointB), pointC(pointC), 
		pointAB(pointB - pointA), pointAC(pointC - pointA),
		normVec(pointAB.xmul(pointAC))
	{
		normVec.normalize();
	}


	void getNormVecAt(const Point3 &point, Vec3 &norm) const
	{
		norm = normVec;
	}

	void calcAABB(AABB &result) const {

		result.set_top_left(
		Point3(
			min(pointA.x, min(pointB.x, pointC.x)),
			min(pointA.y, min(pointB.y, pointC.y)),
			min(pointA.z, min(pointB.z, pointC.z))

		));

		result.set_down_right(
		Point3(
			max(pointA.x, max(pointB.x, pointC.x)) + 1,
			max(pointA.y, max(pointB.y, pointC.y)) + 1,
			max(pointA.z, max(pointB.z, pointC.z)) + 1
		));
	}


	float getIntersection(const Point3 &emitPoint, const Vec3 &rayVec, bool isInMedium) const
	{
		/*
		Moller-Trumbore Algorithm
		*/

		Vec3 P = rayVec.xmul(pointAC);

		float determinant = pointAB * P;

		Vec3 T(determinant > 0 ? emitPoint - pointA : pointA - emitPoint);

		determinant = fabs(determinant);

		if (determinant < EPSILON) return NO_INTERSECTION;

		float u = T * P;

		if (u < 0.0f || u > determinant) return NO_INTERSECTION;

		Vec3 Q = T.xmul(pointAB);

		float v = rayVec * Q;
		if (v < 0.0f || u + v > determinant) return NO_INTERSECTION;

		float t = pointAC * Q / determinant;

		if (t < 10 * EPSILON) return NO_INTERSECTION;

		return t;
	}

	const Color &getReflectionRatio(const Point3 &point) const
	{
		return reflectionRatio;
	}


	void calcReflectionRay(const Point3 &reflectionPoint, const Vec3 &rayVec, Vec3 &reflectionRay) const
	{
		// R = I - 2 * (I * N) * N
		reflectionRay = rayVec - normVec * 2 * (rayVec * normVec);
	}


	bool calcRefractionRay(const Point3 &refractionPoint, const Vec3 &rayVec, bool isInMedium, Vec3 &refractionRay) const
	{
		float eta = isInMedium ? refractionEta : refractionEtaEntry;


		float cosi = -rayVec * normVec;
		float cost2 = 1.0f - eta * eta * (1.0f - cosi * cosi);
		refractionRay = rayVec * eta + normVec * (eta * fabsf(cosi) - sqrt(fabs(cost2))) * (cosi < 0.0f ? -1.0f : 1.0f);

		return cost2 <= 0;
	}

protected:
	Point3 pointA;
	Point3 pointB;
	Point3 pointC;

	Vec3 pointAB;
	Vec3 pointAC;

	Vec3 normVec;
};