#ifndef __MESH_RENDERER_H__
#define __MESH_RENDERER_H__

#include "FBXHelper.h"

class NavTriangle;
struct MeshVertex;
class NavHeightmap;

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
public:
	IDirect3DDevice9* mDevice;
	std::vector<ID3DXMesh*> mMeshes;
	std::vector<ID3DXMesh*> mWireMeshes;
	std::vector<ID3DXMesh*> mHeightMeshes;

	bool mShowNavMesh;
	bool mShowHeightmap;
	bool mShowWireMesh;

	std::vector<MeshVertex> mSelectedPath;
	ID3DXMesh* mStartMesh;
	ID3DXMesh* mEndMesh;
	ID3DXMesh* mSelectedMesh;
};

#endif