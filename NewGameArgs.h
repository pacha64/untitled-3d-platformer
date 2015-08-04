#pragma once
#include "MapFormat.h"
#include "WorldGenerator.h"
#include "InfinitePathWorldGenerator.h"
#include "InfiniteMiniWorldGenerator.h"
#include "GameState.h"

struct NewGameArgs
{
	NewGameArgs()
	{
		type = WorldGenerator::WorldGeneratorType::WGT_GENERIC;
		difficulty = GameState::Difficulty::D_NORMAL;
		miniWorldMode = InfiniteMiniWorldGenerator::GameMode::GM_CHECKPOINTS;
		optionalMapFormat = 0;
		usesTime = false;
	}
	WorldGenerator::WorldGeneratorType type;
	GameState::Difficulty difficulty;
	InfiniteMiniWorldGenerator::GameMode miniWorldMode;
	MapFormat* optionalMapFormat;
	bool usesTime;
};
