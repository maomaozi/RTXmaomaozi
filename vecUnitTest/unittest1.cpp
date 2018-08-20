#include "stdafx.h"
#include "CppUnitTest.h"

#include "../RTXmaomaozi/vec.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


template <> inline std::wstring Microsoft::VisualStudio::CppUnitTestFramework::ToString<vec3>(const vec3& q)
{
	WCHAR tmp[100];
	swprintf(tmp, L"V(%.3f, %.3f, %.3f)", q.x, q.y, q.z);

	return std::wstring(tmp);
}


template <> inline std::wstring Microsoft::VisualStudio::CppUnitTestFramework::ToString<point3>(const point3& q)
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
			vec3 v1(1, 2, 3);
			vec3 v2(1, 2, 3);
			vec3 v3(-1, 2, 3);
			vec3 v4(1, 1, 3);
			vec3 v5(0, 0, 0);
			vec3 v6(0, 0, 0);

			Assert::AreEqual(v1, v2);
			Assert::AreEqual(v5, v6);
			Assert::AreNotEqual(v1, v3);
			Assert::AreNotEqual(v1, v4);
			Assert::AreNotEqual(v2, v3);

		}


		TEST_METHOD(TestVec3Add)
		{
			Assert::AreEqual(vec3(2, 4, 6), vec3(1, 2, 3) + vec3(1, 2, 3));
			Assert::AreEqual(vec3(1, 2, 3), vec3(0, 0, 0) + vec3(1, 2, 3));
			Assert::AreEqual(vec3(0, 0, 0), vec3(1, 2, 3) + vec3(-1, -2, -3));
		}


		TEST_METHOD(TestVec3Sub)
		{
			vec3 v1(1, 2, 3);
			vec3 v2 = v1 + vec3(1, 2, 3);

			Assert::AreEqual(vec3(1, 2, 3), v1);
			Assert::AreEqual(vec3(0, 0, 0), vec3(1, 2, 3) - vec3(1, 2, 3));
			Assert::AreEqual(vec3(-1, -2, -3), vec3(0, 0, 0) - vec3(1, 2, 3));
			Assert::AreEqual(vec3(2, 4, 6), vec3(1, 2, 3) - vec3(-1, -2, -3));
		}


		TEST_METHOD(TestVec3AddEqual)
		{
			vec3 v1(1, 2, 3);
			v1 += vec3(1, 2, 3);

			Assert::AreEqual(vec3(2, 4, 6), v1);
		}


		TEST_METHOD(TestVec3SubEqual)
		{
			vec3 v1(1, 2, 3);
			v1 -= vec3(2, 4, 6);

			Assert::AreEqual(vec3(-1, -2, -3), v1);
		}


		TEST_METHOD(TestVec3Negtive)
		{
			vec3 v1(1, 2, 3);
			
			Assert::AreEqual(vec3(-1, -2, -3), -v1);
			Assert::AreEqual(vec3(1, 2, 3), v1);
		}


		TEST_METHOD(TestVec3Dot)
		{
			Assert::AreEqual(vec3(1, 2, 3) * vec3(-1, 2, 0), 3.0f);
			Assert::AreEqual(vec3(1, 2, 3) * vec3(0, 0, 0), 0.0f);
		}


		TEST_METHOD(TestVec3Xmul)
		{
			Assert::AreEqual(vec3(-6, -3, 4), vec3(1, 2, 3).xmul(vec3(-1, 2, 0)));
			Assert::AreEqual(vec3(1, 2, 3).xmul(vec3(-1, 2, 0)), (-vec3(-1, 2, 0)).xmul(vec3(1, 2, 3)));
			Assert::AreEqual(vec3(0, 0, 0), vec3(1, 2, 3).xmul(vec3(0, 0, 0)));
		}


		TEST_METHOD(TestVec3Length)
		{
			Assert::AreEqual(1.0f, vec3(1, 0, 0).length());
			Assert::AreEqual(0.0f, vec3(0, 0, 0).length());
			Assert::AreEqual(1.0f, vec3(-1, 0, 0).length());
			Assert::IsTrue(vec3(1, 2, 0).length() - sqrtf(5) < 0.01);
		}


		TEST_METHOD(TestVec3Normalize)
		{
			vec3 v1(1, 2, 3);
			v1.normalize();

			Assert::AreEqual(vec3(1, 2, 3), v1);
			Assert::IsTrue((vec3(1, 0, 0).normalize() - vec3(1 ,0, 0)).length() < 0.0001f);
			Assert::IsTrue((vec3(1, 2, 2).normalize() - vec3(1 / 3.0f, 2 / 3.0f, 2 / 3.0f)).length() < 0.0001f);
		}


		TEST_METHOD(TestVec3Angel)
		{
			Assert::AreEqual(0.0f, vec3(0, 0, 0).angle(vec3(0, 0, 0)));
			Assert::AreEqual(0.0f, vec3(1, 1, 0).angle(vec3(1, 1, 0)));
			Assert::AreEqual(PI / 2, vec3(1, 0, 0).angle(vec3(0, 1, 0)));
			Assert::AreEqual(PI / 2, vec3(1, 0, 0).angle(vec3(0, 0, 1)));
			Assert::AreEqual(PI / 2, vec3(0, 1, 0).angle(vec3(1, 0, 0)));
		}
	};



	TEST_CLASS(Point3UnitTest)
	{
	public:
		TEST_METHOD(TestPoint3Distance)
		{
			Assert::AreEqual(0.0f, point3(1, 0, 0).distance(point3(1, 0, 0)));
			Assert::AreEqual(1.0f, point3(1, 0, 0).distance(point3(0, 0, 0)));
		}


		TEST_METHOD(TestPoint3AddVec3)
		{
			Assert::AreEqual(point3(1, 2, -1), point3(1, 0, 0) + vec3(0, 2, -1));
		}


		TEST_METHOD(TestPoint3SubVec3)
		{
			Assert::AreEqual(point3(1, -2, 1), point3(1, 0, 0) - vec3(0, 2, -1));
		}


		TEST_METHOD(TestPoint3CalcVec3)
		{
			Assert::AreEqual(vec3(1, -2, 1), point3(1, 0, 0) - point3(0, 2, -1));
		}
	};
}