#pragma once
#include "stdafx.h"

#define EPSILON 1e-5f
#define PI 3.14159265358979f


/*
		    ¡ü y
			|
			|
			|
			|----------¡úx
		   /
		  /
		 /
	   ¨L z

	   Angle is define as conter-clockwise ( While one axis point to right and one point to sky)
*/



struct struct3 
{
	struct3(float x, float y, float z) : x(x), y(y), z(z) {}

	struct3 &operator+=(const struct3 &rhs)
	{
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;

		return *this;
	}


	struct3 &operator-=(const struct3 &rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
		z -= rhs.z;

		return *this;
	}


	bool operator==(const struct3 &rhs) const
	{
		return x == rhs.x && y == rhs.y && z == rhs.z;
	}


	float x;
	float y;
	float z;
};


struct vec3 : public struct3
{

	vec3() = delete;

	vec3(float x, float y, float z) : struct3(x, y, z)
	{
		;
	}


	float dot(const vec3 &rhs) const
	{
		return x * rhs.x + y * rhs.y + z * rhs.z;
	}


	vec3 xmul(const vec3 &rhs) const
	{
		return vec3(y * rhs.z - rhs.y * z, z * rhs.x - rhs.z * x, x * rhs.y - rhs.x * y);
	}


	vec3 normalize() const
	{
		float len = length();

		if(len == 0)
		{
			return vec3(1, 0, 0);
		}

		len += EPSILON;

		return vec3(x / len, y / len, z / len);
	}


	float length() const
	{
		return sqrtf(powf(x, 2) + powf(y, 2) + powf(z, 2));
	}


	float angle(const vec3 &rhs) const
	{

		vec3 vec1 = this->normalize();
		vec3 vec2 = rhs.normalize();

		float tmp = vec1 * vec2;

		if (fabs(fabs(tmp) - 1) < EPSILON * 10)
		{
			return tmp > 0 ? 0 : PI;
		}

		float angle = acosf(tmp);

		return angle;

		//vec3 axis = this->xmul(vec2);

		//return axis * vec3(0.0f, 0.0f, 1.0f) > 0.0f ? angle : 2 * PI - angle;
	}


	vec3 operator+(const vec3 &rhs) const
	{
		return vec3(rhs.x + x, rhs.y + y, rhs.z + z);
	}


	vec3 operator-(const vec3 &rhs) const
	{
		return vec3(x - rhs.x, y - rhs.y, z - rhs.z);
	}


	float operator*(const vec3 &rhs) const
	{
		return dot(rhs);
	}


	vec3 operator-() const
	{
		return vec3(-x, -y, -z);
	}
};


struct point3 : public struct3
{
	point3() = delete;

	point3(float x, float y, float z) : struct3(x, y, z)
	{
		;
	}


	float distance(const point3 &rhs)
	{
		return sqrtf(powf(x - rhs.x, 2) + powf(y - rhs.y, 2) + powf(z - rhs.z, 2));
	}


	//	move point alone vector
	point3 operator+(const vec3 &rhs) const
	{
		return point3(x + rhs.x, y + rhs.y, z + rhs.z);
	}


	//	move point alone vector
	point3 operator-(const vec3 &rhs) const
	{
		return point3(x - rhs.x, y - rhs.y, z - rhs.z);
	}


	//	calculate vector between two points
	vec3 operator-(const point3 &rhs) const
	{
		return vec3(x - rhs.x, y - rhs.y, z - rhs.z);
	}


	point3 operator-() const
	{
		return point3(-x, -y, -z);
	}
};