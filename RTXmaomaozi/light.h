#pragma once
#include "vec.h"
#include "color.h"
#include "aabb.h"

#define NO_INTERSECTION -1.0f



class VolumnLight
{
public:
	VolumnLight(const Point3 &position, const Color &color, float strength) : position(position), color(color), strength(strength)
	{
		;
	}

	virtual void calcAABB(AABB &result) const = 0;

public:
	virtual bool hasVolume() const
	{
		return true;
	}

	virtual float getIntersection(const Point3 &emitPoint, const Vec3 &rayVec) const = 0;

	virtual float sampleRayVec(const Point3  &emitPoint, Vec3 &newRayvec, float &ratio) = 0;

	virtual float getSampleRatio(const Point3  &emitPoint) = 0;

	virtual Color getLightStrength(const Vec3 &lightDirection, float distance, const Vec3 &objNorm) const = 0;

	Point3 position;
	float strength;
	Color color;
};



class SphereLight : public VolumnLight
{
public:
	SphereLight(const Point3 &position, const Color &color, float strength, float radius) :
		VolumnLight(position, color, strength),
		radius(radius),
		radiusSquare(radius * radius)
	{
		;
	}

	void calcAABB(AABB &result) const {
		Vec3 v_r(radius, radius, radius);
		result.set_top_left(position - v_r);
		result.set_down_right(position + v_r);
	}

	virtual float getIntersection(const Point3 &emitPoint, const Vec3 &rayVec) const
	{

		Vec3 sphereDist = position - emitPoint;

		float sphereDistProjectOnRay = rayVec * sphereDist;

		if (sphereDistProjectOnRay < 0) return NO_INTERSECTION;

		float sphereRayDistSquare = sphereDist * sphereDist - sphereDistProjectOnRay * sphereDistProjectOnRay;

		if (sphereRayDistSquare >= radiusSquare) return NO_INTERSECTION;

		return sphereDistProjectOnRay - sqrtf(radiusSquare - sphereRayDistSquare);
	}

	virtual float sampleRayVec(const Point3  &emitPoint, Vec3 &newRayVec, float &ratio)
	{
		Vec3 v1 = position - emitPoint;
	
		float lightDistance = v1.length();

		//float targetCosAngle = sqrtf(lightDistance * lightDistance - radiusSquare * ((u(e) + 1.0f) / 2.0f)) / lightDistance;
		float targetCosAngle = sqrtf(lightDistance * lightDistance - radiusSquare * ((float)rand() / (float)RAND_MAX)) / lightDistance;

		float maxCosAngle = sqrtf(lightDistance * lightDistance - radiusSquare) / lightDistance;
		ratio = acosf(maxCosAngle) / (2.0f * PI);

		//Vec3 p(u(e), u(e), u(e));
		Vec3 p
		(
			(float)rand() / (float)RAND_MAX * 2.0f - 1.0f, 
			(float)rand() / (float)RAND_MAX * 2.0f - 1.0f,
			(float)rand() / (float)RAND_MAX * 2.0f - 1.0f
		);

		p -= v1 * ((p * v1) / (v1 * v1));

		p /= p.length();
		p *= lightDistance;

		newRayVec = v1 * targetCosAngle;
		p *= sqrtf(1.0f - targetCosAngle * targetCosAngle);
		newRayVec += p;

		newRayVec.normalize();

		return lightDistance;
	}

	virtual float getSampleRatio(const Point3 &emitPoint)
	{
		float lightDistance = (position - emitPoint).length();

		float targetAngle = acosf(sqrtf(lightDistance * lightDistance - radiusSquare) / lightDistance);

		return targetAngle / PI;
	}

private:
	float radius;
	float radiusSquare;
};



class SphereDotLight : public SphereLight
{
public:
	using SphereLight::SphereLight;


public:
	virtual Color getLightStrength(const Vec3 &lightDirection, float distance, const Vec3 &objNorm) const
	{
		return std::move(color * strength);
	}
};


class SphereSpotLight : public SphereLight
{
public:
	// SpotLight(const Point3 &position, const Color &color, float strength, const Vec3 &direct, float decayRatio)
	SphereSpotLight(const Point3 &position, const Color &color, float strength, const Vec3 &direct, float decayRatio, float radius) :
		SphereLight(position, color, strength, radius),
		direct(direct),
		decayRatio(decayRatio)
	{
		this->direct.normalize();
	}

public:
	Color getLightStrength(const Vec3 &lightDirection, float distance, const Vec3 &objNorm) const
	{
		float offsetCosAngle = (-lightDirection * direct + 1) / 2.0f;

		Color c = color * strength;
		c *= powf(offsetCosAngle, 1 / decayRatio);

		return std::move(c);
	}

private:
	Vec3 direct;
	float decayRatio;
};



class Object;