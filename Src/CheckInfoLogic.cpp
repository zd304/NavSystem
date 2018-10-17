#include "CheckInfoLogic.h"
#include "Nav.h"
#include "Test.h"
#include "MeshRenderer.h"
#include "NavLinkInfo.h"

CheckInfoLogic::CheckInfoLogic(Test* test)
{
	mTest = test;
	mGraph = NULL;
	mTri = NULL;
}

CheckInfoLogic::~CheckInfoLogic()
{
	if (!mTest->mRenderer)
		return;
	mTest->mRenderer->ClearPath();
}

void CheckInfoLogic::OnPick(const Nav::NavTriangle* tri, const Nav::Vector3& point, const Nav::NavGraph* graph)
{
	if (mGraph && !graph)
		return;
	if (!mTest->mRenderer)
		return;
	//if (mTri)
	//{
	//	mTri = NULL;
	//	mTest->mRenderer->ClearPath();
	//	return;
	//}
	if (tri != NULL && graph != NULL)
	{
		mTri = (Nav::NavTriangle*)tri;
		mGraph = (Nav::NavGraph*)graph;
		mTest->mRenderer->SetPointMesh(tri->mPoint[0], tri->mPoint[1], tri->mPoint[2], point, true);
	}
}

void CheckInfoLogic::OnGUI()
{
	if (!mTri || !mGraph) return;

	unsigned int triIndex = -1;
	for (size_t i = 0; i < mGraph->mMesh->mTriangles.size(); ++i)
	{
		if (mGraph->mMesh->mTriangles[i] == mTri)
		{
			triIndex = (unsigned int)i;
			break;
		}
	}

	float distance[3];
	memset(distance, 0, sizeof(float) * 3);
	for (size_t i = 0; i < mTri->mDistance.size(); ++i)
	{
		distance[i] = mTri->mDistance[i];
	}

	char szContent[1024];
	memset(szContent, 0, 1024);
	sprintf_s(szContent, "\n三角形[%d]:\n  (%.4f, %.4f, %.4f)\n  (%.4f, %.4f, %.4f)\n  (%.4f, %.4f, %.4f)\n邻接三角形数量:  %d\n到邻接三角形距离:\n  %.4f, %.4f, %.4f\n三角形是否可通行:  %s\n所处导航ID:  %d, 场景ID:  %d",
		triIndex,
		mTri->mPoint[0].x, mTri->mPoint[0].y, mTri->mPoint[0].z,
		mTri->mPoint[1].x, mTri->mPoint[1].y, mTri->mPoint[1].z,
		mTri->mPoint[2].x, mTri->mPoint[2].y, mTri->mPoint[2].z,
		(unsigned int)mTri->mNeighbors.size(),
		distance[0], distance[1], distance[2],
		mTri->mPassable ? "是" : "否",
		mGraph->GetLayerID(), mGraph->GetSceneID());
	ImGui::Text(STU(szContent).c_str());

	Nav::NavLinkInfo* linkInfo = NULL;
	for (size_t i = 0; i < mGraph->mMesh->mNavLinkInfos.size(); ++i)
	{
		Nav::NavLinkInfo* lk = mGraph->mMesh->mNavLinkInfos[i];
		if (lk->mTriIndex == triIndex)
		{
			linkInfo = lk;
			break;
		}
	}

	ImGui::Separator();

	if (linkInfo)
	{
		int lkScnID = (int)linkInfo->mLinkID / 100;
		int lkLayerID = (int)linkInfo->mLinkID % 100;
		ImGui::Text(STU("连接场景ID").c_str());
		ImGui::SameLine();
		ImGui::InputInt("##LKSCNID", &lkScnID);

		ImGui::Text(STU("连接层的ID").c_str());
		ImGui::SameLine();
		ImGui::InputInt("##LKLAYERID", &lkLayerID);

		linkInfo->mLinkID = (unsigned int)(lkScnID * 100 + lkLayerID);
	}

	if (!linkInfo && ImGui::Button(STU("创建连接").c_str(), ImVec2(mTest->mLeftUIWidth - 20.0f, 30.0f)))
	{
		Nav::NavLinkInfo* linkInfo = new Nav::NavLinkInfo();
		linkInfo->mTriIndex = triIndex;
		linkInfo->mLinkID = 0;
		mGraph->mMesh->mNavLinkInfos.push_back(linkInfo);
	}
	if (linkInfo && ImGui::Button(STU("删除连接").c_str(), ImVec2(mTest->mLeftUIWidth - 20.0f, 30.0f)))
	{
		std::vector<Nav::NavLinkInfo*>::iterator it;
		for (it = mGraph->mMesh->mNavLinkInfos.begin();
			it != mGraph->mMesh->mNavLinkInfos.end(); ++it)
		{
			Nav::NavLinkInfo* lk = *it;
			if (lk->mTriIndex == triIndex)
			{
				SAFE_DELETE(lk);
				mGraph->mMesh->mNavLinkInfos.erase(it);
				break;
			}
		}
	}
}