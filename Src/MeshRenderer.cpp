#include "MeshRenderer.h"

struct MeshVertex
{
	D3DXVECTOR3 pos;

	static const int fvf = D3DFVF_XYZ;
};

MeshRenderer::MeshRenderer(IDirect3DDevice9* device, FBXHelper::FBXMeshDatas* mDatas)
{
	mDevice = device;
	if (mDatas == NULL)
		return;
	for (size_t i = 0; i < mDatas->datas.size(); ++i)
	{
		FBXHelper::FBXMeshData* data = mDatas->datas[i];

		ID3DXMesh* mesh = NULL;
		DWORD faceNum = (DWORD)(data->indices.size() / 3);
		DWORD vertexNum = (DWORD)data->pos.size();
		HRESULT hr = D3DXCreateMeshFVF(faceNum, vertexNum, D3DXMESH_32BIT, MeshVertex::fvf, mDevice, &mesh);
		if (FAILED(hr))
			continue;
		mMeshes.push_back(mesh);

		void* vb = NULL;
		mesh->LockVertexBuffer(NULL, &vb);
		memcpy(vb, &data->pos[0], vertexNum * sizeof(D3DXVECTOR3));
		mesh->UnlockVertexBuffer();

		void* ib = NULL;
		mesh->LockIndexBuffer(NULL, &ib);
		memcpy(ib, &data->indices[0], faceNum * 3 * sizeof(unsigned int));
		mesh->UnlockIndexBuffer();
	}
}

MeshRenderer::~MeshRenderer()
{
	mDevice = NULL;
	for (size_t i = 0; i < mMeshes.size(); ++i)
	{
		ID3DXMesh* mesh = mMeshes[i];
		SAFE_RELEASE(mesh);
	}
	mMeshes.clear();
}

void MeshRenderer::Render()
{
	for (size_t i = 0; i < mMeshes.size(); ++i)
	{
		ID3DXMesh* mesh = mMeshes[i];
		if (!mesh)
		{
			continue;
		}
		mesh->DrawSubset(0);
	}
}