#include "MonsterState.h"


MonsterState::MonsterState(void)
{
}


MonsterState::~MonsterState(void)
{
}

void MonsterState::addMonster(Monster* monster)
{
	mMonsters.push_back(monster);
}

void MonsterState::update(Player* player, const Ogre::FrameEvent& evt)
{
	for (int i = 0; i < mMonsters.size(); i++)
	{
		mMonsters[i]->update(player, evt);
	}
}