#ifndef __NAV_SCENE_NODE_H__
#define __NAV_SCENE_NODE_H__

#include "NavInc.h"

namespace Nav
{
	class NavSceneNode
	{
	public:
		Vector2 GetCenter() const;

		unsigned int GetSize();

		unsigned int WriteTo(char* dest, unsigned int ptr);

		unsigned int ReadFrom(char* src, unsigned int ptr);
	public:
		unsigned int mScnID;
		std::vector<unsigned int> mLinkScnIDs;

		float x;
		float y;
		float width;
		float height;
	};
}

#endif
