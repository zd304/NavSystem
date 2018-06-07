#include "GateLogic.h"
#include "Test.h"
#include "NavTriangle.h"
#include "NavGraph.h"
#include "NavGate.h"

GateLogic::GateLogic(Test* test)
{
	mTest = test;
	mNavGraph = NULL;
	mCurMode = GateEditMode_GateList;
}

GateLogic::~GateLogic()
{

}

void GateLogic::OnPick(const NavTriangle* tri, const Vector3& point, const NavGraph* graph)
{
	if (mNavGraph == NULL)
		mNavGraph = (NavGraph*)graph;
}

void GateLogic::OnGUI()
{
	if (mNavGraph == NULL)
	{
		return;
	}
	if (mCurMode == GateEditMode_GateList)
	{
		if (ImGui::Button(STU("返回").c_str(), ImVec2(240.0f, 30.0f)))
		{
			mNavGraph = NULL;
			mNavGate = NULL;
			return;
		}

		if (ImGui::Button(STU("创建门").c_str()))
		{
			NavGate* gate = new NavGate(mNavGraph);
			mNavGraph->mGates.push_back(gate);
		}

		char szTemp[64];

		for (size_t i = 0; i < mNavGraph->mGates.size(); ++i)
		{
			NavGate* gate = mNavGraph->mGates[i];
			gate->mID = i;
			memset(szTemp, 0, 64);
			sprintf_s(szTemp, "门[%d]", i);
			if (ImGui::Button(STU(szTemp).c_str()))
			{
				mCurMode = GateEditMode_GateTriangles;
				mNavGate = gate;
				break;
			}
			ImGui::SameLine();

			memset(szTemp, 0, 64);
			sprintf_s(szTemp, "#GATE[%d]", i);
			ImGui::PushID(szTemp);
			if (ImGui::Button(STU("删除").c_str()))
			{
				mNavGraph->mGates.erase(mNavGraph->mGates.begin() + i);
				ImGui::PopID();
				break;
			}
			ImGui::PopID();
		}
	}
	else if (mCurMode == GateEditMode_GateTriangles)
	{
		char szTemp[64];
		memset(szTemp, 0, 64);
		sprintf_s(szTemp, "当前编辑的门ID = [%d]", mNavGate->mID);

		ImGui::Text(STU(szTemp).c_str());
		if (ImGui::Button(STU("返回").c_str()))
		{
			mCurMode = GateEditMode_GateList;
			mNavGate = NULL;
			return;
		}
	}
}