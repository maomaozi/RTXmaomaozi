#pragma once
#include "vec.h"

class Camera
{
public:
	Camera(const Point3 &cameraPosition, const Vec3 &verticalVec, const Vec3 &horizonVec, float width, float height, float screenDist) :
		_verticalVec(verticalVec), _horizonVec(horizonVec),
		width(width), height(height),
		screenDist(screenDist),
		cameraPosition(cameraPosition),
		screenCenter(cameraPosition + _verticalVec * (height / 2) + _horizonVec * (width / 2)),
		viewPoint(0,0,0)
	{
		_verticalVec.normalize();
		_horizonVec.normalize();

		Vec3 norm = _verticalVec.xmul(_horizonVec);
		norm.normalize();

		viewPoint = screenCenter + norm * screenDist;
	}

	Vec3 getViewPlaneNorm()
	{
		Vec3 norm = _verticalVec.xmul(_horizonVec);
		norm.normalize();
		return norm;
	}

	// (right-clockwise) for positive angel
	void turnByX(float angel)
	{
		turnVecByVec(_verticalVec, _horizonVec, angel);

		screenCenter = cameraPosition + _verticalVec * (height / 2) + _horizonVec * (width / 2);

		Vec3 norm = _verticalVec.xmul(_horizonVec);
		norm.normalize();

		viewPoint = screenCenter + norm * screenDist;
	}

	// top-clockwise for positive angel
	void turnByY(float angel)
	{

		turnVecByVec(_horizonVec, _verticalVec, angel);

		screenCenter = cameraPosition + _verticalVec * (height / 2) + _horizonVec * (width / 2);

		Vec3 norm = _verticalVec.xmul(_horizonVec);
		norm.normalize();

		viewPoint = screenCenter + norm * screenDist;
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
		screenCenter = cameraPosition + _verticalVec * (height / 2) + _horizonVec * (width / 2);

		Vec3 norm = _verticalVec.xmul(_horizonVec);
		norm.normalize();

		viewPoint = screenCenter + norm * screenDist;
	}

	Point3 rasterization(float x, float y)
	{
		auto a = cameraPosition + _horizonVec;
		return cameraPosition + _verticalVec * y  + _horizonVec * x;
	}

	Vec3 getViewRay(float x, float y)
	{
		Vec3 viewRay = rasterization(x, y) - viewPoint;

		viewRay.normalize();

		return viewRay;
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

	Vec3 _verticalVec;
	Vec3 _horizonVec;

	Point3 cameraPosition;
	Point3 screenCenter;
	Point3 viewPoint;
};