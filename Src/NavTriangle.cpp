#include "NavTriangle.h"

NavTriangle::NavTriangle(const Vector3& v0, const Vector3& v1, const Vector3& v2)
{
	mCenter.Set(0.0f, 0.0f, 0.0f);
	mCenter += v0;
	mCenter += v1;
	mCenter += v2;
	mCenter /= 3.0f;

	mPoint[0] = v0;
	mPoint[0] = v1;
	mPoint[0] = v2;
}