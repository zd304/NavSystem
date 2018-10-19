#ifndef __SKETCH_SCENE_LOGIC_H__
#define __SKETCH_SCENE_LOGIC_H__

#include "inc.h"

namespace Nav
{
	class NavSceneTree;
	class NavSceneNode;
}

class MeshRenderer;
class Test;

class SketchSceneLogic
{
public:
	SketchSceneLogic(Test* test);
	~SketchSceneLogic();

	void SetNavSceneTree(const Nav::NavSceneTree* tree);
public:
	Test* mTest;
	Nav::NavSceneTree* mTree;
};

#endif
