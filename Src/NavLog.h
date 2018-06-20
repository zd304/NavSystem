#ifndef __NAV_LOG_H__
#define __NAV_LOG_H__

#include "NavInc.h"

class NavLog
{
public:
	void Init(const char* path);

	void Log(const std::string& log);
private:
	std::string mLogPath;
};

#endif
