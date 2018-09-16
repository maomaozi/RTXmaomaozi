#pragma once


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
		return std::move(Color(r * rhs, g * rhs, b * rhs));
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


	UINT32 getColor() const
	{
		return ((UINT32)max(min(r, 255.0f), 0.0f) << 16) | ((UINT32)max(min(g, 255.0f), 0.0f) << 8) | (UINT32)max(min(b, 255.0f), 0.0f);
	}

	float r;
	float g;
	float b;
};