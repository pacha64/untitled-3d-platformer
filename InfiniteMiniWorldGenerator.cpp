#include "InfiniteMiniWorldGenerator.h"
#include "MatrixLevel.h"

InfiniteMiniWorldGenerator::InfiniteMiniWorldGenerator(SceneManager* sceneManager)
	: WorldGenerator(sceneManager)
{
}


InfiniteMiniWorldGenerator::~InfiniteMiniWorldGenerator(void)
{

}
void InfiniteMiniWorldGenerator::generate()
{

	mWorldTopRight = TO_BULLET(mCurrentFormat->size / 2);
	mWorldBottomLeft = TO_BULLET(-mCurrentFormat->size / 2);
	
	recreateSkyBox();
	
	mStaticGeometry = mSceneManager->createStaticGeometry(requestNewStaticGeometryName());
	mStaticGeometry->setRegionDimensions (mCurrentFormat->size);
	mStaticGeometry->setCastShadows(true);
	mStaticGeometry->build();

	MatrixLevel matrixGenerator;
	matrixGenerator.fillPlatforms(GameState::D_EASY, GameMode::GM_CHECKPOINTS, mPlatforms);

	for (int i = 0; i < mPlatforms.size(); i++)
	{
		auto plat = mPlatforms[i];
		if (plat->Type() == PlatformType::PT_MOVABLE)
		{
			mMovablePlatforms.push_back((MovablePlatform*)plat);
		}
		if (plat->getActivator())
		{
			auto activator = plat->getActivator();
			if (activator->getActivatorType() == WorldActivator::AT_RESPAWN)
			{
				mRespawns.push_back(activator);
			}
			else if (activator->getActivatorType() == WorldActivator::AT_POINTS)
			{
				mPoints.push_back(activator);
			}
		}
	}
}