#include "NavPhysics.h"
#include "NavTriangle.h"

namespace NavPhysics
{
	bool RayIntersectTriangle(const Vector3& orig, const Vector3& dir,
		const Vector3& v0, const Vector3& v1, const Vector3& v2,
		NavHit* hit)
	{
		// Find vectors for two edges sharing vert0
		Vector3 edge1 = v1 - v0;
		Vector3 edge2 = v2 - v0;

		// Begin calculating determinant - also used to calculate U parameter
		Vector3 pvec;
		Vector3::Vector3Cross(pvec, dir, edge2);

		// If determinant is near zero, ray lies in plane of triangle
		float det = Vector3::Vector3Dot(pvec, edge1);
		Vector3 tvec;
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
		float u = Vector3::Vector3Dot(pvec, tvec);
		if (u < 0.0f || u > det)
		{
			return false;
		}

		// Prepare to test V parameter
		Vector3 qvec;
		Vector3::Vector3Cross(qvec, tvec, edge1);

			// Calculate V parameter and test bounds
		float v = Vector3::Vector3Dot(qvec, dir);
		if (v < 0.0f || u + v > det)
		{
			return false;
		}

		// Calculate t, scale parameters, ray intersects triangle
		float t = Vector3::Vector3Dot(qvec, edge2);
		float fInvDet = 1.0f / det;
		t *= fInvDet;

		if (hit)
		{
			hit->distance = t;
			hit->hitPoint = orig + dir * t;
		}

		return t > 0.f;
	}

	bool RayIntersectSegment2D(const Vector2& orig, const Vector2& dir,
		const Vector2& lineBegin, const Vector2& lineEnd,
		NavHit* hit)
	{
		// 射线方向向量不能为零向量;
		if (dir == Vector2::ZERO)
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
		Vector2 lAB = lineEnd - lineBegin;

		float tmp = dir.x * lAB.y - dir.y * lAB.x;
		if (tmp == 0.0f)
			return false;
		//由A + k·AB = C + d·dir解出λ，其中A AB C dir均为向量，k d为数值;
		float d = (Xa * Yb - Xb * Ya + Yc * lAB.x - Xc * lAB.y) / tmp;
		if (d < 0.0f || d > 1.0f) // 不相交 没有撞击点;
		{
			return false;
		}

		Vector2 intersection = orig + dir * d;
		Vector2 lAI = intersection - lineBegin;
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

	bool TriangleAABBPoint2D(const Vector2& v0, const Vector2& v1, const Vector2& v2,
		const Vector2& point2D)
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

	bool TriangleAABBPoint2D(const Vector3& v0, const Vector3& v1, const Vector3& v2,
		const Vector3& point2D)
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

	bool SegmentAABBSegment2D(const Vector3& line1Begin, const Vector3& line1End,
		const Vector3& line2Begin, const Vector3& line2End)
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
}