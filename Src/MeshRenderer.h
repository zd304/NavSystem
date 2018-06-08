#ifndef __MESH_RENDERER_H__
#define __MESH_RENDERER_H__

#include "FBXHelper.h"

class NavTriangle;
struct MeshVertex;
class NavHeightmap;
class NavGraph;
class NavGate;

class MeshRenderer
{
public:
	MeshRenderer(IDirect3DDevice9* device, FBXHelper::FBXMeshDatas* mDatas);
	~MeshRenderer();

	void Render();

	void SetPointMesh(const Vector3& v0, const Vector3& v1, const Vector3& v2, const Vector3& point, bool isStart);

	void SetSelectedPath(const std::vector<NavTriangle*>& tris);

	void SetSelectedPath(const std::vector<Vector3>& path);

	void SetHeightmap(const NavHeightmap* heightmap, size_t index);

	void ClearPath();

	void SetGates(NavGraph* graph);

	void SetSingleGate(NavGraph* graph, NavGate* gate);

	void ClearGate();
public:
	IDirect3DDevice9* mDevice;
	std::vector<ID3DXMesh*> mMeshes;
	std::vector<ID3DXMesh*> mWireMeshes;
	std::vector<ID3DXMesh*> mHeightMeshes;

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