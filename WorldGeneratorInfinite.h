// not implemented

#if 0
#pragma once
#include "stdafx.h"
#include "worldgenerator.h"
#include "MapCollection.h"

class WorldGeneratorInfinite : public WorldGenerator
{
public:
	WorldGeneratorInfinite(btDiscreteDynamicsWorld* world, SceneManager* sceneManager);
	virtual ~WorldGeneratorInfinite(void);
	virtual WorldGeneratorType getGeneratorType()
	{
		return WorldGeneratorType::WGT_INFNITE;
	}
	//virtual bool updateTouchingSoul(Vector3&, Vector3&);
	virtual void generate();
	virtual void activateRespawn(WorldAc*);
	virtual WorldGenerator::RespawnPoint* getRespawnAt(Vector3& aabbMax, Vector3& aabbMin);
	virtual void update(const FrameEvent&, std::vector<Player*>*);
	//virtual void createTouchingSoul();
protected:
	virtual StaticGeometry* type2geometry(WorldBox::BoxType, int = 1);
	//Platform* generateMapBlock(WorldStructure::Map::Block*, Vector3 = Vector3::ZERO);
	Vector3 mLastMargin, mCurrentMargin, mNextMargin;
	void recalculateMargins();
	virtual Vector3 getInitialRespawn();
	virtual Vector3 getRandomRespawn();
	Vector3 mSoulMarginCenter;
	RespawnPoint* mRespawnCurrent, *mRespawnLast;
	void generateNext();
	WorldStructure *mLastStructure, *mNextStructure;
	StaticGeometry* mStaticGeometryLast, *mStaticGeometryNext;
	Platform* mLastFinalPlatform;
	std::vector<Platform*>* mLastPlatforms, *mCurrentPlatforms, *mNextPlatforms;
	std::vector<MovablePlatform*>* mLastMovablePlatforms, *mCurrentMovablePlatforms, *mNextMovablePlatforms;
};

#endif