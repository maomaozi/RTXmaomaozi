#pragma once

#include "aabb.h"
#include "vec.h"
#include <unordered_set>
#include <algorithm>
#include <vector>

class KdTree
{
public:
	KdTree(AABB area, int split_threshold, int depth, int max_depth);
	~KdTree();

	void insert(const AABB* box);
	void ray_query(const Point3 &point, const Vec3 &direct, std::unordered_set<const AABB *, AABBhash> &result);

private:
	bool is_leaf();
	bool level_intersect(const Point3 &point, const Vec3 &direct);
	void split();
	void insert_to_child(const AABB *box);

private:
	int depth;
	int max_depth;

	std::unordered_set<const AABB *, AABBhash> values;

	AABB area;

	KdTree *l_child = nullptr;
	KdTree *r_child = nullptr;

	int split_threshold;
};

KdTree::KdTree(AABB area, int split_threshold, int depth, int max_depth): 
	area(area), split_threshold(split_threshold), depth(depth), max_depth(max_depth)
{
}

KdTree::~KdTree()
{
	if (l_child != nullptr) delete l_child;
	if (r_child != nullptr) delete r_child;
}

inline void KdTree::insert(const AABB *box)
{
	if (!area.intersection(box)) return;

	if (is_leaf()) 
	{
		values.insert(box);

		if (values.size() > split_threshold && depth < max_depth) split();
	}
	else 
	{
		insert_to_child(box);
	}
}

inline void KdTree::ray_query(const Point3 &point, const Vec3 &direct, std::unordered_set<const AABB *, AABBhash> &result)
{
	if (level_intersect(point, direct)) 
	{

		if (is_leaf()) 
		{
			result.insert(values.cbegin(), values.cend());
			return;
		}

		l_child->ray_query(point, direct, result);
		r_child->ray_query(point, direct, result);
	}

}

inline bool KdTree::is_leaf()
{
	return l_child == nullptr;
}

inline bool KdTree::level_intersect(const Point3 &point, const Vec3 &direct)
{
	float t_min = 0.0f;
	float t_max = FLT_MAX;

	for (int i = 0; i < 3; ++i) 
	{
		if (abs(direct[i]) < 1e-6 && (direct[i] < area.get_top_left()[i] || direct[i] > area.get_down_right()[i]))  return false;

		float n = 1.0f / (direct[i] + 1e-5);

		float t1 = (area.get_top_left()[i] - point[i]) * n;
		float t2 = (area.get_down_right()[i] - point[i]) * n;

		if (t1 > t2) std::swap(t1, t2);
		if (t1 > t_min) t_min = t1;
		if (t2 < t_max) t_max = t2;
		if (t_min > t_max) return false;
	}

	return true;
}

inline void KdTree::split()
{
	float max_var = -1;
	int max_var_axis = -1;
	float max_var_mean = -1;


	for (size_t i = 0; i < 3; i++)
	{
		float sum = 0.0f;
		for (const AABB *v: values)
		{
			sum += (v->get_top_left()[i] +  v->get_down_right()[i]) / 2;
		}

		float mean = sum / (split_threshold + 1);

		float var = 0.0;
		for (const AABB *v : values)
		{
			float center = (v->get_top_left()[i] + v->get_down_right()[i]) / 2;
			var += (center - mean) * (center - mean);
		}

		if (var > max_var) 
		{
			max_var = var;
			max_var_axis = i;
			max_var_mean = mean;
		}
	}

	AABB new_box1, new_box2;
	area.split(max_var_mean, max_var_axis, new_box1, new_box2);

	l_child = new KdTree(new_box1, split_threshold, depth + 1, max_depth);
	r_child = new KdTree(new_box2, split_threshold, depth + 1, max_depth);

	for (const AABB *box : values) {
		insert_to_child(box);
	}

	values.clear();
}

inline void KdTree::insert_to_child(const AABB *box)
{
	l_child->insert(box);
	r_child->insert(box);
}
