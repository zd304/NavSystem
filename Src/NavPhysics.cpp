#include "NavPhysics.h"
#include "NavTriangle.h"

#ifdef _CHECK_LEAK
#define new  new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

namespace NavPhysics
{
	bool RayIntersectTriangle(const Nav::Vector3& orig, const Nav::Vector3& dir,
		const Nav::Vector3& v0, const Nav::Vector3& v1, const Nav::Vector3& v2,
		NavHit* hit)
	{
		// Find vectors for two edges sharing vert0
		Nav::Vector3 edge1 = v1 - v0;
		Nav::Vector3 edge2 = v2 - v0;

		// Begin calculating determinant - also used to calculate U parameter
		Nav::Vector3 pvec;
		Nav::Vector3::Vector3Cross(pvec, dir, edge2);

		// If determinant is near zero, ray lies in plane of triangle
		float det = Nav::Vector3::Vector3Dot(pvec, edge1);
		Nav::Vector3 tvec;
		if (det > 0)
		{
			tvec = orig - v0;
		}
		else
		{
			tvec = v0 - orig;
			det = -det;
		}

		if (det < 0.0001f)
		{
			return false;
		}

		// Calculate U parameter and test bounds
		float u = Nav::Vector3::Vector3Dot(pvec, tvec);
		if (u < 0.0f || u > det)
		{
			return false;
		}

		// Prepare to test V parameter
		Nav::Vector3 qvec;
		Nav::Vector3::Vector3Cross(qvec, tvec, edge1);

			// Calculate V parameter and test bounds
		float v = Nav::Vector3::Vector3Dot(qvec, dir);
		if (v < 0.0f || u + v > det)
		{
			return false;
		}

		// Calculate t, scale parameters, ray intersects triangle
		float t = Nav::Vector3::Vector3Dot(qvec, edge2);
		float fInvDet = 1.0f / det;
		t *= fInvDet;

		if (hit)
		{
			hit->distance = t;
			hit->hitPoint = orig + dir * t;
		}

		return t > 0.f;
	}

	bool RayIntersectSegment2D(const Nav::Vector2& orig, const Nav::Vector2& dir,
		const Nav::Vector2& lineBegin, const Nav::Vector2& lineEnd,
		NavHit* hit)
	{
		// 射线方向向量不能为零向量;
		if (dir == Nav::Vector2::ZERO)
		{
			return false;
		}
		//线段两端点不能相同;
		if (lineBegin == lineEnd)
		{
			return false;
		}

		//记线段两端点为A(Xa, Ya) B(Xb, Yb)，所在直线为AB;
		const float Xa = lineBegin.x, Ya = lineBegin.y, Xb = lineEnd.x, Yb = lineEnd.y;
		//记射线起点为C(Xc, Yc);
		const float Xc = orig.x, Yc = orig.y;
		//记射线方向向量为dir(dirx,diry);
		const float dirx = dir.x, diry = dir.y;
		//射线方向向量、线段所在直线的方向向量平行;
		//rayDirection.Cross(lineAB) == 0
		//时，没有交点;
		Nav::Vector2 lAB = lineEnd - lineBegin;

		float tmp = dir.x * lAB.y - dir.y * lAB.x;
		if (tmp == 0.0f)
			return false;
		//由A + k·AB = C + d·dir解出λ，其中A AB C dir均为向量，k d为数值;
		float d = (Xa * Yb - Xb * Ya + Yc * lAB.x - Xc * lAB.y) / tmp;
		if (d < 0.0f || d > 1.0f) // 不相交 没有撞击点;
		{
			return false;
		}

		Nav::Vector2 intersection = orig + dir * d;
		Nav::Vector2 lAI = intersection - lineBegin;
		float k = lAI.Dot(lAB) / lAB.LengthSquared();
		if (k < 0.0f || k > 1.0f)
			return false;
		if (hit)
		{
			hit->distance = d;
			hit->hitPoint = intersection;
		}
		return true;
	}

	bool TriangleAABBPoint2D(const Nav::Vector2& v0, const Nav::Vector2& v1, const Nav::Vector2& v2,
		const Nav::Vector2& point2D)
	{
		float x = point2D.x;
		if (x < v0.x && x < v1.x && x < v2.x)
			return false;
		if (x > v0.x && x > v1.x && x > v2.x)
			return false;

		float y = point2D.y;
		if (y < v0.y && y < v1.y && y < v2.y)
			return false;
		if (y > v0.y && y > v1.y && y > v2.y)
			return false;
		return true;
	}

	bool TriangleAABBPoint2D(const Nav::Vector3& v0, const Nav::Vector3& v1, const Nav::Vector3& v2,
		const Nav::Vector3& point2D)
	{
		float x = point2D.x;
		if (x < v0.x && x < v1.x && x < v2.x)
			return false;
		if (x > v0.x && x > v1.x && x > v2.x)
			return false;

		float z = point2D.z;
		if (z < v0.z && z < v1.z && z < v2.z)
			return false;
		if (z > v0.z && z > v1.z && z > v2.z)
			return false;
		return true;
	}

