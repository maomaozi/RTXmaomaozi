#pragma once
#include "vec.h"
#include "color.h"

#define NO_INTERSECTION -1.0f


class Light 
{
public:
	Light(const Point3 &position, const Color &color, float strength) : position(position), color(color), strength(strength)
	{
		;
	}

public:
	virtual Color getLightStrength(const Vec3 &lightDirection, float distance, const Vec3 &objNorm) const = 0;

	virtual bool hasVolume() const 
	{
		return false;
	}

	Point3 position;
	float strength;
	Color color;
};


class VolumnLight : public Light
{
public:
	VolumnLight(const Point3 &position, const Color &color, float strength) :
		Light(position, color, strength),
		u(-1, 1)
	{
		;
	}

public:
	virtual bool hasVolume() const
	{
		return true;
	}

	virtual float getIntersection(const Point3 &emitPoint, const Vec3 &rayVec) const = 0;

	virtual float sampleRayVec(const Point3  &emitPoint, Vec3 &newRayvec) = 0;

	virtual float getSampleRatio(const Point3  &emitPoint) = 0;

protected:
	std::default_random_engine e;
	std::uniform_real_distribution<double> u;
};


class DotLight : public Light
{
public:
	using Light::Light;

public:
	Color getLightStrength(const Vec3 &lightDirection, float distance, const Vec3 &objNorm) const
	{
		return std::move(color * strength);
	}

};


class SpotLight : public Light
{
public:
	SpotLight(const Point3 &position, const Color &color, float strength, const Vec3 &direct, float decayRatio):
		Light(position, color, strength),
		direct(direct),
		decayRatio(decayRatio)
	{
		this->direct.normalize();
	}

public:
	Color getLightStrength(const Vec3 &lightDirection, float distance, const Vec3 &objNorm) const
	{
		float offsetCosAngle = -lightDirection * direct;

		float decay = powf(offsetCosAngle, 1 / decayRatio);

		if (decay < 0) decay = 0;

		Color c = color * strength;
		c *= decay;

		return std::move(c);
	}

private:
	Vec3 direct;
	float decayRatio;

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

	virtual float getIntersection(const Point3 &emitPoint, const Vec3 &rayVec) const
	{

		Vec3 sphereDist = position - emitPoint;

		float sphereDistProjectOnRay = rayVec * sphereDist;

		if (sphereDistProjectOnRay < 0) return NO_INTERSECTION;

		float sphereRayDistSquare = sphereDist * sphereDist - sphereDistProjectOnRay * sphereDistProjectOnRay;

		if (sphereRayDistSquare >= radiusSquare) return NO_INTERSECTION;

		return sphereDistProjectOnRay - sqrtf(radiusSquare - sphereRayDistSquare);
	}

	virtual float sampleRayVec(const Point3  &emitPoint, Vec3 &newRayVec)
	{
		Vec3 v1 = position - emitPoint;
	
		float lightDistance = v1.length();

		float targetCosAngle = sqrtf(lightDistance * lightDistance - radiusSquare * ((u(e) + 1.0f) / 2.0f)) / lightDistance;

		Vec3 p(u(e), u(e), u(e));

		p -= v1 * ((p * v1) / (v1 * v1));

		p /= p.length();
		p *= lightDistance;

		newRayVec = v1 * targetCosAngle;
		p *= sqrtf(1.0f - targetCosAngle * targetCosAngle);
		newRayVec += p;

		return lightDistance;
	}

	virtual float getSampleRatio(const Point3 &emitPoint)
	{
		float lightDistance = (position - emitPoint).length();

		float targetAngle = acosf(sqrtf(lightDistance * lightDistance - radiusSquare) / lightDistance);

		return 4.0f * targetAngle * targetAngle / (PI * PI);
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
		float offsetCosAngle = -lightDirection * direct;

		float decay = powf(offsetCosAngle, 1 / decayRatio);

		if (decay < 0) decay = 0;

		Color c = color * strength;
		c *= decay;

		return std::move(c);
	}

private:
	Vec3 direct;
	float decayRatio;
};



class Object;

struct Ray 
{
	Ray() : emitPoint(0,0,0), rayDirect(0,0,0)
	{
		;
	}

	Ray(const Point3 &emitPoint, const Vec3 &rayVec, Object* castObj, bool isInMedium) :
		emitPoint(emitPoint), rayDirect(rayVec), emitObject(emitObject)
	{
		;
	}

	Point3 emitPoint;
	Vec3 rayDirect;
	Object *emitObject;
};