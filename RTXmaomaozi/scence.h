#pragma once

#include "light.h"
#include "object.h"
#include "kdTree.h"
#include <vector>
#include <cmath>

class Scence
{
public:
	Scence() {};
	~Scence() 
	{
		// TODO
	};

	void addLight(VolumnLight *vlight)
	{
		AABB *box = new AABB();
		box->data = vlight;
		vlight->calcAABB(*box);

		vlightsRaw.push_back(vlight);
		vlights.push_back(box);
	}

	void addPlane(Plane *plane) 
	{
		objectsRaw.push_back(plane);
		planes.push_back(plane);
	}

	void addTriangle(Triangle *triangle)
	{
		AABB *box = new AABB();
		box->data = triangle;
		triangle->calcAABB(*box);

		objectsRaw.push_back(triangle);

		objects.push_back(box);
	}

	void addSphere(Sphere *sphere)
	{
		AABB *box = new AABB();
		box->data = sphere;
		sphere->calcAABB(*box);

		objectsRaw.push_back(sphere);

		objects.push_back(box);
	}

	void ray_query_vlights(const Point3 &point, const Vec3 &direct, std::unordered_set<void *> &result)
	{
		result.clear();
		vlightTree->ray_query(point, direct, result);
	}

	void ray_query_objects(const Point3 &point, const Vec3 &direct, std::unordered_set<void *> &result)
	{
		result.clear();
		objectTree->ray_query(point, direct, result);
		result.insert(planes.cbegin(), planes.cend());
	}

	const std::vector<VolumnLight *> & getAllLights() const
	{
		return vlightsRaw;
	}

	const std::vector<Object *> & getAllObjects() const
	{
		return objectsRaw;
	}

	void build()
	{
		vlightTree = new KdTree(AABB(Point3(-100000, -100000, -100000), Point3(100000, 100000, 100000)), 2, 0, max(log2(vlights.size() / 2), 2));
		objectTree = new KdTree(AABB(Point3(-100000, -100000, -100000), Point3(100000, 100000, 100000)), 5, 0, max(log2(objects.size() / 5), 2));

		for (auto vlight : vlights) 
		{
			vlightTree->insert(vlight);
		}

		for (auto object : objects)
		{
			objectTree->insert(object);
		}
	}

	
	

private:

	KdTree *vlightTree = nullptr;
	KdTree *objectTree = nullptr;

	std::vector<AABB *> vlights;

	std::vector<VolumnLight *> vlightsRaw;
	std::vector<Object *> objectsRaw;

	std::vector<AABB *> objects;
	std::vector<Plane *> planes;
};
