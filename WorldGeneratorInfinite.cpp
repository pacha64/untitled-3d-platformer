// not implemented

#if 0
#include "WorldGeneratorInfinite.h"
#include "BtOgreGP.h"
#include "BtOgrePG.h"
#include "MonsterCometLauncher.h"
#include "MonsterState.h"
#include "Player.h"

WorldGeneratorInfinite::WorldGeneratorInfinite(btDiscreteDynamicsWorld* world, SceneManager* sceneManager)
	: WorldGenerator(sceneManager),
	mLastPlatforms(0),
	mCurrentPlatforms(0),
	mNextPlatforms(0),
	mLastMovablePlatforms(0),
	mCurrentMovablePlatforms(0),
	mNextMovablePlatforms(0),
	mRespawnCurrent(0),
	mRespawnLast(0),
	mLastStructure(0),
	mNextStructure(0),
	mSoulMarginCenter(Vector3::ZERO),
	mLastMargin(Vector3::ZERO),
	mCurrentMargin(Vector3::ZERO),
	mNextMargin(Vector3::ZERO),
	mLastFinalPlatform(0),
	mStaticGeometryLast(0),
	mStaticGeometryNext(0)
	//mStaticGeometryCircleLast(0),
	//mStaticGeometryCircleNext(0)
{
	mWorldTopRight.setX(numeric_limits<btScalar>::max());
	mWorldTopRight.setZ(numeric_limits<btScalar>::max());
	mWorldBottomLeft.setX(numeric_limits<btScalar>::min());
	mWorldBottomLeft.setZ(numeric_limits<btScalar>::min());

	mCurrentMeta.creator = INFINITE_PLATFORMER_CREATOR;
	mCurrentMeta.name = INFINITE_PLATFORMER_NAME;
	mCurrentMeta.description = INFINITE_PLATFORMER_DESCRIPTION;
	mCurrentMeta.group = INFINITE_PLATFORMER_GROUP;
}

WorldGeneratorInfinite::~WorldGeneratorInfinite(void)
{
	mSceneManager->destroyStaticGeometry(mStaticGeometry);
	
	if (mStaticGeometryLast != 0)
	{
		mSceneManager->destroyStaticGeometry(mStaticGeometryLast);
		//mSceneManager->destroyStaticGeometry(mStaticGeometryCircleLast);
	}
	if (mStaticGeometryNext != 0)
	{
		mSceneManager->destroyStaticGeometry(mStaticGeometryNext);
		//mSceneManager->destroyStaticGeometry(mStaticGeometryCircleNext);
	}

	if (mLastPlatforms != 0)
	{
		for (int i = 0; i < mLastPlatforms->size(); i++)
		{
			delete (*mLastPlatforms)[i];
		}
	}
	if (mCurrentPlatforms != 0)
	{
		for (int i = 0; i < mCurrentPlatforms->size(); i++)
		{
			delete (*mCurrentPlatforms)[i];
		}
	}
	if (mNextPlatforms != 0)
	{
		for (int i = 0; i < mNextPlatforms->size(); i++)
		{
			delete (*mNextPlatforms)[i];
		}
	}

	for (int i = 0; i < mPlatforms.size(); i++)
	{
		delete mPlatforms[i];
	}

	/*for (int i = 0; i < mSouls.size(); i++)
	{
		delete mSouls[i];
	}*/

	for (int i = 0; i < mRespawns.size(); i++)
	{
		delete mRespawns[i];
	}
	
	if (mRespawnCurrent)
	{
		delete mRespawnCurrent;
	}
	if (mRespawnLast)
	{
		delete mRespawnLast;
	}
}

/*bool WorldGeneratorInfinite::updateTouchingSoul(Vector3& aabbMax, Vector3& aabbMin)
{
	if (mSoulInfinite->isTouchingObject(aabbMax, aabbMin))
	{
		return true;
	}

	return false;
}*/

/*void WorldGeneratorInfinite::createTouchingSoul()
{
	generateNext();
}*/

