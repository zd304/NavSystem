#include "NavLog.h"
#include <time.h>

void NavLog::Init(const char* path)
{
	mLogPath = path;
}

void NavLog::Log(const std::string& log)
{
	time_t t;
	t = time(NULL);
	struct tm* local;
	local = localtime(&t);
	char temp[256];
	memset(temp, 0, 256);
	sprintf(temp, "[%d-%d-%d %d:%d:%d]",
		local->tm_year, local->tm_mon, local->tm_mday,
		local->tm_hour, local->tm_min, local->tm_sec);

	std::string fullLog = temp + log;

	FILE* fp = NULL;
	fp = fopen(mLogPath.c_str(), "a");
	if (!fp) return;
	fwrite(fullLog.c_str(), 1, fullLog.length(), fp);
	fclose(fp);
}