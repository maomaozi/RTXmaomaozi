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

	Color &operator+(const Color &rhs) const
	{
		return Color(min(r + rhs.r, 255.0f), min(g + rhs.g, 255.0f), min(b + rhs.b, 255.0f));
	}

	UINT32 getColor() const
	{
		return ((UINT32)r << 16) & ((UINT32)g << 8) & (UINT32)b;
	}

	void operator+=(const Color &rhs)
	{
		r = min(r + rhs.r, 255.0f);
		g = min(g + rhs.g, 255.0f);
		b = min(b + rhs.b, 255.0f);
	}

	Color &operator*(float factor) const
	{
		return Color(min(r * factor, 255.0f), min(g  * factor, 255.0f), min(b  * factor, 255.0f));
	}

	float r;
	float g;
	float b;
};


struct light 
{
	light(point3 position, int strength) : position(position), strength(strength)
	{
		;
	}

	point3 position;
	int strength;
};