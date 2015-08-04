#include "WorldSoul.h"
#include "Playground.h"
#include "balance.h"

WorldSoul::WorldSoul(Vector3 position)
	: WorldTouchableObject(position + Vector3(0, 30, 0), WORLD_SOUL_SIZE)
{
	SceneManager* manager = Root::getSingleton().getSceneManager(SCENE_MANAGER_NAME);
	mNode = manager->getRootSceneNode()->createChildSceneNode();
	position.y += 30; // base is position, 30 is height
	mNode->translate(position);
	mNode->scale(6, 6, 6);
	mEntity = manager->createEntity("soul.mesh");
	mEntity->setMaterialName("Items/Soul");
	mNode->attachObject(mEntity);
}

WorldSoul::~WorldSoul(void)
{
	Playground::destroySceneNode(mNode);
}

void WorldSoul::update(const Ogre::FrameEvent& evt)
{
	/*mNode->time
	mEntity->addtime*/
}

int WorldSoul::counter = 0;