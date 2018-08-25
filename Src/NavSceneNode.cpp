#include "NavSceneNode.h"

namespace Nav
{
	Vector2 NavSceneNode::GetCenter() const
	{
		Vector2 min(x, y);
		Vector2 max(x + width, y + height);

		return (min + max) * 0.5f;
	}

	unsigned int NavSceneNode::GetSize()
	{
		unsigned int size = sizeof(unsigned int);
		size += sizeof(unsigned int);

		unsigned int linkScnIDSize = (unsigned int)mLinkScnIDs.size();
		size += (linkScnIDSize * sizeof(unsigned int));

		size += (4 * sizeof(float));

		return size;
	}

	unsigned int NavSceneNode::WriteTo(char* dest, unsigned int ptr)
	{
		memcpy(dest + ptr, &mScnID, sizeof(unsigned int));
		ptr += sizeof(unsigned int);

		unsigned int linkScnIDSize = (unsigned int)mLinkScnIDs.size();
		memcpy(dest + ptr, &linkScnIDSize, sizeof(unsigned int));
		ptr += sizeof(unsigned int);
		for (unsigned int i = 0; i < linkScnIDSize; ++i)
		{
			unsigned int scnID = mLinkScnIDs[i];
			memcpy(dest + ptr, &scnID, sizeof(unsigned int));
			ptr += sizeof(unsigned int);
		}

		memcpy(dest + ptr, &x, sizeof(float));
		ptr += sizeof(float);
		memcpy(dest + ptr, &y, sizeof(float));
		ptr += sizeof(float);
		memcpy(dest + ptr, &width, sizeof(float));
		ptr += sizeof(float);
		memcpy(dest + ptr, &height, sizeof(float));
		ptr += sizeof(float);

		return ptr;
	}

	unsigned int NavSceneNode::ReadFrom(char* src, unsigned int ptr)
	{
		memcpy(&mScnID, src + ptr, sizeof(unsigned int));
		ptr += sizeof(unsigned int);

		unsigned int linkScnIDSize = 0;
		memcpy(&linkScnIDSize, src + ptr, sizeof(unsigned int));
		ptr += sizeof(unsigned int);
		for (unsigned int i = 0; i < linkScnIDSize; ++i)
		{
			unsigned int scnID = 0;

			memcpy(&scnID, src + ptr, sizeof(unsigned int));
			ptr += sizeof(unsigned int);
		}

		memcpy(&x, src + ptr, sizeof(float));
		ptr += sizeof(float);
		memcpy(&y, src + ptr, sizeof(float));
		ptr += sizeof(float);
		memcpy(&width, src + ptr, sizeof(float));
		ptr += sizeof(float);
		memcpy(&height, src + ptr, sizeof(float));
		ptr += sizeof(float);

		return ptr;
	}
}