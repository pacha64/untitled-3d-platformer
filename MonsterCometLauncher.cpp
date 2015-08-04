#if 0
#include "MonsterCometLauncher.h"
#include "Playground.h"
#include "balance.h"

MonsterCometLauncher::CometMonster::CometMonster(SceneManager* manager, Vector3 position)
	: WorldTouchableObject(position, Vector3(60, 60, 60)),
	parentNode(0),
	cometNode(0),
	entity(0)
{
	parentNode = manager->getRootSceneNode()->createChildSceneNode(position);
	cometNode = parentNode->createChildSceneNode();
	entity = manager->createEntity("comet.mesh");
	entity->setMaterialName("Monster/Comet");
	entity->setCastShadows(false);
	cometNode->attachObject(entity);
}
MonsterCometLauncher::CometMonster::~CometMonster()
{
	Playground::destroySceneNode(parentNode);
}

MonsterCometLauncher::MonsterCometLauncher(StaticGeometry* staticGeometry, Vector3 position)
	: Monster(),
	mIsFlying(false),
	mTimeSinceFlying(0),
	mNormalizedCometDirection(Vector3::ZERO),
	cometSpeed(COMET_LAUNCHER_COMET_SPEED),
	mTimeCounter(0.0),
	cometMarginFromLauncher(125.0),
	cometTimeToLaunch(COMET_LAUNCHER_TIME_TO_LAUNCH),
	cometTimeMaxLifespan(COMET_LAUNCHER_LIFE_SPAN),
	mStartedFlyingHelper(false),
	mComet(0),
	mPosition(position)
{
	//mEntity = 0;
	
	//mSceneNode = mSceneManager->getRootSceneNode()->createChildSceneNode(mPosition);// + mNormalizedCometDirection * cometMarginFromLauncher);

	btTransform transf;
	transf.setIdentity();
	transf.setOrigin(TO_BULLET(mPosition));
//	mRocketLauncherPlatform = new Platform(new WorldBox(staticGeometry, transf, WorldBox::BoxType::BT_COMET_LAUNCHER, false));
}

MonsterCometLauncher::~MonsterCometLauncher(void)
{
}

MonsterType MonsterCometLauncher::getMonsterType()
{
	return MonsterType::MT_COMET_LAUNCHER;
}

void MonsterCometLauncher::update(Player* player, const Ogre::FrameEvent& evt)
{
	mTimeCounter += evt.timeSinceLastFrame;

	if (mComet)
	{
		if (mComet->isTouchingObject(player->getAabbMin(), player->getAabbMax()))
		{
		}
	}

	if (!mIsFlying)
	{
		if (mComet)
		{
			delete mComet;
			mComet = 0;
		}

		mNormalizedCometDirection = player->getLocation() - mPosition;
		mNormalizedCometDirection.normalise();

		mComet = new CometMonster(mSceneManager, mPosition);// + mNormalizedCometDirection * cometMarginFromLauncher + Vector3::UNIT_Z * cometMarginFromLauncher);
		mComet->cometNode->translate(Vector3::UNIT_Z * cometMarginFromLauncher);//mNormalizedCometDirection * cometMarginFromLauncher + Vector3::UNIT_Z * cometMarginFromLauncher);
		//mSceneNode = mSceneManager->getRootSceneNode()->createChildSceneNode(mPosition + mNormalizedCometDirection * cometMarginFromLauncher);
		//mCometNode->lookAt(player->getLocation(), Node::TransformSpace::TS_LOCAL);
		//mEntity->setCastShadows(false);
		//mCometNode->scale(10, 10, 10);
	//	mCometNode->setOrientation(mCometOrientation);

		//mCometNode->attachObject(mEntity);

		mStartedFlyingHelper = false;
		mIsFlying = true;
		mTimeCounter = 0;
	}
	else
	{
		if (mComet)
		{
			if (mTimeCounter <= cometTimeToLaunch)
			{
				mStartedFlyingHelper = false;
				mNormalizedCometDirection = player->getLocation() - mPosition;//mCometNode->_getDerivedPosition();
				mNormalizedCometDirection.normalise();
				//mSceneNode->setPosition(mPosition + mNormalizedCometDirection * cometMarginFromLauncher);
				
				mComet->cometNode->roll(evt.timeSinceLastFrame * Radian(Degree(360 * COMET_LAUNCHER_ROTATIONS_PER_LAUNCH).valueRadians()) / cometTimeToLaunch);
				//mComet->parentNode->yaw(evt.timeSinceLastFrame * Radian(Degree(360 * COMET_LAUNCHER_ROTATIONS_PER_LAUNCH).valueRadians()) / cometTimeToLaunch);
				//mComet->parentNode->pitch(evt.timeSinceLastFrame * Radian(Degree(360 * COMET_LAUNCHER_ROTATIONS_PER_LAUNCH).valueRadians()) / cometTimeToLaunch);
				
				//Vector3 axis = Vector3::UNIT_Z;
				//Radian angle;
				//axis.crossProduct(mNormalizedCometDirection);
				//axis.normalise();
				//angle = Ogre::Math::ACos(mNormalizedCometDirection.dotProduct(axis));

				mComet->parentNode->setOrientation(
					Vector3::UNIT_Z.getRotationTo(mNormalizedCometDirection)//, Vector3::UNIT_Z)
				);
				//mComet->parentNode->setOrientation(Quaternion(Math::ATan2(mNormalizedCometDirection.x, mNormalizedCometDirection.z), Vector3::UNIT_Y) * Quaternion(Math::ATan2(mNormalizedCometDirection.y, mNormalizedCometDirection.z), Vector3::NEGATIVE_UNIT_X));//);// * Quaternion(Math::ATan2(mNormalizedCometDirection.x, mNormalizedCometDirection.y), Vector3::UNIT_Z));
				//mComet->parentNode->setOrientation(Quaternion(mNormalizedCometDirection, mNormalizedCometDirection, mNormalizedCometDirection));//, Radian(0))));
				
				//mCometNode->lookAt(player->getLocation(), Node::TransformSpace::TS_WORLD);
			}
			else
			{
				if (!mStartedFlyingHelper)
				{
					mStartedFlyingHelper = true;
					mNormalizedCometDirection = player->getLocation() - (mPosition + mNormalizedCometDirection * cometMarginFromLauncher);
					mNormalizedCometDirection.normalise();
				}
				mComet->parentNode->translate(mNormalizedCometDirection * cometSpeed * evt.timeSinceLastFrame);
				if (mTimeCounter >= cometTimeMaxLifespan)
				{
					mIsFlying = false;
				}

				if (mComet->isTouchingObject(player->getAabbMin(), player->getAabbMax()))
				{
					player->shouldKillPlayer = true;
				}
			}

			mComet->updatePosition(mComet->cometNode->_getDerivedPosition());
		}
	}
}
#endif