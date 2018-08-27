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


	void getDirectLight(const Intersection &intersection, const Vec3 &rayVec, const Vec3 &normVector, bool isInMedium, Color &accumulateLightColor)
	{
		for (auto lightIter = lights.begin(); lightIter != lights.end(); ++lightIter)
		{
			// Only process direct reflactor(illuminate by light source)

			Vec3 lightDirection = (*lightIter)->position - intersection.entryPoint;
			float lightLength = lightDirection.length();

			lightDirection.normalize();

			int shadowState = isShadow(lightDirection, lightLength, intersection, isInMedium);

			if (shadowState == 0 || shadowState == -1) {

				float angleDiffuseCos = normVector * lightDirection;
				float angleReflectCos = powf(rayVec * lightDirection, 29);
				angleReflectCos = powf(angleReflectCos, 29);

				if (angleDiffuseCos < 0.0f) angleDiffuseCos = 0.0f;
				if (angleReflectCos < 0.0f) angleReflectCos = 0.0f;

				Color lightColor = (*lightIter)->getLightStrength(lightDirection, lightLength, normVector);

				// come from diffuse
				accumulateLightColor += lightColor * angleDiffuseCos * intersection.obj->getDiffuseFactor();

				// come from real reflector
				accumulateLightColor += lightColor * angleReflectCos * (1 - intersection.obj->getDiffuseFactor());

			}

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


	void deffuseMonteCarlo(const Intersection &firstIntersection, const Vec3 &rayVec,  bool isInMedium, int nowDepth, Color &reflectionColor)
	{
		Color diffuseColor(0, 0, 0);
		Vec3 p(0, 0, 0);
		for (int i = 0; i < 500; ++i)
		{
			// vector create referer to https://math.stackexchange.com/questions/2464998/random-vector-with-fixed-angle

			float targetCosAngel = 1.0f - rand() % (int)(firstIntersection.obj->getDiffuseFactor() * 100000) / 100000.0f;

			p.x = (rand() % 10000 - 5000) / 1000.0f;
			p.y = (rand() % 10000 - 5000) / 1000.0f;
			p.z = (rand() % 10000 - 5000) / 1000.0f;

			p -= rayVec * ((p * rayVec) / (rayVec * rayVec));

			p /= p.length();
			p *= rayVec.length();

			Vec3 v = rayVec * targetCosAngel;
			p *= sqrtf(1.0f - targetCosAngel * targetCosAngel);
			v += p;

			diffuseColor.r = diffuseColor.g = diffuseColor.b = 0.0f;
			castTraceRay(firstIntersection.entryPoint, v, firstIntersection.obj, isInMedium, nowDepth - 1, diffuseColor);

			diffuseColor *= targetCosAngel;
			reflectionColor += diffuseColor;
		}

		reflectionColor /= 500.0f;
	}


	// Cast a ray to object and add the light of it on color parameter
	void castTraceRay(const Point3 &emitPoint, const Vec3 &rayVec, Object *castObj, bool isInMedium, int nowDepth, Color &light)
	{
		// Step 1:	determine if depth reach max trace depth
		if (nowDepth <= 0)
		{
			return;		// Stop iter
		}

		// Step 2:	Go through each object and calculate intersection
		//			find which intersection is most near the emitPoint
		//			if there is no intersection, return background color
		Intersection firstIntersection;

		if (!getFirstIntersection(emitPoint, rayVec, isInMedium, castObj, firstIntersection))
		{
			// No intersection, return background color
			if (castObj == nullptr)
			{
				light = backgroundColor;
				return;
			}
			else 
			{
				return;
			}
		}

		Color castOnColor(0, 0, 0);			// Direct and indirect light shade on this object

		// Step 3:	Process reflection, calculate reflection ray and recursion trace
		Vec3 reflectionRay(0, 0, 0);
		firstIntersection.obj->calcReflectionRay(firstIntersection.entryPoint, rayVec, reflectionRay);

		Color reflectionColor(0, 0, 0);

		// If object deffuse light 
		if (firstIntersection.obj->getDiffuseFactor() < 0.001f || castObj != nullptr)
		{
			castTraceRay(firstIntersection.entryPoint, reflectionRay, firstIntersection.obj, isInMedium, nowDepth - 1, reflectionColor);

			// if object is diffuse, direct reflector will have less weight
			reflectionColor *= (1 - firstIntersection.obj->getDiffuseFactor());

			castOnColor += reflectionColor;
		}
		else 
		{
			// monte-carlo simulation of diffuse
			// we only do this for first level trace 
			deffuseMonteCarlo(firstIntersection, reflectionRay, isInMedium, nowDepth, reflectionColor);
			castOnColor += reflectionColor;
		}

		// Step 4:	Process refraction, calculate refraction ray and recursion trace
		//			If total reflection happend, no need to calculate refraction
		Vec3 refractionRay(0, 0, 0);
		bool totalReflection = firstIntersection.obj->calcRefractionRay(firstIntersection.entryPoint, rayVec, refractionRay, isInMedium);


		// Step 5:	cast shadow ray to light source
		//			if shadow ray has intersection with object
		//			(some thing between point and light source), then cast shadow
		//			otherwise collect each light strength and contribute to color

		// Cast shadow to every light source

		Vec3 norm(0, 0, 0);
		firstIntersection.obj->getNormVecAt(firstIntersection.entryPoint, norm);

		if(!isInMedium)
			getDirectLight(firstIntersection,  reflectionRay, norm, isInMedium, castOnColor);

		// Step 6: Process reflect color
		if (!totalReflection)
		{
			if(isInMedium)
				getDirectLight(firstIntersection, refractionRay, norm, isInMedium, castOnColor);

			// castOnColor will be never modify again
			castOnColor *= isInMedium ? 
				firstIntersection.obj->getRefractionRatio(firstIntersection.entryPoint) :
				firstIntersection.obj->getReflectionRatio(firstIntersection.entryPoint);

			light += castOnColor;

			// then add refraction of other object
			Color refractionColor(0, 0, 0);
			castTraceRay(firstIntersection.entryPoint, refractionRay, firstIntersection.obj, !isInMedium, nowDepth - 1, refractionColor);

			refractionColor *= firstIntersection.obj->getRefractionRatio(firstIntersection.entryPoint);
		
			light += refractionColor;
		}
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