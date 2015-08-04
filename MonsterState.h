#pragma once
#include "stdafx.h"

#include "Player.h"
#include "Monster.h"



class MonsterState
{
public:
	MonsterState(void);
	~MonsterState(void);
	void addMonster(Monster*);
	void update(Player*, const Ogre::FrameEvent& evt);
protected:
	std::vector<Monster*> mMonsters;
};

