#pragma once
#include "stdafx.h"

#define EPSILON 0.001f
#define PI 3.14159265358979f


/*
		    ¡ü y    %
			|     / z
			|   /
			| /
			|__________ x


	   Angle is define as conter-clockwise ( While one axis point to right and one point to sky)
*/



struct Struct3 
{
	Struct3(float x, float y, float z) : x(x), y(y), z(z) {}

	Struct3 &operator+=(const Struct3 &rhs)
	{
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;

		return *this;
	}


	Struct3 &operator-=(const Struct3 &rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
		z -= rhs.z;

		return *this;
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


	float x;
	float y;
	float z;
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
		return x * rhs.x + y * rhs.y + z * rhs.z;
	}


	Vec3 xmul(const Vec3 &rhs) const
	{
		return Vec3(y * rhs.z - rhs.y * z, z * rhs.x - rhs.z * x, x * rhs.y - rhs.x * y);
	}


	Vec3 normalize() const
	{
		float len = length();

		if(len == 0)
		{
			return Vec3(1, 0, 0);
		}

		len += EPSILON;

		return Vec3(x / len, y / len, z / len);
	}


	float length() const
	{
		return sqrtf(powf(x, 2) + powf(y, 2) + powf(z, 2));
	}


	float angle(const Vec3 &rhs) const
	{

		Vec3 vec1 = this->normalize();
		Vec3 vec2 = rhs.normalize();

		float tmp = vec1 * vec2;

		if (fabs(fabs(tmp) - 1) < EPSILON * 10)
		{
			return tmp > 0 ? 0 : PI;
		}

		float angle = acosf(tmp);

		return angle;

		//Vec3 axis = this->xmul(vec2);

		//return axis * Vec3(0.0f, 0.0f, 1.0f) > 0.0f ? angle : 2 * PI - angle;
	}


	Vec3 operator+(const Vec3 &rhs) const
	{
		return Vec3(rhs.x + x, rhs.y + y, rhs.z + z);
	}


	Vec3 operator-(const Vec3 &rhs) const
	{
		return Vec3(x - rhs.x, y - rhs.y, z - rhs.z);
	}


	Vec3 operator*(float rhs) const
	{
		return Vec3(x * rhs, y * rhs, z * rhs);
	}

	Vec3 operator/(float rhs) const
	{
		return Vec3(x / rhs, y / rhs, z / rhs);
	}


	float operator*(const Vec3 &rhs) const
	{
		return dot(rhs);
	}


	Vec3 operator-() const
	{
		return Vec3(-x, -y, -z);
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
		return sqrtf(powf(x - rhs.x, 2) + powf(y - rhs.y, 2) + powf(z - rhs.z, 2));
	}


	//	move point alone vector
	Point3 operator+(const Vec3 &rhs) const
	{
		return Point3(x + rhs.x, y + rhs.y, z + rhs.z);
	}


	//	move point alone vector
	Point3 operator-(const Vec3 &rhs) const
	{
		return Point3(x - rhs.x, y - rhs.y, z - rhs.z);
	}


	//	calculate vector between two points
	Vec3 operator-(const Point3 &rhs) const
	{
		return Vec3(x - rhs.x, y - rhs.y, z - rhs.z);
	}


	Point3 operator-() const
	{
		return Point3(-x, -y, -z);
	}
};