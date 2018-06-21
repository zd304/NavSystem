#ifndef __NAV_INC_H__
#define __NAV_INC_H__

#define _CRT_SECURE_NO_WARNINGS 

#include <vector>
#include "math/Vector3.h"
#include "micropather/micropather.h"

#define SAFE_DELETE(x) if (x) { delete x; x = NULL; }
#define SAFE_DELETE_ARRAY(x) if (x) { delete [] x; x = NULL; }
#define SAFE_RELEASE(x) if (x) { x->Release(); x = NULL; }

namespace Nav
{
	float MinFloat(float f1, float f2);

	float MaxFloat(float f1, float f2);
}

#endif