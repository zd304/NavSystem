#ifndef __MESH_RENDERER_H__
#define __MESH_RENDERER_H__

#include "FBXHelper.h"

struct MeshVertex;
namespace Nav
{
	class NavTriangle;
	class NavHeightmap;
	class NavGraph;
	class NavGate;
}
class Test;

class MeshRenderer
{
public:
	MeshRenderer(IDirect3DDevice9* device, FBXHelper::FBXMeshDatas* mDatas, Test* test);
	~MeshRenderer();

	void Render();

	void SetPointMesh(const Vector3& v0, const Vector3& v1, const Vector3& v2, const Vector3& point, bool isStart);

	void SetSelectedPath(const std::vector<Nav::NavTriangle*>& tris);

	void SetSelectedPath(const std::vector<Vector3>& path);

	void SetHeightmap(const Nav::NavHeightmap* heightmap, size_t index);

	void ClearPath();

	void SetGates(Nav::NavGraph* graph);

	void SetSingleGate(Nav::NavGraph* graph, Nav::NavGate* gate);

	void CalcAllCloseGates();

	void ClearGate();
public:
	Test* mTest;
	IDirect3DDevice9* mDevice;
	std::vector<ID3DXMesh*> mMeshes;
	std::vector<ID3DXMesh*> mWireMeshes;
	std::vector<ID3DXMesh*> mHeightMeshes;
	ID3DXMesh* mAllCloseGates;

	bool mShowNavMesh;
	bool mShowHeightmap;
	bool mShowWireMesh;

	// PathFind;
	std::vector<MeshVertex> mSelectedPath;
	ID3DXMesh* mStartMesh;
	ID3DXMesh* mEndMesh;
	ID3DXMesh* mSelectedMesh;

	// Gate;
	ID3DXMesh* mGateMeshInGraph;
	ID3DXMesh* mGateMeshSingle;
};

#endif