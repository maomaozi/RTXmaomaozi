#pragma once

#include "vec.h"

class AABB
{
public:
	AABB();
	AABB(const Point3 &top_left, Point3 &down_right);
	~AABB();

	bool intersection(const AABB *box);

	void split(float value, int axis, AABB &lhs, AABB &rhs);

	const Point3 &get_top_left() const;

	const Point3 &get_down_right() const;

	void set_top_left(const Point3 &vec);

	void set_down_right(const Point3 &vec);

	int operator==(AABB const * const &lhs) const;

	void *data;
private:
	Point3 top_left;
	Point3 down_right;
};


inline AABB::AABB() : top_left({ 0,0,0 }), down_right({ 0, 0, 0 })
{
}


AABB::AABB(const Point3 &top_left, Point3 &down_right) : top_left(top_left), down_right(down_right)
{
}


AABB::~AABB()
{
}


inline bool AABB::intersection(const AABB *box)
{
	for (int i = 0; i < 3; i++)
	{
		float max_d = (box->down_right[i] - box->top_left[i]) / 2 + (down_right[i] - top_left[i]) / 2;
		float actual_d = abs((box->down_right[i] + box->top_left[i]) / 2 - (down_right[i] + top_left[i]) / 2);

		if (actual_d > max_d) {
			return false;
		}
	}
		
	return true;
}


inline void AABB::split(float value, int axis, AABB &lhs, AABB &rhs)
{
	Point3 new_down_right = Point3(down_right);
	new_down_right[axis] = value;

	Point3 new_top_left = Point3(top_left);
	new_top_left[axis] = value;

	lhs.top_left = top_left;
	lhs.down_right = new_down_right;

	rhs.top_left = new_top_left;
	rhs.down_right = down_right;
}


inline const Point3 & AABB::get_top_left() const
{
	return top_left;
}


inline const Point3 & AABB::get_down_right() const
{
	return down_right;
}


inline void AABB::set_top_left(const Point3 & vec)
{
	top_left = vec;
}


inline void AABB::set_down_right(const Point3 & vec)
{
	down_right = vec;
}


inline int AABB::operator==(AABB const * const &lhs) const
{
	for (int i = 0; i < 3; ++i)
	{
		if (top_left[i] != lhs->get_top_left()[i]) return 0;
		if (down_right[i] != lhs->get_down_right()[i]) return 0;
	}

	return 1;
}
