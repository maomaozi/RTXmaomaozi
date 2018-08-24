#pragma once

#include "vec.h"
#include "object.h"
#include "light.h"
#include "camera.h"



class Tracer
{
public:
	Tracer();
	~Tracer();


private:

	Color castShadowRay(const Light &lightSource, const Point3 &emitPoint)
	{
		// Check if any object between lightSource and emitPoint
		// If there is something, return 0

		float lightDistance = (lightSource.position - emitPoint).length();

		for (auto objIter = objects.begin(); objIter != objects.end(); ++objIter)
		{
			// if any object block this light source 
			float distance = (*objIter)->getIntersection(emitPoint, (lightSource.position - emitPoint).normalize(), false);

			if (distance != NO_INTERSECTION && distance < lightDistance)
			{
				// block by some object front fo light source
				// global light
				return Color(0, 0, 0);
			}
		}

		// Else return light information
		return lightSource.color * lightSource.strength;
	}


	void getDirectLight(const Point3 &nowPoint, const Vec3 &reflectorVec, const Vec3 &normVector, float diffuseFactor, Color &accumulateLightColor)
	{
		for (auto lightIter = lights.begin(); lightIter != lights.end(); ++lightIter)
		{
			// Only process direct reflactor(illuminate by light source)

			Vec3 lightDirection = ((*lightIter)->position - nowPoint).normalize();

			float angleDiffuseCos = normVector * lightDirection;
			float angleReflectCos = powf(reflectorVec * lightDirection, 9);

			if (angleReflectCos < (1.0f - diffuseFactor)) angleReflectCos = 0.0f;

			Color lightColor = castShadowRay(**lightIter, nowPoint);

			accumulateLightColor += 
				lightColor * angleDiffuseCos * diffuseFactor +				// come from diffuse
				lightColor * angleReflectCos * (1 - diffuseFactor);			// come from real reflector
		}

		accumulateLightColor += globalLight;
	}


	bool getFirstIntersection(const Point3 &emitPoint, const Vec3 &rayVec, bool isInMedium, Object *castObj, Intersection &firstIntersection)
	{
		// rayVec is always normalized

		float firstIntersectionDistance = FLT_MAX;		// The most near intersection distance
		bool isFound = false;							// If we got any intersection

		for (auto objIter = objects.begin(); objIter != objects.end(); ++objIter)
		{
			// Get all intersection and then calculate distance

			float intersectionDistance = (*objIter)->getIntersection(emitPoint, rayVec, isInMedium);

			if (intersectionDistance > 0 && intersectionDistance < firstIntersectionDistance && (isInMedium || castObj != objIter->get()))
			{
				isFound = true;
				firstIntersectionDistance = intersectionDistance;

				firstIntersection.entryPoint = emitPoint + rayVec * intersectionDistance;
				firstIntersection.obj = objIter->get();
			}
		}

		return isFound;
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


		Intersection firstIntersection;

		if (!getFirstIntersection(emitPoint, rayVec, isInMedium, castObj, firstIntersection))
		{
			// No intersection, return background color
			if (nowDepth == _traceDepth)
			{
				return backgroundColor;
			}
			else 
			{
				return Color(0, 0, 0);
			}
		}

		Color reflectColor(0, 0, 0);		// Actual reflect light color
		Color castOnColor(0, 0, 0);			// Direct and indirect light shade on this object

		// Step 3:	Process reflection, calculate reflection ray and recursion trace
		Vec3 reflectionRay(0, 0, 0);
		firstIntersection.obj->calcReflectionRay(firstIntersection.entryPoint, rayVec, reflectionRay);

		castOnColor += castTraceRay(firstIntersection.entryPoint, reflectionRay, firstIntersection.obj, isInMedium, nowDepth - 1) * (1 - firstIntersection.obj->getDiffuseFactor());


		// Step 4:	Process refraction, calculate refraction ray and recursion trace
		//			If total reflection happend, no need to calculate refraction
		Vec3 refractionRay(0, 0, 0);
		bool totalReflection = firstIntersection.obj->calcRefractionRay(firstIntersection.entryPoint, rayVec, refractionRay, isInMedium);


		// Step 5:	cast shadow ray to light source
		//			if shadow ray has intersection with object
		//			(some thing between point and light source), then cast shadow
		//			otherwise collect each light strength and contribute to color
		// Cast shadow to every light source
		if (!isInMedium)
		{
			Vec3 norm(0, 0, 0);
			firstIntersection.obj->getNormVecAt(firstIntersection.entryPoint, norm);
			getDirectLight(firstIntersection.entryPoint, reflectionRay, norm, firstIntersection.obj->getDiffuseFactor(), castOnColor);
		}

		// Step 6: Process reflect color
		if (totalReflection)
		{
			 //total reflection
			reflectColor += castOnColor;
		}
		else
		{
			reflectColor += castOnColor * firstIntersection.obj->getReflectionRatio(firstIntersection.entryPoint);

			// then add refraction
			reflectColor += castTraceRay(firstIntersection.entryPoint, refractionRay, firstIntersection.obj, !isInMedium, nowDepth - 1) * firstIntersection.obj->getRefractionRatio(firstIntersection.entryPoint);
		}

		
		return reflectColor;
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

	void trace(Camera camera, size_t traceDepth, Color bgColor, Color ambientLight, int antiAliasScale, UINT32 *bitmap)
	{
		backgroundColor = bgColor;
		globalLight = ambientLight;

		_traceDepth = traceDepth;

		// calculate color of each pixel
#pragma omp parallel
		{
#pragma omp for nowait schedule(static, 1)
			for (int y = 0; y < (int)camera.getHeight(); ++y)
			{
				for (int x = 0; x < (int)camera.getWidth(); ++x)
				{

					Vec3 nowViewRay = camera.getViewRay(x, y);
					Vec3 nextViewRayX = camera.getViewRay(x + 1, y);
					Vec3 nextViewRayY = camera.getViewRay(x, y + 1);


					Vec3 diffX = (nextViewRayX - nowViewRay) / (float)antiAliasScale;
					Vec3 diffY = (nextViewRayY - nowViewRay) / (float)antiAliasScale;

					Color buffer(0, 0, 0);

					// calculate sub pixel
					for (int subY = 0; subY < antiAliasScale; ++subY) 
					{
						for (int subX = 0; subX < antiAliasScale; ++subX) 
						{
							buffer += castTraceRay(camera.getViewPoint(), nowViewRay + diffY * subY + diffX * subX, nullptr, false, traceDepth);
						}
					}
					
					bitmap[x + y * (int)camera.getWidth()] = (buffer / (antiAliasScale * antiAliasScale)).getColor();
				}
			}
		}
	}

private:
	Color globalLight;
	Color backgroundColor;

	int _traceDepth;

	std::vector<std::shared_ptr<Object>> objects;						// store all objects
	std::vector<std::shared_ptr<Light>> lights;							// use dot light here
};

Tracer::Tracer()
{
}

Tracer::~Tracer()
{
}