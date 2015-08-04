#pragma once
#include "WorldGenerator.h"

class InfiniteMiniWorldGenerator : public WorldGenerator
{
public:
	enum GameMode
	{
		GM_STAND_ALL_PLATFORMS,
		GM_CHECKPOINTS,
		GM_GOAL_PLATFORM
	};
	InfiniteMiniWorldGenerator(SceneManager*);
	~InfiniteMiniWorldGenerator(void);
	virtual void generate();
};