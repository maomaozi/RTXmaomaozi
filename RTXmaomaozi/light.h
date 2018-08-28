#pragma once
#include "vec.h"


struct Color
{
	Color() : r(0), g(0), b(0)
	{
		;
	}

	Color(float r, float g, float b) : r(r), g(g), b(b)
	{
		;
	}

	Color operator+(const Color &rhs) const
	{
		return std::move(Color(r + rhs.r, g + rhs.g, b + rhs.b));
	}

	Color operator-(const Color &rhs) const
	{
		return std::move(Color(r - rhs.r, g - rhs.g, b - rhs.b));
	}

	Color operator*(const Color &rhs) const
	{
		return std::move(Color(r * rhs.r, g * rhs.g, b * rhs.b));
	}

	Color operator/(const Color &rhs) const
	{
		return std::move(Color(r / rhs.r, g / rhs.g, b / rhs.b));
	}


	Color operator+(float rhs) const
	{
		return std::move(Color(r + rhs, g + rhs, b + rhs));
	}


	Color operator-(float rhs) const
	{
		return std::move(Color(r - rhs, g - rhs, b - rhs));
	}


	Color operator*(float rhs) const
	{
		return std::move(Color (r * rhs, g * rhs, b * rhs));
	}


	Color operator/(float rhs) const
	{
		return std::move(Color((r / rhs), (g / rhs), (b / rhs)));
	}


	void operator/=(float rhs)
	{
		r /= rhs;
		g /= rhs;
		b /= rhs;
	}


	void operator/=(const Color &rhs)
	{
		r /= rhs.r;
		g /= rhs.g;
		b /= rhs.b;
	}


	UINT32 getColor() const
	{
		return ((UINT32)max(min(r, 255.0f), 0.0f) << 16) | ((UINT32)max(min(g, 255.0f), 0.0f) << 8) | (UINT32)max(min(b, 255.0f), 0.0f);
	}


	void operator+=(const Color &rhs)
	{
		r = r + rhs.r;
		g = g + rhs.g;
		b = b + rhs.b;
	}


	void operator*=(float rhs)
	{
		r = r * rhs;
		g = g * rhs;
		b = b * rhs;
	}


	void operator*=(const Color &rhs)
	{
		r *= rhs.r;
		g *= rhs.g;
		b *= rhs.b;
	}


	float r;
	float g;
	float b;
};


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

	virtual float sampleRayVec(const Point3 emitPoint, Vec3 &newRayvec) = 0;

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

	virtual float sampleRayVec(const Point3 emitPoint, Vec3 &newRayVec)
	{
		Vec3 v1 = position - emitPoint;
	
		float lightDistance = v1.length();

		float targetCosAngle = lightDistance / sqrtf(lightDistance * lightDistance + radiusSquare * ((u(e) + 1.0f) / 2));

		Vec3 p(u(e), u(e), u(e));

		p -= v1 * ((p * v1) / (v1 * v1));

		p /= p.length();
		p *= lightDistance;

		newRayVec = v1 * targetCosAngle;
		p *= sqrtf(1.0f - targetCosAngle * targetCosAngle);
		newRayVec += p;

		return lightDistance;
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
	Ray() : emitPoint(0,0,0), rayVec(0,0,0)
	{
		;
	}

	Ray(const Point3 &emitPoint, const Vec3 &rayVec, Object* castObj) : 
		emitPoint(emitPoint), rayVec(rayVec), castObj(castObj)
	{
		;
	}

	Point3 emitPoint;
	Vec3 rayVec;
	Object *castObj;
};