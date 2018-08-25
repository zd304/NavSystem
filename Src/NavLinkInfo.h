#ifndef __NAV_LINK_INFO_H__
#define __NAV_LINK_INFO_H__

#include "NavInc.h"

namespace Nav
{
	class NavLinkInfo
	{
	public:
		NavLinkInfo();

		unsigned int GetSize();

		unsigned int WriteTo(char* dest, unsigned int ptr);

		unsigned int ReadFrom(char* src, unsigned int ptr);
	public:
		unsigned int mTriIndex;
		unsigned int mLinkID;
	};
}

#endif
