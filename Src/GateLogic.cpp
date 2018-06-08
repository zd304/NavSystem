#include "GateLogic.h"
#include "Test.h"
#include "MeshRenderer.h"
#include "NavTriangle.h"
#include "NavGraph.h"
#include "NavGate.h"
#include "NavMesh.h"

GateLogic::GateLogic(Test* test)
{
	mTest = test;
	mNavGraph = NULL;
	mCurMode = GateEditMode_GateList;
}

GateLogic::~GateLogic()
{
	if (mTest && mTest->mRenderer)
	{
		mTest->mRenderer->ClearGate();
	}
}

void GateLogic::OnPick(const NavTriangle* tri, const Vector3& point, const NavGraph* graph)
{
	if (mCurMode == GateEditMode_GateList && mNavGraph == NULL && graph)
	{
		mNavGraph = (NavGraph*)graph;
		mTest->mRenderer->SetGates(mNavGraph);
		return;
	}
	if (mCurMode == GateEditMode_GateList && mNavGraph != NULL && graph && tri)
	{
		bool find = false;
		unsigned int triIndex = 0;
		for (size_t i = 0; i < graph->mMesh->mTriangles.size(); ++i)
		{
			if (graph->mMesh->mTriangles[i] == tri)
			{
				find = true;
				triIndex = i;
				break;
			}
		}
		if (find)
		{
			for (size_t i = 0; i < graph->mGates.size(); ++i)
			{
				NavGate* gate = graph->mGates[i];
				for (size_t j = 0; j < gate->mTriIndices.size(); ++j)
				{
					if (gate->mTriIndices[j] == triIndex)
					{
						mCurMode = GateEditMode_GateTriangles;
						mNavGate = gate;
						return;
					}
				}
			}
		}
	}

	if (mCurMode == GateEditMode_GateTriangles && mNavGate && graph && tri)
	{
		for (size_t i = 0; i < graph->mMesh->mTriangles.size(); ++i)
		{
			if (graph->mMesh->mTriangles[i] == tri)
			{
				mNavGate->mTriIndices.push_back(i);
				mTest->mRenderer->SetSingleGate(mNavGraph, mNavGate);
				return;
			}
		}
	}
}

void GateLogic::OnGUI()
{
	GateEditMode lastMode = mCurMode;
	if (mNavGraph == NULL)
	{
		ImGui::Text(STU("点击场景视图选择导航").c_str());
		return;
	}
	if (mCurMode == GateEditMode_GateList)
	{
		if (ImGui::Button(STU("返回").c_str(), ImVec2(mTest->mLeftUIWidth - 16.0f, 30.0f)))
		{
			mNavGraph = NULL;
			mNavGate = NULL;
			return;
		}

		if (ImGui::Button(STU("创建门").c_str(), ImVec2(mTest->mLeftUIWidth - 16.0f, 30.0f)))
		{
			NavGate* gate = new NavGate(mNavGraph);
			mNavGraph->mGates.push_back(gate);
		}

		ImGui::Spacing();

		char szTemp[64];

		for (size_t i = 0; i < mNavGraph->mGates.size(); ++i)
		{
			NavGate* gate = mNavGraph->mGates[i];
			gate->mID = i;
			memset(szTemp, 0, 64);
			sprintf_s(szTemp, "门[%d]", i);
			if (ImGui::Button(STU(szTemp).c_str(), ImVec2(mTest->mLeftUIWidth * 0.3333f - 16.0f, 24.0f)))
			{
				mCurMode = GateEditMode_GateTriangles;
				mNavGate = gate;
				break;
			}

			ImGui::SameLine(0.0f, 16.0f);

			memset(szTemp, 0, 64);
			sprintf_s(szTemp, "#GP[%d]", i);
			ImGui::PushID(szTemp);
			const char* btText = gate->mPassable ? "关门" : "开门";
			if (ImGui::Button(STU(btText).c_str(), ImVec2(mTest->mLeftUIWidth * 0.3333f - 16.0f, 24.0f)))
			{
				gate->mPassable = !gate->mPassable;
				mTest->mRenderer->SetGates(mNavGraph);
				gate->SwitchPassable(gate->mPassable);
			}
			ImGui::PopID();

			ImGui::SameLine(0.0f, 16.0f);

			memset(szTemp, 0, 64);
			sprintf_s(szTemp, "#GATE[%d]", i);
			ImGui::PushID(szTemp);
			if (ImGui::Button(STU("删除").c_str(), ImVec2(mTest->mLeftUIWidth * 0.3333f - 16.0f, 24.0f)))
			{
				mNavGraph->mGates.erase(mNavGraph->mGates.begin() + i);
				mTest->mRenderer->SetGates(mNavGraph);
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

		const char* btText = mNavGate->mPassable ? "关门" : "开门";
		if (ImGui::Button(STU(btText).c_str(), ImVec2(mTest->mLeftUIWidth - 16.0f, 30.0f)))
		{
			mNavGate->mPassable = !mNavGate->mPassable;
			mTest->mRenderer->SetGates(mNavGraph);
			mNavGate->SwitchPassable(mNavGate->mPassable);
		}

		ImGui::Text(STU(szTemp).c_str());
		if (ImGui::Button(STU("返回").c_str(), ImVec2(mTest->mLeftUIWidth - 16.0f, 30.0f)))
		{
			mCurMode = GateEditMode_GateList;
			mNavGate = NULL;
			return;
		}
	}

	if (lastMode != mCurMode)
	{
		if (mCurMode == GateEditMode_GateTriangles)
		{
			mTest->mRenderer->SetSingleGate(mNavGraph, mNavGate);
		}
		if (mCurMode == GateEditMode_GateList)
		{
			mTest->mRenderer->SetGates(mNavGraph);
		}
	}
}