void WorldGeneratorInfinite::update(const FrameEvent& evt, std::vector<Player*>* players)
{
	for (int i = 0; i < players->size(); i++)
	{
		Vector3 playerPos = (*players)[i]->getLocation();
		playerPos.y -= (PLAYER_BODY_Y / 2 + 0.1);

		//Ray ray(playerPos, Vector3::NEGATIVE_UNIT_Y);
		//RaySceneQuery* query = mSceneManager->createRayQuery(ray);
		//Ogre::RaySceneQueryResult &result = query->execute();
		//Ogre::RaySceneQueryResult::iterator itr = result.begin();
		//if (itr != result.end() && itr->worldFragment)
		//{
		//	TextRenderer::getSingleton().printf("textbox-1", "%f", itr->worldFragment->singleIntersection.y);
		//}
		//mSceneManager->destroyQuery(query);

		//btCollisionWorld::ClosestRayResultCallback result(TO_BULLET(playerPos), btVector3(playerPos.x, playerPos.y - 100000, playerPos.z));
		//Physics::getSingleton()->getWorld()->rayTest(TO_BULLET(playerPos), btVector3(playerPos.x, playerPos.y - 100000, playerPos.z), result);
	}
	for (int i = 0; i < mRespawns.size(); i++)
	{
		mRespawns[i]->respawn->update(evt);
	}

	mRespawnCurrent->respawn->update(evt);
	mRespawnLast->respawn->update(evt);
	
	mMonsterState->update((*players)[0], evt);

	//for (int i = 0; i < mMovablePlatforms.size(); i++)
	//{
	//	mMovablePlatforms[i]->update(evt);
	//}
}

void WorldGeneratorInfinite::recalculateMargins()
{

	//for (int i = 0; i < 3; i++)
	//{
	//	Real last, current, next;
	//	if (mLastStructure != 0)
	//	{
	//		last = mLastMargin[i] + mLastStructure->information.size[i] / 2;
	//	}
	//	current = mCurrentMargin[i] + mMapWorld->information.size[i] / 2;
	//	next = mNextMargin[i] + mNextStructure->information.size[i] / 2;
	//	if (mLastStructure != 0 && (last > current && last > next))
	//	{
	//		mWorldTopRight[i] = last;
	//	}
	//	else if (current > next)
	//	{
	//		mWorldTopRight[i] = current;
	//	}
	//	else
	//	{
	//		mWorldTopRight[i] = next;
	//	}
	//}
	//for (int i = 0; i < 3; i++)
	//{
	//	Real last, current, next;
	//	if (mLastStructure != 0)
	//	{
	//		last = mLastMargin[i] - mLastStructure->information.size[i] / 2;
	//	}
	//	current = mCurrentMargin[i] - mMapWorld->information.size[i] / 2;
	//	next = mNextMargin[i] - mNextStructure->information.size[i] / 2;
	//	if (mLastStructure != 0 && (last < current && last < next))
	//	{
	//		mWorldBottomLeft[i] = last;
	//	}
	//	else if (current < next)
	//	{
	//		mWorldBottomLeft[i] = current;
	//	}
	//	else
	//	{
	//		mWorldBottomLeft[i] = next;
	//	}
	//}

	Vector3 leftBottom = Vector3::ZERO, rightTop = Vector3::ZERO;
	
	btVector3 origin = (*mCurrentPlatforms)[0]->getBox()->getBody()->getWorldTransform().getOrigin();
	btVector3 leftBottomHelper = origin - (*mCurrentPlatforms)[0]->getBox()->typeSize((*mCurrentPlatforms)[0]->getBox()->getBoxType()) / 2;
	btVector3 rightTopHelper = origin + (*mCurrentPlatforms)[0]->getBox()->typeSize((*mCurrentPlatforms)[0]->getBox()->getBoxType()) / 2;
	leftBottom = TO_OGRE(leftBottomHelper);
	rightTop = TO_OGRE(rightTopHelper);
	
	if (mLastPlatforms != 0)
	{
		for (int i = 0; i < mLastPlatforms->size(); i++)
		{
			btVector3 origin = (*mLastPlatforms)[i]->getBox()->getBody()->getWorldTransform().getOrigin();
			btVector3 leftBottomHelper = origin - (*mLastPlatforms)[i]->getBox()->typeSize((*mLastPlatforms)[i]->getBox()->getBoxType()) / 2;
			btVector3 rightTopHelper = origin + (*mLastPlatforms)[i]->getBox()->typeSize((*mLastPlatforms)[i]->getBox()->getBoxType()) / 2;
			for (int j = 0; j < 3; j++)
			{
				if (leftBottomHelper[j] < leftBottom[j])
				{
					leftBottom[j] = leftBottomHelper[j];
				}
				if (rightTopHelper[j] > rightTop[j])
				{
					rightTop[j] = rightTopHelper[j];
				}
			}
		}
	}
	for (int i = 0; i < mCurrentPlatforms->size(); i++)
	{
		btVector3 origin = (*mCurrentPlatforms)[i]->getBox()->getBody()->getWorldTransform().getOrigin();
		btVector3 leftBottomHelper = origin - (*mCurrentPlatforms)[i]->getBox()->typeSize((*mCurrentPlatforms)[i]->getBox()->getBoxType()) / 2;
		btVector3 rightTopHelper = origin + (*mCurrentPlatforms)[i]->getBox()->typeSize((*mCurrentPlatforms)[i]->getBox()->getBoxType()) / 2;
		for (int j = 0; j < 3; j++)
		{
			if (leftBottomHelper[j] < leftBottom[j])
			{
				leftBottom[j] = leftBottomHelper[j];
			}
			if (rightTopHelper[j] > rightTop[j])
			{
				rightTop[j] = rightTopHelper[j];
			}
		}
	}
	for (int i = 0; i < mNextPlatforms->size(); i++)
	{
		btVector3 origin = (*mNextPlatforms)[i]->getBox()->getBody()->getWorldTransform().getOrigin();
		btVector3 leftBottomHelper = origin - (*mNextPlatforms)[i]->getBox()->typeSize((*mNextPlatforms)[i]->getBox()->getBoxType()) / 2;
		btVector3 rightTopHelper = origin + (*mNextPlatforms)[i]->getBox()->typeSize((*mNextPlatforms)[i]->getBox()->getBoxType()) / 2;
		for (int j = 0; j < 3; j++)
		{
			if (leftBottomHelper[j] < leftBottom[j])
			{
				leftBottom[j] = leftBottomHelper[j];
			}
			if (rightTopHelper[j] > rightTop[j])
			{
				rightTop[j] = rightTopHelper[j];
			}
		}
	}
	 
	btScalar margin = INFINITE_PLATFORMING_SKY_BOX_MARGIN;
	mWorldBottomLeft = TO_BULLET(leftBottom) - btVector3(margin, margin, margin);
	mWorldTopRight = TO_BULLET(rightTop) + btVector3(margin, margin, margin);

	fixMapLimits();
}

