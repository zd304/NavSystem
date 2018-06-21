#ifndef __NAV_TRIANGLE_H__
#define __NAV_TRIANGLE_H__

#include "NavInc.h"

namespace Nav
{
	class NavTriangle
	{
	public:
		NavTriangle();

		NavTriangle(const Vector3& v0, const Vector3& v1, const Vector3& v2);

		unsigned int GetSize();

		unsigned int WriteTo(char* dest, unsigned int ptr);

		unsigned int ReadFrom(char* src, unsigned int ptr);
	public:
		// 三角形三个顶点;
		Vector3 mPoint[3];

		// 三角形中心;
		Vector3 mCenter;

		// Adjacent;

		// 邻接三角形;
		std::vector<NavTriangle*> mNeighbors;
		// 共享边序号说明 0:[0,1]; 1:[1,2]; 2:[2,0];
		std::vector<int> mShareEdgeIndices;
		// 到邻接三角形的距离;
		std::vector<float> mDistance;
		// 邻接边的中点;
		std::vector<Vector3> mEdgeCenter;

		// 是否门;
		bool mPassable;
	};
}

#endif