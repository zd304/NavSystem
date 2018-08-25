#include "NavLinkInfo.h"

#ifdef _CHECK_LEAK
#define new  new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif
namespace Nav
{
	NavLinkInfo::NavLinkInfo()
	{
		mTriIndex = -1;
		mLinkID = -1;
	}

	unsigned int NavLinkInfo::GetSize()
	{
		return sizeof(unsigned int) * 2;
	}

	unsigned int NavLinkInfo::WriteTo(char* dest, unsigned int ptr)
	{
		memcpy(dest + ptr, &mTriIndex, sizeof(unsigned int));
		ptr += sizeof(unsigned int);
		memcpy(dest + ptr, &mLinkID, sizeof(unsigned int));
		ptr += sizeof(unsigned int);
		return ptr;
	}

	unsigned int NavLinkInfo::ReadFrom(char* src, unsigned int ptr)
	{
		memcpy(&mTriIndex, &src[ptr], sizeof(unsigned int));
		ptr += sizeof(unsigned int);
		memcpy(&mLinkID, &src[ptr], sizeof(unsigned int));
		ptr += sizeof(unsigned int);

		return ptr;
	}
}