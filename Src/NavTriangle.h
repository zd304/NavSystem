#ifndef __NAV_TRIANGLE_H__
#define __NAV_TRIANGLE_H__

#include "NavInc.h"

class NavTriangle
{
public:
	NavTriangle(const Vector3& v0, const Vector3& v1, const Vector3& v2);
public:
	Vector3 mPoint[3];

	Vector3 mCenter;

	// Adjacent;
	std::vector<NavTriangle*> mNeighbors;
	std::vector<float> mDistance;
};

#endif