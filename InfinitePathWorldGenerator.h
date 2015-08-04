#pragma once
#include "WorldGenerator.h"
#include "GameState.h"

class InfinitePathWorldGenerator : public WorldGenerator
{
public:
	InfinitePathWorldGenerator(SceneManager*);
	~InfinitePathWorldGenerator(void);
	virtual void generate();
	virtual void update(const FrameEvent&, std::vector<Player*>*);
	void generateNextBatch();
	virtual Vector3 worldRespawn2vector(WorldActivator*);
	virtual Vector3 getInitialRespawn();
	virtual Vector3 getRandomRespawn();
	virtual bool isInsideWorld(Vector3 aabbMax, Vector3 aabbMin);
	virtual void platformShaderArgs();
	virtual Real calculateTimeLeft();
	void setDifficulty(GameState::Difficulty difficulty) { mDifficulty = difficulty; }
	Real getPlatformsToGoal()
	{
		return mTotalPlatforms[1];
	}
	virtual WorldGeneratorType getGeneratorType()
	{
		return WorldGeneratorType::WGT_INFINITE_PATH;
	}
protected:
	Platform* mGoalPlatform, *mNextGoalPlatform, *mPreviousGoalPlatform;
	std::vector<Platform*> mPreviousToDestroyPlatforms, mPreviousPlatforms, mCurrentPlatforms, mNextPlatforms;
	std::pair<Vector3, Vector3> mAabb[3];
	Real mTimeSinceGoalReach, mTotalPlatforms[3];
	GameState::Difficulty mDifficulty;
};

