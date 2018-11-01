#include "CommandMgr.h"
#include "Test.h"
#include "NavSystemDLL.h"

std::map<std::string, CommandMgr::CmdFunc> CommandMgr::mMsg;

CommandMgr::CommandMgr(const Test* test)
{
	mTest = const_cast<Test*>(test);
	memset(mCommandText, 0, COMMAND_LEN);
}

void CommandMgr::OnGui()
{
	ImGui::SetNextWindowPos(ImVec2(mTest->mLeftUIWidth + 10.0f, 0));
	ImGui::SetNextWindowSize(ImVec2(800.0f, 120.0f));
	ImGui::Begin(STU("√¸¡Ó––").c_str(), NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar);

	ImGui::InputText(STU("###").c_str(), mCommandText, COMMAND_LEN);
	ImGui::SameLine();
	if (ImGui::Button(STU("Ã·Ωª").c_str()))
	{
		std::string cmd = mCommandText;

		std::vector<std::string> commandTexts;
		StringSplit(cmd, " ", commandTexts);

		if (commandTexts.size() == 0)
			return;

		if (commandTexts.size() > 1)
		{
			if (OnSubmit(commandTexts[0], &commandTexts[1], (int)commandTexts.size() - 1))
			{
				memset(mCommandText, 0, COMMAND_LEN);
			}
		}
		else
		{
			if (OnSubmit(commandTexts[0], NULL, 0))
			{
				memset(mCommandText, 0, COMMAND_LEN);
			}
		}
	}

	ImGui::End();
}

bool CommandMgr::OnSubmit(const std::string& cmdName, const std::string* cmdParams, int cmdParamNum)
{
	std::map<std::string, CmdFunc>::iterator it = mMsg.find(cmdName);
	if (it == mMsg.end())
		return false;
	CmdFunc func = it->second;
	func(cmdName, cmdParams, cmdParamNum);
	return true;
}

//////////////////////////////////////////////////////////////////////////

void OnCreate(const std::string& cmdName, const std::string* cmdParams, int cmdParamNum)
{
	if (cmdParamNum < 1)
		return;
	const std::string& path = cmdParams[0];
	Nav_Create(path.c_str(), path.c_str());
}

void OnLoadSketchSceneFromFile(const std::string& cmdName, const std::string* cmdParams, int cmdParamNum)
{
	if (cmdParamNum < 1)
		return;
	const std::string& path = cmdParams[0];
	Nav_LoadSketchSceneFromFile(path.c_str());
}

void OnGetLayerHeight(const std::string& cmdName, const std::string* cmdParams, int cmdParamNum)
{
	if (cmdParamNum < 4)
		return;
	NAV_VEC3 pos;
	pos.x = (float)atof(cmdParams[0].c_str());
	pos.y = (float)atof(cmdParams[1].c_str());
	pos.z = (float)atof(cmdParams[2].c_str());
	unsigned int layer = atoi(cmdParams[3].c_str());
	float h = 0.0f;
	if (Nav_GetLayerHeight(&pos, layer, &h))
	{
		printf("GetLayerHeight - %.4f", h);
	}
}

void CommandMgr::Init()
{
	mMsg["Nav_Create"] = OnCreate;
	mMsg["Nav_LoadSketchSceneFromFile"] = OnLoadSketchSceneFromFile;
	mMsg["Nav_GetLayerHeight"] = OnGetLayerHeight;
}