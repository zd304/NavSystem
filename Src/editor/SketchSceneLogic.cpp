#include "SketchSceneLogic.h"
#include "Test.h"
#include "MeshRenderer.h"

SketchSceneLogic::SketchSceneLogic(Test* test)
{
	mTest = test;
	mTree = NULL;
}

SketchSceneLogic::~SketchSceneLogic()
{
	if (mTest && mTest->mRenderer)
	{
		SetNavSceneTree(NULL);
	}
}

void SketchSceneLogic::SetNavSceneTree(const Nav::NavSceneTree* tree)
{
	if (mTree == tree)
		return;
	mTree = (Nav::NavSceneTree*)tree;

	mTest->mRenderer->SetSketchScene(mTree);
}