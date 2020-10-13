#pragma once
#include "stdafx.h"

#define EPSILON 0.001f
#define PI 3.14159265358979f


//#define USE_SSE_AVX

/*
		    ¡ü y    %
			|     / z
			|   /
			| /
			|__________ x


	   Angle is define as conter-clockwise ( While one axis point to right and one point to sky)
*/

float mySqrt(float x)
{
	float a = x;
	unsigned int i = *(unsigned int *)&x;
	i = (i + 0x3f76cf62) >> 1;
	x = *(float *) & i;
	x = (x + a / x) * 0.5f;
	return x;
}

struct Struct3 
{
#ifdef USE_SSE_AVX
	Struct3(float x, float y, float z)
	{
		f[0] = x;
		f[1] = y;
		f[2] = z;
		f[3] = 0.0f;
	}
#else
	Struct3(float x, float y, float z) : x(x), y(y), z(z) {}
#endif

	void operator+=(const Struct3 &rhs)
	{
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;

	}


	void operator-=(const Struct3 &rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
		z -= rhs.z;
	}

	float operator[](int i) const
	{
		if (i == 0) return x;
		if (i == 1) return y;
		if (i == 2) return z;

		return 0;
	}

	float &operator[](int i)
	{
		if (i == 0) return x;
		if (i == 1) return y;

		return z;
	}

	bool operator==(const Struct3 &rhs) const
	{
		return x == rhs.x && y == rhs.y && z == rhs.z;
	}

#ifdef USE_SSE_AVX
	Struct3 &operator=(const Struct3 &rhs)
	{
		if (&rhs != this)
		{
			f[0] = rhs.f[0];
			f[1] = rhs.f[1];
			f[2] = rhs.f[2];
		}

		return *this;
	}

	Struct3(const Struct3 &rhs)
	{
		f[0] = rhs.f[0];
		f[1] = rhs.f[1];
		f[2] = rhs.f[2];
		f[3] = 0.0f;
	}
#endif

#ifdef USE_SSE_AVX
	
	__declspec(align(16)) float f[4];

	float &x = f[0];
	float &y = f[1];
	float &z = f[2];

	mutable float tmp_float4[4];
#else
	float x, y, z;
#endif
};


struct Vec3 : public Struct3
{

	Vec3() = delete;

	Vec3(float x, float y, float z) : Struct3(x, y, z)
	{
		;
	}


	float dot(const Vec3 &rhs) const
	{
#ifdef USE_SSE_AVX

		__m128 op1 = _mm_load_ps(f);
		__m128 op2 = _mm_load_ps(rhs.f);

		__m128 r1 = _mm_mul_ps(op1, op2);
		__m128 r2 = _mm_hadd_ps(r1, r1);
		__m128 r3 = _mm_hadd_ps(r2, r2);

		return r3.m128_f32[0];

#else
		return x * rhs.x + y * rhs.y + z * rhs.z;
#endif
	}


	Vec3 xmul(const Vec3 &rhs) const
	{
		return Vec3(y * rhs.z - rhs.y * z, z * rhs.x - rhs.z * x, x * rhs.y - rhs.x * y);
	}


	void normalize()
	{
		float len = length();

		if (len != 0)
		{
			len += EPSILON;

			x /= len;
			y /= len;
			z /= len;
		}
	}


	float length() const
	{
		return sqrtf(x * x + y * y + z * z);
	}


	float angle(const Vec3 &rhs) const
	{

		Vec3 vec1 = *this;
		vec1.normalize();

		Vec3 vec2 = rhs;
		vec2.normalize();

		float tmp = vec1 * vec2;

		if (fabs(fabs(tmp) - 1) < EPSILON * 10)
		{
			return tmp > 0 ? 0 : PI;
		}

		return acosf(tmp);
	}


	Vec3 operator+(const Vec3 &rhs) const
	{
		return std::move(Vec3(rhs.x + x, rhs.y + y, rhs.z + z));
	}


	Vec3 operator-(const Vec3 &rhs) const
	{
		return std::move(Vec3(x - rhs.x, y - rhs.y, z - rhs.z));
	}


	Vec3 operator*(float rhs) const
	{
		return std::move(Vec3(x * rhs, y * rhs, z * rhs));
	}


	void operator*=(float rhs)
	{
		x *= rhs;
		y *= rhs;
		z *= rhs;
	}

	void operator/=(float rhs)
	{
		x /= rhs;
		y /= rhs;
		z /= rhs;
	}


	Vec3 operator/(float rhs) const
	{
		return std::move(Vec3(x / rhs, y / rhs, z / rhs));
	}


	float operator*(const Vec3 &rhs) const
	{
		return dot(rhs);
	}


	Vec3 operator-() const
	{
		return std::move(Vec3(-x, -y, -z));
	}
};


struct Point3 : public Struct3
{
	Point3() = delete;

	Point3(float x, float y, float z) : Struct3(x, y, z)
	{
		;
	}


	float distance(const Point3 &rhs)
	{
		return sqrtf((x - rhs.x) * (x - rhs.x) + (y - rhs.y) * (y - rhs.y) + (z - rhs.z) * (z - rhs.z));
	}


	//	move point alone vector
	Point3 operator+(const Vec3 &rhs) const
	{
		return std::move(Point3(x + rhs.x, y + rhs.y, z + rhs.z));
	}


	//	move point alone vector
	Point3 operator-(const Vec3 &rhs) const
	{
		return std::move(Point3(x - rhs.x, y - rhs.y, z - rhs.z));
	}


	//	calculate vector between two points
	Vec3 operator-(const Point3 &rhs) const
	{
		return std::move(Vec3(x - rhs.x, y - rhs.y, z - rhs.z));
	}


	Point3 operator-() const
	{
		return std::move(Point3(-x, -y, -z));
	}
};