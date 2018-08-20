#pragma once

class Object;


struct Intersection
{
	Intersection(point3 entryPoint, Object *obj) :
		entryPoint(entryPoint), 
		obj(obj)
	{
		;
	}

	Intersection() :
		entryPoint(0, 0, 0),
		obj(nullptr)
	{
		;
	}

	point3 entryPoint;
	Object *obj;
};



class Object
{

public:
	virtual bool getIntersection(const point3 &emitPoint, const vec3 &rayVec, point3 &IntersectionPoint) = 0;
	virtual bool getIntersection(const point3 &emitPoint, const point3 &endPoint, point3 &IntersectionPoint) = 0;

	virtual float calcReflectionRay(const point3 &emitPoint, const vec3 &rayVec, vec3 &reflectionRay) = 0;
	virtual float calcRefractionRay(const point3 &emitPoint, const vec3 &rayVec, vec3 &refractionRay) = 0;

	float getReflectionFactor()
	{
		return reflectionFactor;
	}

	float getRefractionFactor()
	{
		return refractionFactor;
	}

private:
	float reflectionFactor;
	float refractionFactor; 
};



class sphere : public Object 
{
public:
	bool getIntersection(const point3 &emitPoint, const vec3 &rayVec, point3 &IntersectionPoint)
	{
		return false;
	}

	bool getIntersection(const point3 &emitPoint, const point3 &endPoint, point3 &IntersectionPoint)
	{
		return false;
	}

	float calcReflectionRay(const point3 &emitPoint, const vec3 &rayVec, vec3 &reflectionRay)
	{
		return 0.0f;
	}

	float calcRefractionRay(const point3 &emitPoint, const vec3 &rayVec, vec3 &refractionRay)
	{
		return 0.0f;
	}
};