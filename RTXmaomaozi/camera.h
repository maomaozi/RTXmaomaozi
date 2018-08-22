#pragma once
#include "vec.h"

class Camera
{
public:
	Camera(const Point3 &cameraPosition, const Vec3 &verticalVec, const Vec3 &horizonVec, float width, float height, float screenDist) :
		verticalVec(verticalVec.normalize()), horizonVec(horizonVec.normalize()),
		width(width), height(height),
		screenDist(screenDist),
		cameraPosition(cameraPosition),
		screenCenter(cameraPosition + verticalVec * (height / 2) + horizonVec * (width / 2)),
		viewPoint(screenCenter + verticalVec.xmul(horizonVec).normalize() * screenDist)
	{
		;
	}

	// (right-clockwise) for positive angel
	void turnByX(float angel)
	{
		turnVecByVec(verticalVec, horizonVec, angel);

		screenCenter = cameraPosition + verticalVec * (height / 2) + horizonVec * (width / 2);

		viewPoint = screenCenter + verticalVec.xmul(horizonVec).normalize() * screenDist;
	}

	// top-clockwise for positive angel
	void turnByY(float angel)
	{

		turnVecByVec(horizonVec, verticalVec, angel);

		screenCenter = cameraPosition + verticalVec * (height / 2) + horizonVec * (width / 2);

		viewPoint = screenCenter + verticalVec.xmul(horizonVec).normalize() * screenDist;
	}


	void moveX(float offset) {
		cameraPosition.x += offset;
		screenCenter.x += offset;
		viewPoint.x += offset;
	}


	void moveY(float offset) {
		cameraPosition.y += offset;
		screenCenter.y += offset;
		viewPoint.y += offset;
	}


	void moveZ(float offset) {
		cameraPosition.z += offset;
		screenCenter.z += offset;
		viewPoint.z += offset;
	}

	void setPosition(const Point3 &newPosition)
	{
		cameraPosition = newPosition;
		screenCenter = cameraPosition + verticalVec * (height / 2) + horizonVec * (width / 2);
		viewPoint = screenCenter + verticalVec.xmul(horizonVec).normalize() * screenDist;
	}

	Point3 rasterization(float x, float y)
	{
		return cameraPosition + verticalVec * y  + horizonVec * x;
	}

	Vec3 getViewRay(float x, float y)
	{
		return (rasterization(x, y) - viewPoint).normalize();
	}

	Point3 getViewPoint()
	{
		return viewPoint;
	}

	float getWidth()
	{
		return width;
	}

	float getHeight()
	{
		return height;
	}

private:
	void turnVecByVec(Vec3 &vec1, const Vec3 &vec2, float angel)
	{
		vec1 = vec1 * cosf(angel) + (vec2.xmul(vec1)) * sinf(angel) + vec2 * (vec2 * vec1) * (1 - cosf(angel));
	}

private:

	float width;
	float height;
	float screenDist;

	Vec3 verticalVec;
	Vec3 horizonVec;

	Point3 cameraPosition;
	Point3 screenCenter;
	Point3 viewPoint;
};