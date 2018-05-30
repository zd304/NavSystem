#ifndef __FBX_HELPER_H__
#define __FBX_HELPER_H__

#include "FBXCommon.h"
#include "inc.h"

namespace FBXHelper
{
	class FBXMeshData
	{
	public:
		std::vector<D3DXVECTOR3> pos;
		std::vector<unsigned int> indices;
	};

	class FBXMeshDatas
	{
	public:
		~FBXMeshDatas();
	public:
		std::vector<FBXMeshData*> datas;
	};

	bool BeginFBXHelper(const char* fileName);

	FBXMeshDatas* GetMeshDatas();

	void GetBox(D3DXVECTOR3& max, D3DXVECTOR3& min);

	bool EndFBXHelper();
}
#endif