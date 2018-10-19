#ifndef __INC_H__
#define __INC_H__

#include <string>
#include <map>
#include <vector>
#include <list>
#include <functional>
#include <algorithm>
#include <Windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <io.h>
#include <direct.h>
#include "imgui/imgui.h"
#include "math/Vector3.h"

#define SAFE_DELETE(x) if (x) { delete x; x = NULL; }
#define SAFE_DELETE_ARRAY(x) if (x) { delete [] x; x = NULL; }
#define SAFE_RELEASE(x) if (x) { x->Release(); x = NULL; }

std::string UTS(const std::string & str);

std::string STU(const std::string & str);

void StringReplace(std::string &strBase, const std::string& strSrc, const std::string& strDes);

void StringSplit(const std::string& str, const std::string& delim, std::vector<std::string>& rst);

class Global
{
public:
	static ImTextureID mFolderTexID;
	static ImTextureID mFileTexID;
	static ImTextureID mDiskTexID;
};

#endif