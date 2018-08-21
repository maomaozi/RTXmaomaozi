#include "stdafx.h"
#include "CppUnitTest.h"

#include "../RTXmaomaozi/vec.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


template <> inline std::wstring Microsoft::VisualStudio::CppUnitTestFramework::ToString<Vec3>(const Vec3& q)
{
	WCHAR tmp[100];
	swprintf(tmp, L"V(%.3f, %.3f, %.3f)", q.x, q.y, q.z);

	return std::wstring(tmp);
}


template <> inline std::wstring Microsoft::VisualStudio::CppUnitTestFramework::ToString<Point3>(const Point3& q)
{
	WCHAR tmp[100];
	swprintf(tmp, L"P(%.3f, %.3f, %.3f)", q.x, q.y, q.z);

	return std::wstring(tmp);
}


namespace vecUnitTest
{		
	TEST_CLASS(Vec3UnitTest)
	{
	public:

		TEST_METHOD(TestVec3CreateAndCompare)
		{
			Vec3 v1(1, 2, 3);
			Vec3 v2(1, 2, 3);
			Vec3 v3(-1, 2, 3);
			Vec3 v4(1, 1, 3);
			Vec3 v5(0, 0, 0);
			Vec3 v6(0, 0, 0);

			Assert::AreEqual(v1, v2);
			Assert::AreEqual(v5, v6);
			Assert::AreNotEqual(v1, v3);
			Assert::AreNotEqual(v1, v4);
			Assert::AreNotEqual(v2, v3);

		}


		TEST_METHOD(TestVec3Add)
		{
			Assert::AreEqual(Vec3(2, 4, 6), Vec3(1, 2, 3) + Vec3(1, 2, 3));
			Assert::AreEqual(Vec3(1, 2, 3), Vec3(0, 0, 0) + Vec3(1, 2, 3));
			Assert::AreEqual(Vec3(0, 0, 0), Vec3(1, 2, 3) + Vec3(-1, -2, -3));
		}


		TEST_METHOD(TestVec3Sub)
		{
			Vec3 v1(1, 2, 3);
			Vec3 v2 = v1 + Vec3(1, 2, 3);

			Assert::AreEqual(Vec3(1, 2, 3), v1);
			Assert::AreEqual(Vec3(0, 0, 0), Vec3(1, 2, 3) - Vec3(1, 2, 3));
			Assert::AreEqual(Vec3(-1, -2, -3), Vec3(0, 0, 0) - Vec3(1, 2, 3));
			Assert::AreEqual(Vec3(2, 4, 6), Vec3(1, 2, 3) - Vec3(-1, -2, -3));
		}


		TEST_METHOD(TestVec3AddEqual)
		{
			Vec3 v1(1, 2, 3);
			v1 += Vec3(1, 2, 3);

			Assert::AreEqual(Vec3(2, 4, 6), v1);
		}


		TEST_METHOD(TestVec3SubEqual)
		{
			Vec3 v1(1, 2, 3);
			v1 -= Vec3(2, 4, 6);

			Assert::AreEqual(Vec3(-1, -2, -3), v1);
		}


		TEST_METHOD(TestVec3Negtive)
		{
			Vec3 v1(1, 2, 3);
			
			Assert::AreEqual(Vec3(-1, -2, -3), -v1);
			Assert::AreEqual(Vec3(1, 2, 3), v1);
		}


		TEST_METHOD(TestVec3Dot)
		{
			Assert::AreEqual(Vec3(1, 2, 3) * Vec3(-1, 2, 0), 3.0f);
			Assert::AreEqual(Vec3(1, 2, 3) * Vec3(0, 0, 0), 0.0f);
		}


		TEST_METHOD(TestVec3Xmul)
		{
			Assert::AreEqual(Vec3(-6, -3, 4), Vec3(1, 2, 3).xmul(Vec3(-1, 2, 0)));
			Assert::AreEqual(Vec3(1, 2, 3).xmul(Vec3(-1, 2, 0)), (-Vec3(-1, 2, 0)).xmul(Vec3(1, 2, 3)));
			Assert::AreEqual(Vec3(0, 0, 0), Vec3(1, 2, 3).xmul(Vec3(0, 0, 0)));
		}


		TEST_METHOD(TestVec3Length)
		{
			Assert::AreEqual(1.0f, Vec3(1, 0, 0).length());
			Assert::AreEqual(0.0f, Vec3(0, 0, 0).length());
			Assert::AreEqual(1.0f, Vec3(-1, 0, 0).length());
			Assert::IsTrue(Vec3(1, 2, 0).length() - sqrtf(5) < 0.01);
		}


		TEST_METHOD(TestVec3Normalize)
		{
			Vec3 v1(1, 2, 3);
			v1.normalize();

			Assert::AreEqual(Vec3(1, 2, 3), v1);
			Assert::IsTrue((Vec3(1, 0, 0).normalize() - Vec3(1 ,0, 0)).length() < 0.0001f);
			Assert::IsTrue((Vec3(1, 2, 2).normalize() - Vec3(1 / 3.0f, 2 / 3.0f, 2 / 3.0f)).length() < 0.0001f);
		}


		TEST_METHOD(TestVec3Angel)
		{
			Assert::AreEqual(0.0f, Vec3(0, 0, 0).angle(Vec3(0, 0, 0)));
			Assert::AreEqual(0.0f, Vec3(1, 1, 0).angle(Vec3(1, 1, 0)));
			Assert::AreEqual(PI / 2, Vec3(1, 0, 0).angle(Vec3(0, 1, 0)));
			Assert::AreEqual(PI / 2, Vec3(1, 0, 0).angle(Vec3(0, 0, 1)));
			Assert::AreEqual(PI / 2, Vec3(0, 1, 0).angle(Vec3(1, 0, 0)));
		}
	};



	TEST_CLASS(Point3UnitTest)
	{
	public:
		TEST_METHOD(TestPoint3Distance)
		{
			Assert::AreEqual(0.0f, Point3(1, 0, 0).distance(Point3(1, 0, 0)));
			Assert::AreEqual(1.0f, Point3(1, 0, 0).distance(Point3(0, 0, 0)));
		}


		TEST_METHOD(TestPoint3AddVec3)
		{
			Assert::AreEqual(Point3(1, 2, -1), Point3(1, 0, 0) + Vec3(0, 2, -1));
		}


		TEST_METHOD(TestPoint3SubVec3)
		{
			Assert::AreEqual(Point3(1, -2, 1), Point3(1, 0, 0) - Vec3(0, 2, -1));
		}


		TEST_METHOD(TestPoint3CalcVec3)
		{
			Assert::AreEqual(Vec3(1, -2, 1), Point3(1, 0, 0) - Point3(0, 2, -1));
		}
	};
}