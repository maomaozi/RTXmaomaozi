#pragma once

#include "vec.h"
#include "object.h"
#include "light.h"

class Tracer
{
public:
	Tracer();
	~Tracer();


private:

	void getMostNearObject(point3 emitPoint, vec3 rayVec)
	{

	}


	Color castShadowRay(const light &lightSource, const point3 &emitPoint, const vec3 &rayVec)
	{
		// check if any object between lightSource and emitPoint
		// if there is something, return 0



		// else return light strength
	}


	// cast a ray to object and get the color of it
	Color castTraceRay(const point3 &emitPoint, const vec3 &rayVec, size_t nowDepth)
	{
		// step 1:	determine if depth reach max trace depth
		if (nowDepth == 0)
		{
			// no contribute
			return Color(0, 0, 0);
		}

		// step 2:	go through each object and calculate intersection
		//			find which intersection is most near the emitPoint
		//			if there is no intersection, return background color

		Intersection firstIntersection;
		int firstIntersectionDistance = MAXINT32;
		bool isFound = false;

		for (auto objIter = objects.begin(); objIter != objects.end(); ++objIter)
		{
			// get all intersection and then calculate distance
			Intersection intersection;
			if ((*objIter)->getIntersection(emitPoint, rayVec, intersection.entryPoint))
			{
				isFound = true;
				int distance = intersection.entryPoint.distance(emitPoint);
				if (distance < firstIntersectionDistance)
				{
					firstIntersection = intersection;
					firstIntersectionDistance = distance;
				}
			}
		}

		// no intersection, return background color
		if (!isFound)
		{
			return backgroundColor;
		}

		// step 3:	cast shadow ray to light source
		//			if shadow ray has intersection with object
		//			(some thing between point and light source), then cast shadow
		//			otherwise collect each light strength and contribute to color

		Color color(0, 0, 0);

		// Cast shadow to every light source
		for (auto lightIter = lights.begin(); lightIter != lights.end(); ++lightIter)
		{
			color += castShadowRay(**lightIter, firstIntersection.entryPoint, (*lightIter)->position - firstIntersection.entryPoint) * (*lightIter)->strength;
		}

		// step 4:	Process reflection, calculate reflection ray and recursion trace
		vec3 reflectionRay(0, 0, 0);
		float strengthReflection = firstIntersection.obj->calcReflectionRay(firstIntersection.entryPoint, rayVec, reflectionRay);
		color += castTraceRay(firstIntersection.entryPoint, reflectionRay, nowDepth - 1) * strengthReflection;

		// step 5:	Process refraction, calculate refraction ray and recursion trace
		vec3 refractionRay(0, 0, 0);
		float strengthRefraction = firstIntersection.obj->calcRefractionRay(firstIntersection.entryPoint, rayVec, refractionRay);
		color += castTraceRay(firstIntersection.entryPoint, refractionRay, nowDepth - 1) * strengthRefraction;

		return color;
	}

public:
	void trace(size_t resolutionWidth, size_t resolutionHeight, const point3 viewPoint, size_t traceDepth, Color bgColor, UINT32 *bitmap)
	{
		backgroundColor = bgColor;

		// calculate color of each pixel
		for (int y = 0; y < resolutionHeight; ++y) 
		{
			for (int x = 0; x < resolutionWidth; ++x) 
			{
				bitmap[x + y * resolutionWidth] = castTraceRay(viewPoint, point3(x, y, 0) - viewPoint, traceDepth).getColor();
			}
		}
	}

private:
	Color backgroundColor;
	std::vector<std::shared_ptr<Object>> objects;						// store all objects
	std::vector<std::shared_ptr<light>> lights;							// use dot light here
};

Tracer::Tracer()
{
}

Tracer::~Tracer()
{
}