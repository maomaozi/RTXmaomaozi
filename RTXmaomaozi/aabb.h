#pragma once

#include "vec.h"

class AABB
{
public:
	AABB();
	AABB(const Vec3 &top_left, Vec3 &down_right);
	~AABB();

	bool intersection(const AABB *box);

	void split(float value, int axis, AABB &lhs, AABB &rhs);

	const Vec3 &get_top_left() const;

	const Vec3 &get_down_right() const;

	int operator==(AABB const * const &lhs) const;

private:
	Vec3 top_left;
	Vec3 down_right;
};

inline AABB::AABB() : top_left({ 0,0,0 }), down_right({ 0, 0, 0 })
{
}

AABB::AABB(const Vec3 &top_left, Vec3 &down_right) : top_left(top_left), down_right(down_right)
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
	Vec3 new_down_right = Vec3(down_right);
	new_down_right[axis] = value;

	Vec3 new_top_left = Vec3(top_left);
	new_top_left[axis] = value;

	lhs.top_left = top_left;
	lhs.down_right = new_down_right;

	rhs.top_left = new_top_left;
	rhs.down_right = down_right;
}

inline const Vec3 & AABB::get_top_left() const
{
	return top_left;
}

inline const Vec3 & AABB::get_down_right() const
{
	return down_right;
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


class AABBhash {
public:
	int operator()(AABB const * const &box) const {
		int seed = 131;
		int hash = 0;

		for (int i = 0; i < 3; ++i) 
		{
			hash = (hash * seed) + box->get_top_left()[i];
			hash = (hash * seed) + box->get_down_right()[i];
		}

		return hash;
	}
};