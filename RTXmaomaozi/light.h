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
		return Color(r + rhs.r, g + rhs.g, b + rhs.b);
	}

	Color operator-(const Color &rhs) const
	{
		return Color(r - rhs.r, g - rhs.g, b - rhs.b);
	}

	Color operator*(const Color &rhs) const
	{
		return Color(r * rhs.r, g * rhs.g, b * rhs.b);
	}

	Color operator/(const Color &rhs) const
	{
		return Color(r / rhs.r, g / rhs.g, b / rhs.b);
	}


	Color operator+(float rhs) const
	{
		return Color(r + rhs, g + rhs, b + rhs);
	}


	Color operator-(float rhs) const
	{
		return Color(r - rhs, g - rhs, b - rhs);
	}


	Color operator*(float rhs) const
	{
		return Color (r * rhs, g * rhs, b * rhs);
	}


	Color operator/(float rhs) const
	{
		return Color((r / rhs), (g / rhs), (b / rhs));
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
	Light(Point3 position, Color color, float strength) : position(position), color(color), strength(strength)
	{
		;
	}

public:
	virtual Color getLightStrength(const Vec3 &lightDirection, float distance, const Vec3 &objNorm) const = 0;

	Point3 position;
	float strength;
	Color color;
};


class DotLight : public Light
{
public:
	using Light::Light;

public:
	Color getLightStrength(const Vec3 &lightDirection, float distance, const Vec3 &objNorm) const
	{
		return color * strength;
	}

};


class SpotLight : public Light
{
public:
	SpotLight(Point3 position, Color color, float strength, const Vec3 &direct, float decayRatio) :
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

		return color * strength * decay;
	}

private:
	Vec3 direct;
	float decayRatio;

};


struct Object;

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