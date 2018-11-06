#ifndef __NAV_MESH_H__
#define __NAV_MESH_H__

#include "NavInc.h"

namespace Nav
{
	class NavTriangle;
	class NavLinkInfo;
	class NavBoundsVolume;
	struct NavEdge;
	struct AdjacentEdgePair;

	class NavMesh
	{
	public:
		NavMesh();
		NavMesh(Vector3* vertices, int vertexNum, unsigned int* indices, int indicesNum);
		~NavMesh();

		bool GetHeight(const Vector3& pos, float* height);

		void GetBound(Vector3* min, Vector3* max);

		void AddBoundsVolume(NavBoundsVolume* bv);

		void DelBoundsVolume(const std::string& name);

		unsigned int GetSize();

		unsigned int WriteTo(char* dest, unsigned int ptr);

		unsigned int ReadFrom(char* src, unsigned int ptr);

		void UpdateAdjacent(bool calcBounds = true);
	private:
		bool IsTriangleAdjacent(NavTriangle* tri1, NavTriangle* tri2, AdjacentEdgePair* aep);
	private:
		std::map<std::string, NavBoundsVolume*> mBoundsVolumesMap;
	public:
		std::vector<NavTriangle*> mTriangles;
		std::vector<NavLinkInfo*> mNavLinkInfos;
		std::vector<NavEdge*> mBounds;
		std::vector<NavBoundsVolume*> mBoundsVolumes;
		Vector3 mMin;
		Vector3 mMax;
	};

	void DecodeEdgeIndex(int edgeIndex, int* pointIndex1, int* pointIndex2);
}

#endif