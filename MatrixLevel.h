#pragma once

#include "PlatformGeneratorInterface.h"


class MatrixLevel : public PlatformGeneratorInterface
{
public:
	MatrixLevel(void);
	~MatrixLevel(void);
	virtual void fillPlatforms(GameState::Difficulty, InfiniteMiniWorldGenerator::GameMode, std::vector<Platform*>&);
};

