#include "WorldActivator.h"

#include "Playground.h"

WorldActivator::WorldActivator(ActivatorType type, Platform* platform)
	: WorldTouchableObject(Vector3::ZERO, WORLD_ACTIVATOR_SIZE),
	mIsActivated(false),
	isStartRespawnHelper(false),
	mActivatorType(type),
	mNode(0),
	mPlatformParent(platform)
{
	//: WorldTouchableObject(position + Vector3(0, WORLD_RESPAWN_SIZE.y / 2 + 20, 0), WORLD_ACTIVATOR_SIZE),

	position =
		TO_OGRE(mPlatformParent->getBox()->getBody()->getWorldTransform().getOrigin()) +
		Vector3(0, mPlatformParent->getBox()->size().y / 2, 0);

	SceneManager* manager = Root::getSingleton().getSceneManager(SCENE_MANAGER_NAME);
	mEntityActive = manager->createEntity("checkpoint-activated.mesh");
	mEntityNotActive = manager->createEntity("checkpoint-not-activated.mesh");
	
	setMaterial();
	
	mEntityActive->getAnimationState("up-and-down")->setEnabled(true);
	mEntityActive->getAnimationState("up-and-down")->setLoop(true);
	mEntityNotActive->getAnimationState("up-and-down")->setEnabled(true);
	mEntityNotActive->getAnimationState("up-and-down")->setLoop(true);

	if (mPlatformParent->getBox()->getNode() == 0)
	{
		mNode = manager->getRootSceneNode()->createChildSceneNode(position + Vector3(0, WORLD_ACTIVATOR_SIZE.y / 2, 0));
	}
	else
	{
		mNode = mPlatformParent->getBox()->getNode()->createChildSceneNode(
			Vector3(0, WORLD_ACTIVATOR_SIZE.y / 2 + mPlatformParent->getBox()->size().y / 2, 0)
		);
		//platform->getBox()->getNode()->addChild(mNode);
	}

	mNode->attachObject(mEntityActive);
	mNode->attachObject(mEntityNotActive);
	// TODO scale mesh in blender
	mNode->scale(.1, .1, .1);
	
	mNode->setVisible(false);

	mEntityNotActive->setVisible(false);
	mEntityActive->setVisible(false);
	mEntityNotActive->setCastShadows(true);
	mEntityActive->setCastShadows(true);
	static bool helperInit = false;
	if (!helperInit)
	{
		mEntityNotActive->getMesh()->setAutoBuildEdgeLists(true);
		mEntityNotActive->getMesh()->buildEdgeList();
		mEntityActive->getMesh()->setAutoBuildEdgeLists(true);
		mEntityActive->getMesh()->buildEdgeList();
		helperInit = true;
	}
	setActivation(mIsActivated);

	mAabbMin = position;
	mAabbMax = position;
	mAabbMin -= size / 2;
	mAabbMax += size / 2;
}

WorldActivator::~WorldActivator(void)
{
	Playground::destroySceneNode(mNode);
}

Vector3 WorldActivator::getBase()
{
	return position;// - Vector3(0, WORLD_ACTIVATOR_SIZE.y / 2, 0);
}

void WorldActivator::setVisible(bool visible)
{
	mNode->setVisible(visible);
}

void WorldActivator::setActivation(bool activate)
{
	mIsActivated = activate;
	if (activate && !mEntityActive->getVisible())
	{
		mEntityActive->setVisible(true);
		mEntityActive->getAnimationState("up-and-down")->setTimePosition(0);
		mEntityNotActive->setVisible(false);
	}
	else if (!activate && !mEntityNotActive->getVisible())
	{
		mEntityNotActive->setVisible(true);
		mEntityNotActive->getAnimationState("up-and-down")->setTimePosition(0);
		mEntityActive->setVisible(false);
	}
}

void WorldActivator::update(const Ogre::FrameEvent& evt)
{
	if (mEntityActive->getVisible())
	{
		mEntityActive->getAnimationState("up-and-down")->addTime(evt.timeSinceLastFrame);
	}
	if (mEntityNotActive->getVisible())
	{
		mEntityNotActive->getAnimationState("up-and-down")->addTime(evt.timeSinceLastFrame);
	}

	if (mPlatformParent->Type() == PlatformType::PT_MOVABLE)
	{
		position =
			TO_OGRE(mPlatformParent->getBox()->getBody()->getWorldTransform().getOrigin()) +
			Vector3(0, mPlatformParent->getBox()->size().y / 2, 0);
		mAabbMin = position;
		mAabbMax = position;
		mAabbMin -= size / 2;
		mAabbMax += size / 2;
	}
}

void WorldActivator::_resetTime()
{
	mEntityActive->getAnimationState("up-and-down")->setTimePosition(0);
	mEntityNotActive->getAnimationState("up-and-down")->setTimePosition(0);
}