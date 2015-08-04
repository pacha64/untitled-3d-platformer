#pragma once
#include "Monster.h"
#include "Platform.h"

class MonsterCometLauncher : public Monster
{
public:
	MonsterCometLauncher(StaticGeometry*,Vector3);
	~MonsterCometLauncher(void);
	virtual MonsterType getMonsterType();
	virtual void update(Player*, const Ogre::FrameEvent& evt);
	Platform* getLauncherPlatform()
	{
		return mRocketLauncherPlatform;
	}
protected:
	struct CometMonster : public WorldTouchableObject
	{
		CometMonster(SceneManager* manager, Vector3 position);
		/*	: WorldTouchableObject(position, Vector3(60, 60, 60)),
			parentNode(0),
			cometNode(0),
			entity(0)
		{
			parentNode = manager->getRootSceneNode()->createChildSceneNode(position);
			cometNode = parentNode->createChildSceneNode();
			entity = manager->createEntity("comet.mesh");
			entity->setMaterialName("Monster/Comet");
		}*/
		~CometMonster();
		/*{
			Playground::destroySceneNode(parentNode);
		}*/
		SceneNode* parentNode, *cometNode;
		Entity* entity;
	};
	Vector3 mPosition;
	//WorldTouchableObject mMonsterTouchable;
	const double cometSpeed, cometMarginFromLauncher, cometTimeToLaunch, cometTimeMaxLifespan;
	Vector3 mNormalizedCometDirection;
	CometMonster* mComet;
	//Entity* mCometEntity;
	Platform* mRocketLauncherPlatform;
	bool mIsFlying;
	double mTimeSinceFlying, mTimeCounter;
	bool mStartedFlyingHelper;
};

