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
}