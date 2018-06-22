LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := NavSystem
LOCAL_SRC_FILES := \
	../Src/math/Vector2.cpp\
	../Src/math/Vector3.cpp\
	../Src/micropather/micropather.cpp\
	../Src/NavGate.cpp\
	../Src/NavGraph.cpp\
	../Src/NavHeightmap.cpp\
	../Src/NavInc.cpp\
	../Src/NavMesh.cpp\
	../Src/NavPhysics.cpp\
	../Src/NavSystem.cpp\
	../Src/NavSystemDLL.cpp\
	../Src/NavTriangle.cpp

include $(BUILD_SHARED_LIBRARY)