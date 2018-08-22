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
		return Color(min(r + rhs.r, 255.0f), min(g + rhs.g, 255.0f), min(b + rhs.b, 255.0f));
	}

	Color operator*(const Color &rhs) const
	{
		return Color(min(r * rhs.r, 255.0f), min(g * rhs.g, 255.0f), min(b * rhs.b, 255.0f));
	}

	Color operator-(const Color &rhs) const
	{
		return Color(max(r - rhs.r, 0.0f), max(g - rhs.g, 0.0f), max(b - rhs.b, 0.0f));
	}

	UINT32 getColor() const
	{
		return ((UINT32)r << 16) | ((UINT32)g << 8) | (UINT32)b;
	}

	void operator+=(const Color &rhs)
	{
		r = min(r + rhs.r, 255.0f);
		g = min(g + rhs.g, 255.0f);
		b = min(b + rhs.b, 255.0f);
	}

	void operator*=(float rhs)
	{
		r = min(r * rhs, 255.0f);
		g = min(g * rhs, 255.0f);
		b = min(b * rhs, 255.0f);
	}

	Color operator*(float factor) const
	{
		return Color(min(r * factor, 255.0f), min(g  * factor, 255.0f), min(b  * factor, 255.0f));
	}

	float r;
	float g;
	float b;
};


struct Light 
{
	Light(Point3 position, Color color, float strength) : position(position), color(color), strength(strength)
	{
		;
	}

	Point3 position;
	float strength;
	Color color;
};