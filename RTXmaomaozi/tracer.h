#pragma once

#include "vec.h"
#include "object.h"
#include "light.h"
#include "camera.h"


#define USE_MC_REFLECT
#define FASTER_RENDER

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
		// -1. inner shadowed by it self but not shadow by other(only happend while rayInMedium is true)
		// 1. shadow by other

		// Check if any object between lightSource and emitPoint
		// If there is something, return true

		int result = 0;	// no shadowed

		for (auto objIter = objects.begin(); objIter != objects.end(); ++objIter)
		{
			// if any object block this light source, in medium will not block by medium itself 
			float distance = (*objIter)->getIntersection(intersectin.intersectionPoint, lightDirection, isInMedium);

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


	float getNearestObject(const Point3 &emitPoint, const Vec3 &rayVec, bool isInMedium, Object *castObj, Intersection &firstIntersection)
	{
		// rayDirect is always normalized

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

				firstIntersection.intersectionPoint = emitPoint + rayVec * intersectionDistance;
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


	float getNearestLight(const Point3 &emitPoint, const Vec3 &rayVec, Light *&light)
	{
		// rayDirect is always normalized

		float firstIntersectionDistance = FLT_MAX;		// The most near intersection distance
		bool isFound = false;							// If we got any intersection

		for (auto lightIter = lights.begin(); lightIter != lights.end(); ++lightIter)
		{
			// Get all intersection and then calculate distance
			VolumnLight *vLight = static_cast<VolumnLight *>((*lightIter).get());

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


	void directLightColour(const Intersection &intersection, const Vec3 &rayVec, bool isInMedium, Color &accumulateLightColor)
	{

		Vec3 normVector(0, 0, 0);
		intersection.obj->getNormVecAt(intersection.intersectionPoint, normVector);

		for (auto lightIter = lights.begin(); lightIter != lights.end(); ++lightIter)
		{
			Color lightBuffer(0, 0, 0);

			VolumnLight *vLight = static_cast<VolumnLight *>((*lightIter).get());

			int sampleTime = 10;

			Vec3 lightDirection(0, 0, 0);

			Vec3 lightCenterDirection = vLight->position - intersection.intersectionPoint;
			lightCenterDirection.normalize();

			for (int i = 0; i < sampleTime; ++i)
			{
				// Only process direct reflactor(illuminate by light source)
				float lightSourceDistance = vLight->sampleRayVec(intersection.intersectionPoint, lightDirection);

				int shadowState = isShadow(lightDirection, lightSourceDistance, intersection, isInMedium);

				if (shadowState == 0 || shadowState == -1)
				{
					Color lightColor = (*lightIter)->getLightStrength(lightDirection, lightSourceDistance, normVector);
					lightColor *= intersection.obj->getDiffuseFactor() * normVector.dot(lightDirection);

					lightBuffer += lightColor;

				}
			}
			//lightBuffer *= vLight->getSampleRatio(intersection.intersectionPoint);
			lightBuffer /= sampleTime;

			accumulateLightColor += lightBuffer;
		}

		accumulateLightColor += ambientLight;
	}


	void deffuseMonteCarlo(const Intersection &intersection, const Vec3 &rayVec,  bool isInMedium, int nowDepth, Color &reflectionColor)
	{
		static std::default_random_engine e;
		static std::uniform_real_distribution<float> u(-1, 1);

		Color diffuseColor(0, 0, 0);
		Vec3 p(0, 0, 0);
		Vec3 norm(0, 0, 0);

		for (int i = 0; i < 100; ++i) {
			intersection.obj->getNormVecAt(intersection.intersectionPoint, norm);

			float rayVecDot = rayVec * rayVec;
			float rayVecLength = rayVec.length();

			float targetCosAngle = 1.0f - (((float)rand() / (float)RAND_MAX ) / 2.0f * intersection.obj->getDiffuseFactor());

			// vector create referer to https://math.stackexchange.com/questions/2464998/random-vector-with-fixed-angle

			//p.x = u(e);
			//p.y = u(e);
			//p.z = u(e);

			p.x = (float)rand() / (float)RAND_MAX * 2.0f - 1.0f;
			p.y = (float)rand() / (float)RAND_MAX * 2.0f - 1.0f;
			p.z = (float)rand() / (float)RAND_MAX * 2.0f - 1.0f;


			p -= rayVec * ((p * rayVec) / rayVecDot);

			p /= p.length();
			p *= rayVecLength;

			Vec3 v = rayVec * targetCosAngle;
			p *= sqrtf(1.0f - targetCosAngle * targetCosAngle);
			v += p;

			castTraceRay(intersection.intersectionPoint, v, intersection.obj, isInMedium, nowDepth - 1, diffuseColor);

		}

		reflectionColor += diffuseColor / 100.0f;
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
		Light *nearestLightSource;

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
		Vec3 refractionRayDirect(0, 0, 0);
		bool totalReflection = nearestObjectIntersection.obj->calcRefractionRay(nearestObjectIntersection.intersectionPoint, rayDirect, rayInMedium, refractionRayDirect);

		if (!totalReflection)
		{
			// Calculate refraction
			Color refractionColor(0, 0, 0);
			castTraceRay(nearestObjectIntersection.intersectionPoint, refractionRayDirect, nearestObjectIntersection.obj, !rayInMedium, nowDepth - 1, refractionColor);

			refractionColor *= nearestObjectIntersection.obj->getRefractionRatio(nearestObjectIntersection.intersectionPoint);

			light += refractionColor;
		}

		/*
		Step 4:
			Calculate all reflection ray and recursion trace
		*/
		Vec3 mainReflectionRayDirect(0, 0, 0);
		nearestObjectIntersection.obj->calcReflectionRay(nearestObjectIntersection.intersectionPoint, rayDirect, mainReflectionRayDirect);

		Color reflectionColor(0, 0, 0);

#ifdef USE_MC_REFLECT
		if (nowDepth == traceDepth)
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

	void addObject(Object *obj) 
	{
		objects.emplace_back(obj);
	}


	void addLightSource(Light *light)
	{
		lights.emplace_back(light);
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

						if (y % (space * 2) == 0 && x % (space * 2) != 0)
						{
							colorLhs = bitmap[(x - space) + y * w];
							colorRhs = bitmap[(x + space) + y * w];
						}
						else if (y % (space * 2) != 0 && x % (space * 2) == 0)
						{
							colorLhs = bitmap[x + (y - space) * w];
							colorRhs = bitmap[x + (y + space) * w];
						}
						else if (y % (space * 2) != 0 && x % (space * 2) != 0)
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
							bitmap[x + y * w] = Color((lr + rr) / 2, (lg + rg) / 2, (lb + rb) / 2).getColor();
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

	std::vector<std::shared_ptr<Object>> objects;						// store all objects
	std::vector<std::shared_ptr<Light>> lights;							// store all light
};

Tracer::Tracer()
{
}

Tracer::~Tracer()
{
}