#ifndef __NAV_INC_H__
#define __NAV_INC_H__

#include <vector>
#include "math/Vector3.h"
#include "micropather/micropather.h"

#define SAFE_DELETE(x) if (x) { delete x; x = NULL; }
#define SAFE_DELETE_ARRAY(x) if (x) { delete [] x; x = NULL; }
#define SAFE_RELEASE(x) if (x) { x->Release(); x = NULL; }

#endif