void WorldGeneratorInfinite::activateRespawn(WorldGenerator::RespawnPoint*)
{
	generateNext();
}

void WorldGeneratorInfinite::generate()
{
	Light* directionalLight = mSceneManager->createLight();
	directionalLight->setType(Light::LightTypes::LT_DIRECTIONAL);
	directionalLight->setDiffuseColour(1, 1, 1);
    directionalLight->setSpecularColour(1, 1, 1.);
	directionalLight->setDirection(0, -1, 0);
	//asfasf->setPosition(0, 200, 0);
	SceneNode* lightNode = mSceneManager->getRootSceneNode()->createChildSceneNode();
	lightNode->translate(Vector3(0, 200000, 0));
	lightNode->attachObject(directionalLight);

	mLastStructure = 0;
	mMapWorld = MapCollection::getSingleton()->getRandomInfinite();
	mNextStructure = MapCollection::getSingleton()->getRandomInfinite();
	//mSoulInfinite = new WorldSoul(mMapWorld->information.infiniteEnd - mMapWorld->information.size / 2);
	//generateNext();

	mLastPlatforms = 0;
	mLastMovablePlatforms = 0;
	mCurrentPlatforms = new std::vector<Platform*>();//mNextPlatforms;
	mCurrentMovablePlatforms = new std::vector<MovablePlatform*>();//mNextMovablePlatforms;
	mNextPlatforms = new std::vector<Platform*>();
	mNextMovablePlatforms = new std::vector<MovablePlatform*>();
	
	mRespawnCurrent = new RespawnPoint(
		//new WorldRespawn(mMapWorld->information.infiniteEnd),
		mMapWorld->information.infiniteEnd +
		Vector3(0, mMapWorld->information.infiniteEndSize.y / 2, 0),
		false
	);// + Vector3(600, 0, 600));
	mRespawnLast = new RespawnPoint(
		//new WorldRespawn(mMapWorld->information.infiniteStart),
		mMapWorld->information.infiniteStart +
		Vector3(0, mMapWorld->information.infiniteEndSize.y / 2, 0),
		true
	);
	mRespawnLast->respawn->setActivation(true);
	
	mCurrentMargin = mSoulMarginCenter;
	if (mLastStructure != 0)
	{
		//mLastMargin = Vector3::ZERO;
		//mCurrentMargin = mSoulMarginCenter;
		mLastMargin = mSoulMarginCenter;
		mLastMargin -= mMapWorld->information.infiniteEnd;// - mMapWorld->information.size / 2;
		mLastMargin -= -mLastStructure->information.infiniteStart;// - mNextStructure->information.size / 2;
	}
	else
	{
		mLastMargin = Vector3::ZERO;
	}
	mNextMargin = mSoulMarginCenter;//+ mNextStructure->information.infiniteStart +  - 
	mNextMargin += mMapWorld->information.infiniteEnd;// - mMapWorld->information.size / 2;
	mNextMargin += -mNextStructure->information.infiniteStart;// - mNextStructure->information.size / 2;

	mSoulMarginCenter = mNextMargin;

	mStaticGeometry = mSceneManager->createStaticGeometry(requestNewStaticGeometryName());
	mStaticGeometry->setRegionDimensions (Vector3(1000000, 1000000, 1000000));

	mStaticGeometryNext = mSceneManager->createStaticGeometry(requestNewStaticGeometryName());
	mStaticGeometryNext->setRegionDimensions (Vector3(1000000, 1000000, 1000000));

	for (int i = 0; i < mMapWorld->map.blocks.size(); i++)
	{
		Platform* plat = generateMapBlock(mMapWorld->map.blocks[i]);//, mSoulMarginCenter);
		mCurrentPlatforms->push_back(plat);
		if (plat->Type() == PlatformType::PT_MOVABLE)
		{
			mCurrentMovablePlatforms->push_back((MovablePlatform*)plat);
		}
	}
	mStaticGeometry->build();
	//for (int i = 0; i < mMapWorld->map.paths.size(); i++)
	//{
	//	std::vector<Platform*>* plats = generatePath(mMapWorld->map.paths[i]);//, Vector3::ZERO, true);//, mSoulMarginCenter);
	//	for (int i = 0; i < plats->size(); i++)
	//	{
	//		mCurrentPlatforms->push_back((*plats)[i]);
	//		if ((*plats)[i]->Type() == PlatformType::PT_MOVABLE)
	//		{
	//			mCurrentMovablePlatforms->push_back((MovablePlatform*)(*plats)[i]);
	//		}
	//	}
	//}
	
	for (int i = 0; i < mNextStructure->map.blocks.size(); i++)
	{
		if (!mNextStructure->map.blocks[i]->isStart)
		{
			Platform* plat = generateMapBlock(mNextStructure->map.blocks[i], mNextMargin, 2); //Vector3(0, 0, mMapWorld->information.size.z / 2 + mNextStructure->information.size.z / 2));//, 0));
			mNextPlatforms->push_back(plat);
			if (plat->Type() == PlatformType::PT_MOVABLE)
			{
				mNextMovablePlatforms->push_back((MovablePlatform*)plat);
			}
		}
	}
	mStaticGeometryNext->build();

	recalculateMargins();
	recreateSkyBox();

	//for (int i = 0; i < mNextStructure->map.paths.size(); i++)
	//{
	//	std::vector<Platform*>* plats = generatePath(mNextStructure->map.paths[i], mNextMargin, true);//Vector3(0, 0, mMapWorld->information.size.z / 2 + mNextStructure->information.size.z / 2), true);//, mSoulMarginCenter);
	//	for (int i = 0; i < plats->size(); i++)
	//	{
	//		mNextPlatforms->push_back((*plats)[i]);
	//		if ((*plats)[i]->Type() == PlatformType::PT_MOVABLE)
	//		{
	//			mNextMovablePlatforms->push_back((MovablePlatform*)(*plats)[i]);
	//		}
	//	}
	//}
	
	//for (int i = 0; i < 3; i++)
	//{
	//	Real current, next;
	////	last = mLastStructure->information.size[i] / 2;//mLastStructure->information.origin.y + mLastStructure->information.size.y / 2;
	//	current = mCurrentMargin[i] + mMapWorld->information.size[i] / 2;//mMapWorld->information.origin.y + mMapWorld->information.size.y / 2;
	//	next = mNextMargin[i] + mNextStructure->information.size[i] / 2;//mNextStructure->information.size[i] / 2;//mNextStructure->information.origin.y + mNextStructure->information.size.y / 2;
	//	/*if (last > current && last > next)
	//	{
	//		mWorldTopRight[i] = last;
	//		mWorldBottomLeft[i] = -last;
	//	}
	//	else */if (current > next)
	//	{
	//		mWorldTopRight[i] = current;//.setY();
	//	}
	//	else
	//	{
	//		mWorldTopRight[i] = next;
	//	}
	//}
	//for (int i = 0; i < 3; i++)
	//{
	//	Real current, next;
	////	last = mLastStructure->information.size[i] / 2;//mLastStructure->information.origin.y + mLastStructure->information.size.y / 2;
	//	current = mCurrentMargin[i] - mMapWorld->information.size[i] / 2;;//mMapWorld->information.origin.y + mMapWorld->information.size.y / 2;
	//	next = mNextMargin[i] - mNextStructure->information.size[i] / 2;;//mNextStructure->information.size[i] / 2;//mNextStructure->information.origin.y + mNextStructure->information.size.y / 2;
	//	/*if (last > current && last > next)
	//	{
	//		mWorldTopRight[i] = last;
	//		mWorldBottomLeft[i] = -last;
	//	}
	//	else */if (current < next)
	//	{
	//		mWorldBottomLeft[i] = current;//.setY();
	//	}
	//	else
	//	{
	//		mWorldBottomLeft[i] = next;
	//	}
	//}

	//for (int i = 0; i < mMapWorld->map.blocks.size(); i++)
	//{
	//	Platform* plat = generateMapBlock(mMapWorld->map.blocks[i]);
	//	mPlatforms.push_back(plat);
	//	if (plat->Type() == PlatformType::PT_MOVABLE)
	//	{
	//		mMovablePlatforms.push_back((MovablePlatform*)plat);
	//	}
	//}
	//for (int i = 0; i < mMapWorld->map.paths.size(); i++)
	//{
	//	std::vector<Platform*>* plats = generatePath(mMapWorld->map.paths[i]);
	//	for (int i = 0; i < plats->size(); i++)
	//	{
	//		mPlatforms.push_back((*plats)[i]);
	//		if ((*plats)[i]->Type() == PlatformType::PT_MOVABLE)
	//		{
	//			mMovablePlatforms.push_back((MovablePlatform*)(*plats)[i]);
	//		}
	//	}
	//}
}

