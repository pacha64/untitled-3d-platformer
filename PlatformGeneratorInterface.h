#pragma once
#include "stdafx.h"
#include "Platform.h"
#include "InfiniteMiniWorldGenerator.h"
#include "GameState.h"

enum GameMode;

class PlatformGeneratorInterface
{
public:
	virtual void fillPlatforms(GameState::Difficulty, InfiniteMiniWorldGenerator::GameMode, std::vector<Platform*>&) = 0;
};

