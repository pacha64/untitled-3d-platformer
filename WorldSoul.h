#pragma once
#include "stdafx.h"
#include "WorldTouchableObject.h"



class WorldSoul : public WorldTouchableObject
{
public:
	WorldSoul(Vector3);
	~WorldSoul(void);
	void update(const Ogre::FrameEvent& evt);
	SceneNode* getNode()
	{
		return mNode;
	}
protected:
	static int counter;
	SceneNode* mNode;
	Entity* mEntity;
	//btPairCachingGhostObject* mGhostObject;
};

