#pragma once

#include "vec.h"
#include "object.h"
#include "light.h"
#include "camera.h"
#include "scence.h"


//#define USE_MC_REFLECT
#define FASTER_RENDER
//#define USE_KD_TREE

class Tracer
{
public:
	Tracer();
	~Tracer();


private:

	int isShadow(const Vec3 &lightDirection, float lightDistance, const Intersection &intersection, bool isInMedium)
	{
		// three state:
		// 0. no shadowed
		// -1. inner shadowed by it self but not shadow by other(only happend while rayInMedium is true)
		// 1. shadow by other

		// Check if any object between lightSource and emitPoint
		// If there is something, return true

		int result = 0;	// no shadowed

#ifdef USE_KD_TREE
		thread_local static std::unordered_set<void *> filter_objects;
		scence->ray_query_objects(intersection.intersectionPoint, lightDirection, filter_objects);

		for (auto objIter : filter_objects)
#else
		for (auto objIter : scence->getAllObjects())
#endif // USE_KD_TREE
		{
			// if any object block this light source, in medium will not block by medium itself 
			Object *obj = (Object *)objIter;
			float distance = obj->getIntersection(intersection.intersectionPoint, lightDirection, isInMedium);

			if (distance != NO_INTERSECTION && distance < lightDistance)
			{
				// block by some object front fo light source
				if (isInMedium && intersection.obj == objIter)
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


	float getNearestObject(const Point3 &emitPoint, const Vec3 &rayVec, bool isInMedium, Object *castObj, Intersection &firstIntersection)
	{
		// rayDirect is always normalized

		float firstIntersectionDistance = FLT_MAX;		// The most near intersection distance
		bool isFound = false;							// If we got any intersection

#ifdef USE_KD_TREE
		thread_local static std::unordered_set<void *> filter_objects;
		scence->ray_query_objects(emitPoint, rayVec, filter_objects);
		for (auto objIter : filter_objects)
#else
		for (auto objIter : scence->getAllObjects())
#endif
		{
			// Get all intersection and then calculate distance

			Object *obj = (Object *)objIter;

			float intersectionDistance = obj->getIntersection(emitPoint, rayVec, isInMedium);

			if (intersectionDistance > 0 && intersectionDistance < firstIntersectionDistance && (isInMedium || castObj != objIter))
			{
				isFound = true;
				firstIntersectionDistance = intersectionDistance;

				firstIntersection.intersectionPoint = emitPoint + rayVec * intersectionDistance;
				firstIntersection.obj = obj;
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


	float getNearestLight(const Point3 &emitPoint, const Vec3 &rayVec, VolumnLight *&light)
	{
		// rayDirect is always normalized

		float firstIntersectionDistance = FLT_MAX;		// The most near intersection distance
		bool isFound = false;							// If we got any intersection

#ifdef USE_KD_TREE
		thread_local static std::unordered_set<void *> filter_lights;
		scence->ray_query_vlights(emitPoint, rayVec, filter_lights);

		for (auto lightIter : filter_lights)
#else
		for (auto vLight : scence->getAllLights())
#endif
		{
			// Get all intersection and then calculate distance
			float intersectionDistance = vLight->getIntersection(emitPoint, rayVec);

			if (intersectionDistance != NO_INTERSECTION && intersectionDistance < firstIntersectionDistance)
			{
				isFound = true;
				light = vLight;
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


	void directLightColour(const Intersection &intersection, const Vec3 &rayVec, bool isInMedium, Color &accumulateLightColor)
	{

		Vec3 normVector(0, 0, 0);
		intersection.obj->getNormVecAt(intersection.intersectionPoint, normVector);

		for (auto lightIter : scence->getAllLights())
		{
			Color lightBuffer(0, 0, 0);

			VolumnLight *vLight =(VolumnLight *)lightIter;

			int sampleTime = 1;

			Vec3 lightDirection(0, 0, 0);

			Vec3 lightCenterDirection = vLight->position - intersection.intersectionPoint;
			lightCenterDirection.normalize();

			for (int i = 0; i < sampleTime; ++i)
			{
				// Only process direct reflactor(illuminate by light source)
				float ratio;
				float lightSourceDistance = vLight->sampleRayVec(intersection.intersectionPoint, lightDirection, ratio);

				int shadowState = isShadow(lightDirection, lightSourceDistance, intersection, isInMedium);

				if (shadowState <= 0 )
				{
					Color lightColor = vLight->getLightStrength(lightDirection, lightSourceDistance, normVector);
					lightColor *= intersection.obj->getDiffuseFactor() * normVector.dot(lightDirection);

					lightBuffer += lightColor * ratio;

				}
			}
			lightBuffer /= sampleTime;

			accumulateLightColor += lightBuffer;
		}

		accumulateLightColor += ambientLight;
	}


	inline void xorshift32(uint32_t &state)
	{
		state ^= state << 13;
		state ^= state >> 17;
		state ^= state << 5;
	}

	inline uint32_t fastrand(uint32_t &state) {
		state = (214013 * state + 2531011);
		return state;
	}



	void deffuseMonteCarlo(const Intersection &intersection, const Vec3 &rayVec,  bool isInMedium, int nowDepth, Color &reflectionColor)
	{
		
		Color diffuseColor(0, 0, 0);
		Vec3 p(0, 0, 0);
		Vec3 norm(0, 0, 0);

		intersection.obj->getNormVecAt(intersection.intersectionPoint, norm);

		float rayVecDot = rayVec * rayVec;
		float rayVecLength = rayVec.length();

		int sampleTime = 5;
		
		uint32_t state = rand();

		for (int i = 0; i < sampleTime; ++i) {
			float targetCosAngle = 1.0f - (fastrand(state) / 2147483647.5f) * intersection.obj->getDiffuseFactor();

			// vector create referer to https://math.stackexchange.com/questions/2464998/random-vector-with-fixed-angle

			p.x = 1.0f - fastrand(state) / 2147483647.5f;
			p.y = 1.0f - fastrand(state) / 2147483647.5f;
			p.z = 1.0f - fastrand(state) / 2147483647.5f;


			p -= rayVec * ((p * rayVec) / rayVecDot);

			p /= p.length();
			p *= rayVecLength;

			Vec3 v = rayVec * targetCosAngle;
			p *= sqrtf(1.0f - targetCosAngle * targetCosAngle);
			v += p;

			castTraceRay(intersection.intersectionPoint, v, intersection.obj, isInMedium, nowDepth - 2, diffuseColor);

		}

		reflectionColor += diffuseColor / sampleTime;
	}


	// Cast a ray to object and add the light of it on color parameter
	void castTraceRay(const Point3 &emitPoint, const Vec3 &rayDirect, Object *emitObject, bool rayInMedium, int nowDepth, Color &light)
	{
		/* 
		Step 1:	
			Determine if depth reach max trace depth
		*/
		if (nowDepth <= 0)
		{
			return;		// Stop iter
		}

		/*
		Step 2:
			Check if intersect with light source can direct illuminate the surface
		*/
		Intersection nearestObjectIntersection;
		float objDistance = getNearestObject(emitPoint, rayDirect, rayInMedium, emitObject, nearestObjectIntersection);


		//Check if intersect with light source can direct illuminate the surface
		VolumnLight *nearestLightSource;

		float lightDistance = getNearestLight(emitPoint, rayDirect, nearestLightSource);


		if (lightDistance != NO_INTERSECTION && (objDistance == NO_INTERSECTION || objDistance > lightDistance))
		{
			light += nearestLightSource->getLightStrength(rayDirect, lightDistance, rayDirect);
		}

		// See though background
		if (objDistance == NO_INTERSECTION)
		{
			if (lightDistance == NO_INTERSECTION)
			{
				light += backgroundColor;
				return;
			}
			else
			{
				return;
			}
		}

		/*
		Step 3:	
			Process refraction, calculate refraction ray and recursion trace
			If total reflection happend, no need to calculate refraction
		*/
		bool totalReflection = false;

		if (nearestObjectIntersection.obj->getRefractionRatio(nearestObjectIntersection.intersectionPoint).getStrength() >= 0.1f) {
			Vec3 refractionRayDirect(0, 0, 0);
			totalReflection = nearestObjectIntersection.obj->calcRefractionRay(nearestObjectIntersection.intersectionPoint, rayDirect, rayInMedium, refractionRayDirect);

			if (!totalReflection)
			{
				// Calculate refraction
				Color refractionColor(0, 0, 0);
				castTraceRay(nearestObjectIntersection.intersectionPoint, refractionRayDirect, nearestObjectIntersection.obj, !rayInMedium, nowDepth - 1, refractionColor);

				refractionColor *= nearestObjectIntersection.obj->getRefractionRatio(nearestObjectIntersection.intersectionPoint);

				light += refractionColor;
			}
		}

		/*
		Step 4:
			Calculate all reflection ray and recursion trace
		*/
		Vec3 mainReflectionRayDirect(0, 0, 0);
		nearestObjectIntersection.obj->calcReflectionRay(nearestObjectIntersection.intersectionPoint, rayDirect, mainReflectionRayDirect);

		Color reflectionColor(0, 0, 0);

#ifdef USE_MC_REFLECT
		if (nowDepth >= traceDepth - 5 && (emitObject == NULL || emitObject->getDiffuseFactor() <= 0.01f) && nearestObjectIntersection.obj->getDiffuseFactor() >= 0.01f)
		{
			// Use accurate monte-carlo reflect model simulation of diffuse
			deffuseMonteCarlo(nearestObjectIntersection, mainReflectionRayDirect, rayInMedium, nowDepth, reflectionColor);
		}
		else
#endif
		{
			// The direct reflect part
			castTraceRay(nearestObjectIntersection.intersectionPoint, mainReflectionRayDirect, nearestObjectIntersection.obj, rayInMedium, nowDepth - 1, reflectionColor);

			// If object is diffuse, direct reflector will have less weight
			reflectionColor *= (1 - nearestObjectIntersection.obj->getDiffuseFactor());

		}

		// The diffuse part (only diffuse light to reduce calculation)
		directLightColour(nearestObjectIntersection, mainReflectionRayDirect, rayInMedium, reflectionColor);


		if (totalReflection)
		{
			reflectionColor *= nearestObjectIntersection.obj->getTotalReflectionRatio(nearestObjectIntersection.intersectionPoint);
		}
		else
		{
			reflectionColor *= nearestObjectIntersection.obj->getReflectionRatio(nearestObjectIntersection.intersectionPoint);
		}

		light += reflectionColor;
	}

public:

	void setSence(Scence *newScence)
	{
		scence = newScence;
	}

	void renderPixel(int x, int y, const Camera &camera, UINT32 *pixel)
	{
		Vec3 nowViewRay = camera.getViewRay(x, y);
		Vec3 nextViewRayX = camera.getViewRay(x + 1, y);
		Vec3 nextViewRayY = camera.getViewRay(x, y + 1);


		Vec3 diffX = (nextViewRayX - nowViewRay) / (float)antiAliasScale;
		Vec3 diffY = (nextViewRayY - nowViewRay) / (float)antiAliasScale;

		Color buffer(0, 0, 0);

		// calculate sub pixel for anti-alias
		for (int subY = 0; subY < antiAliasScale; ++subY)
		{
			for (int subX = 0; subX < antiAliasScale; ++subX)
			{
				castTraceRay(camera.getViewPoint(), nowViewRay + diffY * subY + diffX * subX, nullptr, false, traceDepth, buffer);
			}
		}

		buffer /= (float)(antiAliasScale * antiAliasScale);
		*pixel = buffer.getColor();
	}


	void trace(const Camera &camera, size_t traceDepth, const Color &backgroundColor, const Color &ambientLight, int antiAliasScale, UINT32 *bitmap)
	{
		this->backgroundColor = backgroundColor;
		this->ambientLight = ambientLight;
		this->traceDepth = traceDepth;
		this->antiAliasScale = antiAliasScale;

		// calculate color of each pixel

		int w = camera.getWidth();
		int h = camera.getHeight();

#pragma omp parallel
		{

#pragma omp for nowait schedule(static, 2)
#ifndef FASTER_RENDER

			for (int y = 0; y < h; ++y)
			{
				for (int x = 0; x < w; ++x)
				{
					renderPixel(x, y, camera, &bitmap[x + y * w]);
				}
			}
#else
			for (int y = 0; y < h; y += 4)
			{
				for (int x = 0; x < w; x += 4)
				{
					renderPixel(x, y, camera, &bitmap[x + y * w]);
				}
			}

			for (int space = 2; space >= 1; --space)
			{
#pragma omp barrier
#pragma omp for nowait schedule(static, 2)
				for (int y = 0; y < h - space - 1; y += space)
				{
					for (int x = 0; x < w - space - 1; x += space)
					{

						int colorLhs = 0x000000;
						int colorRhs = 0xffffff;

						if (y % (space << 1) == 0 && x % (space << 1) != 0)
						{
							colorLhs = bitmap[(x - space) + y * w];
							colorRhs = bitmap[(x + space) + y * w];
						}
						else if (y % (space << 1) != 0 && x % (space << 1) == 0)
						{
							colorLhs = bitmap[x + (y - space) * w];
							colorRhs = bitmap[x + (y + space) * w];
						}
						else if (y % (space << 1) != 0 && x % (space << 1) != 0)
						{
							//colorLhs = bitmap[(x - space) + y * w];
							//colorRhs = bitmap[x + (y - space) * w];
						}
						else
						{
							continue;
						}

						int lr = (colorLhs & 0xff0000) >> 16;
						int lg = (colorLhs & 0x00ff00) >> 8;
						int lb = (colorLhs & 0x0000ff);

						int rr = (colorRhs & 0xff0000) >> 16;
						int rg = (colorRhs & 0x00ff00) >> 8;
						int rb = (colorRhs & 0x0000ff);

						int rDiff = abs(lr - rr);
						int gDiff = abs(lg - rg);
						int bDiff = abs(lb - rb);

						if (rDiff + gDiff + bDiff < 10)
						{
							bitmap[x + y * w] = Color((lr + rr) >> 1, (lg + rg) >> 1, (lb + rb) >> 1).getColor();
						}
						else
						{
							renderPixel(x, y, camera, &bitmap[x + y * w]);
						}
					}
				}
			}
#endif
		}
	}

private:
	int traceDepth;
	int antiAliasScale;

	Color ambientLight;
	Color backgroundColor;

	Scence *scence;
};

Tracer::Tracer()
{
}

Tracer::~Tracer()
{
}