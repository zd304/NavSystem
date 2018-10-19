#ifndef __COMMAND_MGR_H__
#define __COMMAND_MGR_H__

#include "inc.h"

class Test;

class CommandMgr
{
public:
	CommandMgr(const Test* test);

	void OnGui();

	static void Init();
private:
	bool OnSubmit(const std::string& cmdName, const std::string* cmdParams, int cmdParamNum);
public:
	const static int COMMAND_LEN = 512;
public:
	Test* mTest;
	char mCommandText[COMMAND_LEN];

	typedef std::function<void(const std::string&, const std::string*, int)> CmdFunc;
	static std::map<std::string, CmdFunc> mMsg;
};

#endif // !__COMMAND_MGR_H__
