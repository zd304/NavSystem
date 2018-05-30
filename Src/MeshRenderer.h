#ifndef __MESH_RENDERER_H__
#define __MESH_RENDERER_H__

#include "FBXHelper.h"

class MeshRenderer
{
public:
	MeshRenderer(IDirect3DDevice9* device, FBXHelper::FBXMeshDatas* mDatas);
	~MeshRenderer();

	void Render();
public:
	IDirect3DDevice9* mDevice;
	std::vector<ID3DXMesh*> mMeshes;
};

#endif