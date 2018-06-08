#define EXPORTBUILD

#include "NavSystemDLL.h"
#include "Nav.h"

static NavSystem* navSystem = NULL;

extern "C"
{
	bool LoadFromFile(const char* path)
	{
		navSystem = new NavSystem();
		navSystem->LoadFromFile(path);
		return true;
	}

	void Destroy()
	{
		SAFE_DELETE(navSystem);
	}
}