#pragma once

#include "vec.h"
#include "object.h"
#include "light.h"
#include <float.h>

class Tracer
{
public:
	Tracer();
	~Tracer();


private:

	Color castShadowRay(const Light &lightSource, const Point3 &emitPoint, const Vec3 &rayVec)
	{
		// Check if any object between lightSource and emitPoint
		// If there is something, return 0

		Intersection getIntersection;

		for (auto objIter = objects.begin(); objIter != objects.end(); ++objIter)
		{
			// if any object block this light source 
			if ((*objIter)->getIntersection(emitPoint, lightSource.position, getIntersection, false))
			{
				// global light
				return globalLight;
			}
		}

		// Else return light information
		return lightSource.color * lightSource.strength + globalLight;
	}


	// Cast a ray to object and get the color of it
	Color castTraceRay(const Point3 &emitPoint, const Vec3 &rayVec, Object *castObj, bool isInMedium, size_t nowDepth)
	{
		// Step 1:	determine if depth reach max trace depth
		if (nowDepth == 0)
		{
			// No contribute
			return Color(0, 0, 0);
		}

		// Step 2:	Go through each object and calculate intersection
		//			find which intersection is most near the emitPoint
		//			if there is no intersection, return background color

		Intersection firstIntersection;					// The most near intersection with object
		float firstIntersectionDistance = FLT_MAX;		// The most near intersection distance
		bool isFound = false;							// If we got any intersection

		for (auto objIter = objects.begin(); objIter != objects.end(); ++objIter)
		{
			// Get all intersection and then calculate distance
			Intersection intersection;
			if ((*objIter)->getIntersection(emitPoint, rayVec, intersection, isInMedium))
			{
				float distance = intersection.entryPoint.distance(emitPoint);

				if (distance < firstIntersectionDistance && (isInMedium || castObj != objIter->get()))
				{
					isFound = true;
					firstIntersection = intersection;
					firstIntersectionDistance = distance;
				}
			}
		}

		// no intersection, return background color
		if (!isFound)
		{
			return globalLight;
		}

		// step 3:	cast shadow ray to light source
		//			if shadow ray has intersection with object
		//			(some thing between point and light source), then cast shadow
		//			otherwise collect each light strength and contribute to color

		Color color(0, 0, 0);

		// Cast shadow to every light source
		if (!isInMedium)
		{
			for (auto lightIter = lights.begin(); lightIter != lights.end(); ++lightIter)
			{
				// Only process direct reflactor(illuminate by light source or background light)
				color += castShadowRay(**lightIter, firstIntersection.entryPoint, (*lightIter)->position - firstIntersection.entryPoint) * firstIntersection.obj->getReflectionFactor();
			}
		}

		// step 4:	Process refraction, calculate refraction ray and recursion trace
		Vec3 refractionRay(0, 0, 0);
		float strengthRefraction = firstIntersection.obj->calcRefractionRay(firstIntersection.entryPoint, rayVec, refractionRay, isInMedium);
		color += castTraceRay(firstIntersection.entryPoint, refractionRay, firstIntersection.obj, !isInMedium, nowDepth - 1) * strengthRefraction;

		// step 5:	Process reflection, calculate reflection ray and recursion trace
		Vec3 reflectionRay(0, 0, 0);
		float strengthReflection = firstIntersection.obj->calcReflectionRay(firstIntersection.entryPoint, rayVec, reflectionRay);

		if (strengthRefraction == 0)
		{
			 //total reflection
			strengthReflection += firstIntersection.obj->getRefractionFactor();
		}

		color += castTraceRay(firstIntersection.entryPoint, reflectionRay, firstIntersection.obj, isInMedium, nowDepth - 1) * strengthReflection;


		return color;
	}

public:

	void addObject(Object *obj) 
	{
		objects.emplace_back(obj);
	}


	void addLightSource(Light *light)
	{
		lights.emplace_back(light);
	}

	void trace(size_t resolutionWidth, size_t resolutionHeight, const Point3 viewPoint, size_t traceDepth, Color bgColor, UINT32 *bitmap)
	{
		backgroundColor = bgColor;

		// calculate color of each pixel
#pragma omp parallel for
		for (int y = 1; y <= resolutionHeight; ++y) 
		{
			for (int x = 0; x < resolutionWidth; ++x) 
			{
				//if (resolutionHeight - y == 540 && x == 810)
				bitmap[x + (resolutionHeight - y) * resolutionWidth] = castTraceRay(viewPoint, Point3(x, y, 0) - viewPoint, nullptr, false, traceDepth).getColor();
			}
		}
	}

private:
	Color backgroundColor;
	Color globalLight = Color(50, 50, 50);
	std::vector<std::shared_ptr<Object>> objects;						// store all objects
	std::vector<std::shared_ptr<Light>> lights;							// use dot light here
};

Tracer::Tracer()
{
}

Tracer::~Tracer()
{
}