void WorldGeneratorInfinite::generateNext()
{
	if (mLastStructure != 0)
	{
		mSceneManager->destroyStaticGeometry(mStaticGeometryLast);
		
		if (mLastFinalPlatform != 0)
		{
			delete mLastFinalPlatform;
		}
		for (int i = 0; i < mLastPlatforms->size(); i++)
		{
			if ((*mLastPlatforms)[i]->isEnd)
			{
				mLastFinalPlatform = (*mLastPlatforms)[i];
			}
			else
			{
				delete (*mLastPlatforms)[i];
			}
		}
	}

//	mSoulMarginCenter.z += mMapWorld->information.size.z / 2 + mNextStructure->information.size.z / 2;

	delete mRespawnLast;
	mRespawnLast = mRespawnCurrent;
	mRespawnLast->respawn->setActivation(true);
	mLastStructure = mMapWorld;
	mMapWorld = mNextStructure;
	mNextStructure = MapCollection::getSingleton()->getRandomInfinite();
	
	mRespawnCurrent = new RespawnPoint(
		mSoulMarginCenter +
		mMapWorld->information.infiniteEnd +
		Vector3(0, mMapWorld->information.infiniteEndSize.y / 2, 0),
		false
	);// + mMapWorld->information.size / 2);
	
	mCurrentMargin = mSoulMarginCenter;
	if (mLastStructure != 0)
	{
		//mLastMargin = Vector3::ZERO;
		//mCurrentMargin = mSoulMarginCenter;
		mLastMargin = mSoulMarginCenter;
		mLastMargin -= mMapWorld->information.infiniteEnd;// - mMapWorld->information.size / 2;
		mLastMargin -= -mLastStructure->information.infiniteStart;// - mNextStructure->information.size / 2;
	}
	else
	{
		mLastMargin = Vector3::ZERO;
	}
	mNextMargin = mSoulMarginCenter;//+ mNextStructure->information.infiniteStart +  - 
	mNextMargin += mMapWorld->information.infiniteEnd;// - mMapWorld->information.size / 2;
	mNextMargin += -mNextStructure->information.infiniteStart;// - mNextStructure->information.size / 2;

	mSoulMarginCenter = mNextMargin;
	//recreateSkyBox();

	//last *= -1;//mLastStructure->information.size.y / 2;//mLastStructure->information.origin.y - mLastStructure->information.size.y / 2;
	//current *= -1;//= -mMapWorld->information.size.y / 2;//mMapWorld->information.origin.y - mMapWorld->information.size.y / 2;
	//next *= -1;//= -mNextStructure->information.size.y / 2;;//mNextStructure->information.origin.y - mNextStructure->information.size.y / 2;
	//if (last < current && last < next)
	//{
	//	mWorldBottomLeft.setY(last);
	//}
	//else if (current < next)
	//{
	//	mWorldBottomLeft.setY(current);
	//}
	//else
	//{
	//	mWorldBottomLeft.setY(next);
	//}
	//mWorldBottomLeft.setY(mMapWorld->transform.origin.y - mMapWorld->transform.size.y / 2);
	//mWorldBottomLeft.setX(mMapWorld->transform.origin.x - mMapWorld->transform.size.x / 2);
	//mWorldBottomLeft.setZ(mMapWorld->transform.origin.z - mMapWorld->transform.size.z / 2);
	
	delete mLastPlatforms;
	delete mLastMovablePlatforms;

	mStaticGeometryLast = mStaticGeometry;
	mStaticGeometry = mStaticGeometryNext;

	mLastPlatforms = mCurrentPlatforms;
	mLastMovablePlatforms = mCurrentMovablePlatforms;
	mCurrentPlatforms = mNextPlatforms;
	mCurrentMovablePlatforms = mNextMovablePlatforms;

	mNextPlatforms = new std::vector<Platform*>();
	mNextMovablePlatforms = new std::vector<MovablePlatform*>();

	mStaticGeometryNext = mSceneManager->createStaticGeometry(requestNewStaticGeometryName());
	mStaticGeometryNext->setRegionDimensions (Vector3(1000000, 1000000, 1000000));
	//mStaticGeometryCircleNext->setRegionDimensions (Vector3(1000000, 1000000, 1000000));

	for (int i = 0; i < mNextStructure->map.blocks.size(); i++)
	{
		if (!mNextStructure->map.blocks[i]->isStart)
		{
			Platform* plat = generateMapBlock(mNextStructure->map.blocks[i], mNextMargin, 2);
			mNextPlatforms->push_back(plat);
			if (plat->Type() == PlatformType::PT_MOVABLE)
			{
				mNextMovablePlatforms->push_back((MovablePlatform*)plat);
			}
		}
	}
	mStaticGeometryNext->build();
	
	recalculateMargins();
	recreateSkyBox();

	//for (int i = 0; i < mNextStructure->map.paths.size(); i++)
	//{
	//	std::vector<Platform*>* plats = generatePath(mNextStructure->map.paths[i], mNextMargin, true);
	//	for (int i = 0; i < plats->size(); i++)
	//	{
	//		mNextPlatforms->push_back((*plats)[i]);
	//		if ((*plats)[i]->Type() == PlatformType::PT_MOVABLE)
	//		{
	//			mNextMovablePlatforms->push_back((MovablePlatform*)(*plats)[i]);
	//		}
	//	}
	//}
}

