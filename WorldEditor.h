#pragma once
#include "WorldGenerator.h"
#include "BtOgreGP.h"
#include "BtOgrePG.h"
#include "MonsterCometLauncher.h"
#include "MonsterState.h"
#include "Player.h"
#include "MapFormat.h"

class WorldEditor : public WorldGenerator
{
public:
	WorldEditor(SceneManager* sceneManager);
	~WorldEditor(void);
	virtual void update(const FrameEvent&, std::vector<Player*>*);
	MovablePlatform* upcastPlatform(Platform*);
	Platform* newPlatform(Vector3 origin, Degree rotation);
	void deletePlatform(Platform*);
	void setEditing(bool);
	void setInitialRespawn(Platform*);
	void reloadMapFormat();
	void changeActivatorType(WorldActivator* activator)
	{
		if (activator->getActivatorType() == WorldActivator::AT_POINTS)
		{
			mPoints.erase(remove(mPoints.begin(), mPoints.end(), activator), mPoints.end());
			mRespawns.push_back(activator);
		}
		else if (activator->getActivatorType() == WorldActivator::AT_RESPAWN)
		{
			mRespawns.erase(remove(mRespawns.begin(), mRespawns.end(), activator), mRespawns.end());
			mPoints.push_back(activator);
		}
	}
	void removeActivator(WorldActivator* activator)
	{
		if (activator->getActivatorType() == WorldActivator::AT_POINTS)
		{
			mPoints.erase(remove(mPoints.begin(), mPoints.end(), activator), mPoints.end());
		}
		else if (activator->getActivatorType() == WorldActivator::AT_RESPAWN)
		{
			mRespawns.erase(remove(mRespawns.begin(), mRespawns.end(), activator), mRespawns.end());
		}
	}
	void addActivator(WorldActivator* activator)
	{
		if (activator->getActivatorType() == WorldActivator::AT_RESPAWN)
		{
			mRespawns.push_back(activator);
		}
		else if (activator->getActivatorType() == WorldActivator::AT_POINTS)
		{
			mPoints.push_back(activator);
		}
	}
	virtual WorldGeneratorType getGeneratorType()
	{
		return WorldGeneratorType::WGT_WORLD_EDITOR;
	}
	bool canAddMorePlatforms()
	{
		return mPlatforms.size() < MAX_PLATFORMS_NUM;
	}
	int getAmountOfPlatforms()
	{
		return mPlatforms.size();
	}
protected:
	bool mIsEditing;
};