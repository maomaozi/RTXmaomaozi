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

	int isShadow(const Vec3 &lightDirection, float lightDistance, const Intersection &intersectin, bool isInMedium)
	{
		// three state:
		// 0. no shadowed
		// -1. inner shadowed by it self but not shadow by other(only happend while isInMedium is true)
		// 1. shadow by other

		// Check if any object between lightSource and emitPoint
		// If there is something, return true

		int result = 0;	// no shadowed

		for (auto objIter = objects.begin(); objIter != objects.end(); ++objIter)
		{
			// if any object block this light source, in medium will not block by medium itself 
			float distance = (*objIter)->getIntersection(intersectin.entryPoint, lightDirection, isInMedium);

			if (distance != NO_INTERSECTION && distance < lightDistance)
			{
				// block by some object front fo light source
				if (isInMedium && intersectin.obj == (*objIter).get())
				{
					result = -1;
				}
				else
				{
					return 1;
				}
			}
		}

		// can reach light source direct
		return result;
	}


	float getFirstIntersection(const Point3 &emitPoint, const Vec3 &rayVec, bool isInMedium, Object *castObj, Intersection &firstIntersection)
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

		if (isFound)
		{
			return firstIntersectionDistance;
		}
		else
		{
			return NO_INTERSECTION;
		}
	}


	float getFirstlight(const Point3 &emitPoint, const Vec3 &rayVec, Light *&light)
	{
		// rayVec is always normalized

		float firstIntersectionDistance = FLT_MAX;		// The most near intersection distance
		bool isFound = false;							// If we got any intersection

		for (auto lightIter = lights.begin(); lightIter != lights.end(); ++lightIter)
		{
			// Get all intersection and then calculate distance
			VolumnLight *vLight = dynamic_cast<VolumnLight *>((*lightIter).get());
			float intersectionDistance = vLight->getIntersection(emitPoint, rayVec);

			if (intersectionDistance != NO_INTERSECTION && intersectionDistance < firstIntersectionDistance)
			{
				isFound = true;
				light = (*lightIter).get();
				firstIntersectionDistance = intersectionDistance;
			}
		}

		if (isFound)
		{
			return firstIntersectionDistance;
		}
		else
		{
			return NO_INTERSECTION;
		}
	}


	void deffuseMonteCarlo(const Intersection &intersection, const Vec3 &rayVec,  bool isInMedium, int nowDepth, Color &reflectionColor)
	{
		static std::default_random_engine e;
		static std::uniform_real_distribution<float> u(-1, 1);

		Color diffuseColor(0, 0, 0);
		Vec3 p(0, 0, 0);
		Vec3 norm(0, 0, 0);
		intersection.obj->getNormVecAt(intersection.entryPoint, norm);

		float ratio = acosf(-2.0f * intersection.obj->getDiffuseFactor() + 1.0f) / PI;

		int counter = 0;
		for (int i = 0; i < 10 * ratio; ++i)
		{
			// vector create referer to https://math.stackexchange.com/questions/2464998/random-vector-with-fixed-angle

			// use new c++11 random engine here
			float targetCosAngle = 1 + (u(e) - 1) * intersection.obj->getDiffuseFactor();

			p.x = u(e);
			p.y = u(e);
			p.z = u(e);

			/*
			float targetCosAngle = -(rand() % (int)(intersection.obj->getDiffuseFactor() * RAND_MAX) - RAND_MAX / 2) / (RAND_MAX / 2.0f);

			p.x = (rand() % RAND_MAX - RAND_MAX / 2) / (RAND_MAX / 2.0f);
			p.y = (rand() % RAND_MAX - RAND_MAX / 2) / (RAND_MAX / 2.0f);
			p.z = (rand() % RAND_MAX - RAND_MAX / 2) / (RAND_MAX / 2.0f);
			*/

			p -= rayVec * ((p * rayVec) / (rayVec * rayVec));

			p /= p.length();
			p *= rayVec.length();

			Vec3 v = rayVec * targetCosAngle;
			p *= sqrtf(1.0f - targetCosAngle * targetCosAngle);
			v += p;

			if (v * norm <= 0)
			{
				--i;
				continue;
			}

			diffuseColor.r = diffuseColor.g = diffuseColor.b = 0.0f;
			castTraceRay(intersection.entryPoint, v, intersection.obj, isInMedium, nowDepth - 1, diffuseColor);

			diffuseColor *= powf(norm * v, intersection.obj->getDiffuseFactor());

			reflectionColor += diffuseColor;
			++counter;
		}

		if (counter)
		{
			reflectionColor /= counter;
		}
	}


	// Cast a ray to object and add the light of it on color parameter
	void castTraceRay(const Point3 &emitPoint, const Vec3 &rayVec, Object *castObj, bool isInMedium, int nowDepth, Color &light)
	{
		/* 
		Step 1:	Determine if depth reach max trace depth
		*/
		if (nowDepth <= 0)
		{
			return;		// Stop iter
		}


		/*
		Step 2:
			Go through each object and calculate intersection
			find which intersection is most near the emitPoint
			if there is no intersection, check if intersect with light source
		*/
		Intersection firstIntersection;
		Light *firstLightSource;

		float objDistance = getFirstIntersection(emitPoint, rayVec, isInMedium, castObj, firstIntersection);
		float lightDistance = getFirstlight(emitPoint, rayVec, firstLightSource);

		if (lightDistance != NO_INTERSECTION && (objDistance == NO_INTERSECTION || objDistance > lightDistance))
		{
			light += firstLightSource->getLightStrength(rayVec, lightDistance, rayVec);
		}

		if (objDistance == NO_INTERSECTION)
		{
			return;
		}

		/*
		Step 3:	
			Process refraction, calculate refraction ray and recursion trace
			If total reflection happend, no need to calculate refraction
		*/
		Vec3 refractionRay(0, 0, 0);
		bool totalReflection = firstIntersection.obj->calcRefractionRay(firstIntersection.entryPoint, rayVec, refractionRay, isInMedium);

		if (!totalReflection)
		{
			// Calculate refraction
			Color refractionColor(0, 0, 0);
			castTraceRay(firstIntersection.entryPoint, refractionRay, firstIntersection.obj, !isInMedium, nowDepth - 1, refractionColor);

			refractionColor *= firstIntersection.obj->getRefractionRatio(firstIntersection.entryPoint);

			light += refractionColor;
		}

		/*
		Step 4:
			Calculate all reflection ray and recursion trace
		*/
		Vec3 mainReflectionRay(0, 0, 0);
		firstIntersection.obj->calcReflectionRay(firstIntersection.entryPoint, rayVec, mainReflectionRay);

		Color reflectionColor(0, 0, 0);

		// Use accurate reflect model, monte-carlo simulation of diffuse
		deffuseMonteCarlo(firstIntersection, mainReflectionRay, isInMedium, nowDepth, reflectionColor);

		if (totalReflection)
		{
			Color newReflectRatio = firstIntersection.obj->getReflectionRatio(firstIntersection.entryPoint);
			newReflectRatio += firstIntersection.obj->getRefractionRatio(firstIntersection.entryPoint);

			reflectionColor *= newReflectRatio;
		}
		else
		{
			reflectionColor *= firstIntersection.obj->getReflectionRatio(firstIntersection.entryPoint);
		}

		light += reflectionColor;
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
							//if(y >= 160 && y <= 200)
							castTraceRay(camera.getViewPoint(), nowViewRay + diffY * subY + diffX * subX, nullptr, false, traceDepth, buffer);
						}
					}

					buffer /= (float)(antiAliasScale * antiAliasScale);
					bitmap[x + y * (int)camera.getWidth()] = buffer.getColor();
				}
			}
		}
	}

private:
	Color globalLight;
	Color backgroundColor;

	std::vector<std::shared_ptr<Object>> objects;						// store all objects
	std::vector<std::shared_ptr<Light>> lights;							// store all light
};

Tracer::Tracer()
{
}

Tracer::~Tracer()
{
}