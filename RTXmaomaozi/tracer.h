#pragma once

#include "vec.h"
#include "object.h"
#include "light.h"
#include "camera.h"


#define USE_MC_REFLECT 0

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


	void phongLightColour(const Intersection &intersection, const Vec3 &rayVec, bool isInMedium, Color &accumulateLightColor)
	{

		Vec3 normVector(0, 0, 0);
		intersection.obj->getNormVecAt(intersection.intersectionPoint, normVector);

		for (auto lightIter = lights.begin(); lightIter != lights.end(); ++lightIter)
		{
			Color lightBuffer(0, 0, 0);

			int sampleTime = 0;

			VolumnLight *vLight = dynamic_cast<VolumnLight *>((*lightIter).get());
			sampleTime = 20;

			Vec3 lightDirection(0, 0, 0);

			Vec3 lightCenterDirection = vLight->position - intersection.intersectionPoint;
			lightCenterDirection.normalize();

			for (int i = 0; i < sampleTime; ++i)
			{
				// Only process direct reflactor(illuminate by light source)
				float lightSourceDistance;

				if (vLight)
				{
					lightSourceDistance = vLight->sampleRayVec(intersection.intersectionPoint, lightDirection);
				}
				else
				{
					lightDirection = (*lightIter)->position - intersection.intersectionPoint;
					lightSourceDistance = lightDirection.length();
				}

				lightDirection.normalize();

				int shadowState = isShadow(lightDirection, lightSourceDistance, intersection, isInMedium);

				if (shadowState == 0 || shadowState == -1)
				{
					float angleDiffuseCos = normVector * lightDirection;
					float angleReflectCos = powf(rayVec * lightCenterDirection, 29);
					angleReflectCos = powf(angleReflectCos, 29);

					if (angleDiffuseCos < 0.0f) angleDiffuseCos = -angleDiffuseCos;
					if (angleReflectCos < 0.0f) angleReflectCos = 0.0f;

					Color lightColor = (*lightIter)->getLightStrength(lightDirection, lightSourceDistance, normVector);

					// come from diffuse
					lightBuffer += lightColor * angleDiffuseCos * intersection.obj->getDiffuseFactor();

					// come from real reflector
					lightBuffer += lightColor * angleReflectCos * (1 - intersection.obj->getDiffuseFactor());
				}
			}

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

		intersection.obj->getNormVecAt(intersection.intersectionPoint, norm);

		float ratio = acosf(-2.0f * intersection.obj->getDiffuseFactor() + 1.0f) / PI;

		int counter = 0;
		for (int i = 0; i < 200 * ratio; ++i)
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
			castTraceRay(intersection.intersectionPoint, v, intersection.obj, isInMedium, nowDepth - 1, diffuseColor);

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

		if ((USE_MC_REFLECT || emitObject == nullptr) && lightDistance != NO_INTERSECTION &&
			(objDistance == NO_INTERSECTION || objDistance > lightDistance))
		{
			//light += nearestLightSource->getLightStrength(rayDirect, lightDistance, rayDirect);
		}


		if (objDistance == NO_INTERSECTION)
		{
			if (emitObject == nullptr)
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

		if (USE_MC_REFLECT && nowDepth >= traceDepth)
		{
			// Use accurate reflect model, monte-carlo simulation of diffuse
			deffuseMonteCarlo(nearestObjectIntersection, mainReflectionRayDirect, rayInMedium, nowDepth, reflectionColor);
		}
		else
		{
			castTraceRay(nearestObjectIntersection.intersectionPoint, mainReflectionRayDirect, nearestObjectIntersection.obj, rayInMedium, nowDepth - 1, reflectionColor);
			// if object is diffuse, direct reflector will have less weight
			reflectionColor *= (1 - nearestObjectIntersection.obj->getDiffuseFactor());

			phongLightColour(nearestObjectIntersection, mainReflectionRayDirect, rayInMedium, reflectionColor);
		}

		if (totalReflection)
		{
			reflectionColor *= nearestObjectIntersection.obj->getTotalReflectionRatio(nearestObjectIntersection.intersectionPoint);
		}
		else
		{
			reflectionColor *= nearestObjectIntersection.obj->getReflectionRatio(nearestObjectIntersection.intersectionPoint);

			
			if (USE_MC_REFLECT && nowDepth >= traceDepth)
			{
				;
			}
			else 
			{
				if (rayInMedium)
				{
					Color refractLightSource(0, 0, 0);
					phongLightColour(nearestObjectIntersection, refractionRayDirect, rayInMedium, refractLightSource);

					refractLightSource *= nearestObjectIntersection.obj->getRefractionRatio(nearestObjectIntersection.intersectionPoint);
					light += refractLightSource;
				}
			}
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


	void trace(Camera camera, size_t traceDepth, Color backgroundColor, Color ambientLight, int antiAliasScale, UINT32 *bitmap)
	{
		this->backgroundColor = backgroundColor;
		this->ambientLight = ambientLight;
		this->traceDepth = traceDepth;

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

					// calculate sub pixel for anti-alias
					for (int subY = 0; subY < antiAliasScale; ++subY) 
					{
						for (int subX = 0; subX < antiAliasScale; ++subX) 
						{
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
	int traceDepth;

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