StaticGeometry* WorldGeneratorInfinite::type2geometry(WorldBox::BoxType type, int staticGeometryPos)
{
	if (type == WorldBox::BoxType::BT_SIZE)
	{
		return 0;
	}
	else
	{
		switch (staticGeometryPos)
		{
		case 0:
			return mStaticGeometryLast;
			break;
		case 1:
			return mStaticGeometry;
			break;
		case 2:
			return mStaticGeometryNext;
			break;
		}
	}
	return 0;
}

Vector3 WorldGeneratorInfinite::getInitialRespawn()
{
	return mRespawnCurrent->point;
	/*return
		mCurrentMargin +
		mMapWorld->information.infiniteStart +
		Vector3(
			0,
			mMapWorld->information.infiniteStartSize.y / 2 + RESPAWN_Y_MARGIN,
			0
		);// - mMapWorld->information.size / 2;*/
}

WorldGenerator::RespawnPoint* WorldGeneratorInfinite::getRespawnAt(Vector3& aabbMax, Vector3& aabbMin)
{
	if (mRespawnCurrent->respawn->isTouchingObject(aabbMax, aabbMin))
	{
		return mRespawnCurrent;
	}
	return 0;
}

Vector3 WorldGeneratorInfinite::getRandomRespawn()
{
	return mRespawnCurrent->point;
}

#endif