	bool SegmentAABBSegment2D(const Nav::Vector3& line1Begin, const Nav::Vector3& line1End,
		const Nav::Vector3& line2Begin, const Nav::Vector3& line2End)
	{
		float minx = line1Begin.x > line1End.x ? line1End.x : line1Begin.x;
		if (minx > line2Begin.x && minx > line2End.x)
			return false;
		float maxx = line1Begin.x < line1End.x ? line1End.x : line1Begin.x;
		if (maxx < line2Begin.x && maxx < line2End.x)
			return false;

		float minz = line1Begin.z > line1End.z ? line1End.z : line1Begin.z;
		if (minz > line2Begin.z && minz > line2End.z)
			return false;
		float maxz = line1Begin.z < line1End.z ? line1End.z : line1Begin.z;
		if (maxz < line2Begin.z && maxz < line2End.z)
			return false;
		return true;
	}

	bool IsPointInTriangle(const Nav::Vector2& v0, const Nav::Vector2& v1, const Nav::Vector2& v2, const Nav::Vector2& point)
	{
		Nav::Vector2 e1(v1 - v0);
		Nav::Vector2 e2(v2 - v1);
		Nav::Vector2 e3(v0 - v2);
		float fn = e1.Cross(e3);
		float en;
		en = e1.Cross(v0 - point);
		if (en * fn < 0)
			return false;

		en = e2.Cross(v1 - point);
		if (en * fn < 0)
			return false;

		en = e3.Cross(v2 - point);
		if (en * fn < 0)
			return false;

		return true;
	}

	bool IsPointInTriangle2D(const Nav::Vector3& v0, const Nav::Vector3& v1, const Nav::Vector3& v2, const Nav::Vector3& point)
	{
		Nav::Vector2 e1(v1.x - v0.x, v1.z - v0.z);
		Nav::Vector2 e2(v2.x - v1.x, v2.z - v1.z);
		Nav::Vector2 e3(v0.x - v2.x, v0.z - v2.z);

		Nav::Vector2 p1(v0.x - point.x, v0.z - point.z);
		Nav::Vector2 p2(v1.x - point.x, v1.z - point.z);
		Nav::Vector2 p3(v2.x - point.x, v2.z - point.z);

		float fn = e1.Cross(e3);
		float en;
		en = e1.Cross(p1);
		if (en * fn < 0)
			return false;

		en = e2.Cross(p2);
		if (en * fn < 0)
			return false;

		en = e3.Cross(p3);
		if (en * fn < 0)
			return false;

		return true;
	}

	bool IsSegmentsInterct(const Nav::Vector2& p1, const Nav::Vector2& p2,
		const Nav::Vector2& q1, const Nav::Vector2& q2)
	{
		//跨立判断
		float multi1 = ((q1.x - p1.x)*(q1.y - q2.y) - (q1.y - p1.y)*(q1.x - q2.x)) * ((q1.x - p2.x)*(q1.y - q2.y) - (q1.y - p2.y)*(q1.x - q2.x));
		float multi2 = ((p1.x - q1.x)*(p1.y - p2.y) - (p1.y - q1.y)*(p1.x - p2.x)) * ((p1.x - q2.x)*(p1.y - p2.y) - (p1.y - q2.y)*(p1.x - p2.x));
		if (multi1 >= 0 || multi2 >= 0)
			return false;
		return true;
	}

	bool SegmentIntersectSegment(const Nav::Vector2& p1, const Nav::Vector2& p2,
		const Nav::Vector2& q1, const Nav::Vector2& q2,
		Nav::Vector2* hitPoint)
	{
		//跨立判断
		float multi1 = ((q1.x - p1.x)*(q1.y - q2.y) - (q1.y - p1.y)*(q1.x - q2.x)) * ((q1.x - p2.x)*(q1.y - q2.y) - (q1.y - p2.y)*(q1.x - q2.x));
		float multi2 = ((p1.x - q1.x)*(p1.y - p2.y) - (p1.y - q1.y)*(p1.x - p2.x)) * ((p1.x - q2.x)*(p1.y - p2.y) - (p1.y - q2.y)*(p1.x - p2.x));
		if (multi1 >= 0 || multi2 >= 0)
			return false;

		//求交点
		float tmpLeft, tmpRight;
		tmpLeft = (q2.x - q1.x) * (p1.y - p2.y) - (p2.x - p1.x) * (q1.y - q2.y);
		tmpRight = (p1.y - q1.y) * (p2.x - p1.x) * (q2.x - q1.x) + q1.x * (q2.y - q1.y) * (p2.x - p1.x) - p1.x * (p2.y - p1.y) * (q2.x - q1.x);

		hitPoint->x = tmpRight / tmpLeft;

		tmpLeft = (p1.x - p2.x) * (q2.y - q1.y) - (p2.y - p1.y) * (q1.x - q2.x);
		tmpRight = p2.y * (p1.x - p2.x) * (q2.y - q1.y) + (q2.x - p2.x) * (q2.y - q1.y) * (p1.y - p2.y) - q2.y * (q1.x - q2.x) * (p2.y - p1.y);
		hitPoint->y = tmpRight / tmpLeft;
		return true;
	}

	float CalcTriangleArea2D(const Nav::Vector3& v0, const Nav::Vector3& v1, const Nav::Vector3& v2)
	{
		Nav::Vector2 edge0(v1.x - v0.x, v1.z - v0.z);
		Nav::Vector2 edge1(v2.x - v1.x, v2.z - v1.z);
		Nav::Vector2 edge2(v0.x - v2.x, v0.z - v2.z);

		float a = edge0.Length();
		float b = edge1.Length();
		float c = edge2.Length();

		if ((a + b) <= c || (b + c) <= a || (c + a) <= b)
			return 0.0f;

		float m = (a + b + c) * 0.5f;

		return sqrtf(m * (m - a) * (m - b) * (m - c));
	}
}