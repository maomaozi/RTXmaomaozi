#pragma once

#include "vec.h"
#include "object.h"

class Tracer
{
public:
	Tracer();
	~Tracer();

public:

	// cast a ray to object and get the color of it
	char traceRay(point3 emitPoint, vec3 rayVec, size_t nowDepth)
	{
		// step 1:	determine if depth reach max trace depth

		// step 2:	go through each object and calculate intersection
		//			if there is no intersection, return background color

		// step 3:	find which intersection is most near the emitPoint

		// step 4:	cast shadow ray to light source 
	}

	void trace(size_t resolutionWidth, size_t resolutionHeight, const point3 viewPoint, size_t traceDepth, char *bitmap)
	{
		// calculate color of each pixel
		for (int y = 0; y < resolutionHeight; ++y) 
		{
			for (int x = 0; x < resolutionWidth; ++x) 
			{
				bitmap[x + y * resolutionWidth] = traceRay(viewPoint, point3(x, y, 0) - viewPoint, traceDepth);
			}
		}
	}

private:
	std::vector<std::shared_ptr<object>> objects;		// store all objects
	std::vector<point3> lights;							// use dot light here
};

Tracer::Tracer()
{
}

Tracer::~Tracer()
{
}