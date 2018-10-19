#include "GraphEditLogic.h"
#include "Test.h"
#include "NavTriangle.h"
#include "NavGraph.h"
#include "MeshRenderer.h"
#include "NavSystem.h"

#ifdef _CHECK_LEAK
#define new  new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

GraphEditLogic::GraphEditLogic(Test* test)
{
	mTest = test;
	mGraph = NULL;
	mID = 0;
}

GraphEditLogic::~GraphEditLogic()
{

}

void GraphEditLogic::OnPick(const Nav::NavTriangle* tri, const Nav::Vector3& point, const Nav::NavGraph* graph)
{
	if (mGraph == graph || graph == NULL)
		return;
	mGraph = (Nav::NavGraph*)graph;
	mID = mGraph->mID;
	mTest->mRenderer->SetEditGraph(mGraph);
}

static bool isOpenTip = false;
void GraphEditLogic::OnGUI()
{
	if (mGraph == NULL)
		return;
	int sceneID = (int)mID / 100;
	int layerID = (int)mID % 100;
	ImGui::Text(STU("场景ID").c_str());
	ImGui::SameLine();
	ImGui::InputInt("#graphSceneID", &sceneID);

	ImGui::Text(STU("层级ID").c_str());
	ImGui::SameLine();
	ImGui::InputInt("#graphLayerID", &layerID);

	mID = sceneID * 100 + layerID;

	if (ImGui::BeginPopupModal(STU("提醒").c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text(STU("层级已存在请重新填写层级").c_str());
		ImGui::Separator();
		if (ImGui::Button(STU("是").c_str(), ImVec2(60, 24)))
		{
			isOpenTip = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	if (isOpenTip)
	{
		ImGui::OpenPopup(STU("提醒").c_str());
	}

	if (ImGui::Button(STU("确定").c_str(), ImVec2(mTest->mLeftUIWidth - 20.0f, 30.0f)))
	{
		if (NULL != mTest->mNavSystem->GetGraphByID(mID))
		{
			isOpenTip = true;
			return;
		}

		mTest->mNavSystem->ResetGraphID(mGraph->mID, mID);

		mGraph = (Nav::NavGraph*)NULL;
		mTest->mRenderer->SetEditGraph(mGraph);
		return;
	}

	if (ImGui::Button(STU("取消").c_str(), ImVec2(mTest->mLeftUIWidth - 20.0f, 30.0f)))
	{
		mGraph = (Nav::NavGraph*)NULL;
		mTest->mRenderer->SetEditGraph(mGraph);
		return;
	}
}