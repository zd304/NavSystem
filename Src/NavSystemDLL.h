#ifndef __NAV_SYSTEM_DLL_H__
#define __NAV_SYSTEM_DLL_H__

#if defined (EXPORTBUILD)
#define _DLLExprot __declspec (dllexport)
#else
#define _DLLExprot __declspec (dllimport)
#endif

struct NAV_VEC3 { float x, y, z; };

struct NAV_VEC2 { float x, z; };

extern "C"
{
	bool LoadFromFile(const char* path);

	void Destroy();


}

#endif