#include "GameState.h"

GameState::GameState()
	: timeLeft(-1),
	secondsTotal(-1),
	hasTime(false),
	timeSincePlaying(0),
	world(1),
	level(1),
	currentScore(0),
	difficulty(D_EASY),
	maxScore(0)
{
}

GameState::~GameState(void)
{
}

String GameState::difficulty2string(GameState::Difficulty diff)
{
	switch (diff)
	{
	case D_VERY_EASY:
		return "Very Easy";
	case D_EASY:
		return "Easy";
	case D_NORMAL:
		return "Normal";
	case D_HARD:
		return "Hard";
	case D_VERY_HARD:
		return "Very Hard";
	}

	return "Error";
}