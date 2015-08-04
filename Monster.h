#pragma once
#include "stdafx.h"
#include "WorldBox.h"
#include "WorldTouchableObject.h"

class Player;

class Monster
{
public:
	enum MonsterType
	{
		MT_BLACK_KING,
//		MT_HOLY_MARBLE,
		MT_SIZE
	};

	Monster();
	virtual ~Monster(void);
	virtual MonsterType getMonsterType() = 0;
	virtual void update(Player*, const Ogre::FrameEvent& evt) = 0;
	static void initialize(SceneManager* manager)
	{
		mSceneManager = manager;
	}
protected:
	static SceneManager* mSceneManager;
};

