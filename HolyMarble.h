#pragma once
#include "Monster.h"

class HolyMarble :
	public Monster
{
public:
	HolyMarble(void);
	~HolyMarble(void);
	//virtual MonsterType getMonsterType()
	//{
	//	return MT_HOLY_MARBLE;
	//}
//	virtual void update(Player*, const Ogre::FrameEvent& evt);
	inline Vector3 getPosition() { return mNode->_getDerivedPosition(); }
private:
	Entity* mEntity;
	SceneNode* mNode;
	btCollisionObject* mCollisionObject;
	btBoxShape* mBoxShape;
};