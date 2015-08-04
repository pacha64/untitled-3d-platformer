#include "TutorialWorldGenerator.h"


TutorialWorldGenerator::TutorialWorldGenerator(SceneManager* sceneManager)
	: mCurrentLevel(0),
	WorldGenerator(sceneManager)
{

	mCurrentFormat = getTutorialMap(mCurrentLevel);
}


TutorialWorldGenerator::~TutorialWorldGenerator(void)
{
}
