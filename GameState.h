#pragma once
#include "MapFormat.h"

struct GameState
{
public:
	enum Difficulty
	{
		D_VERY_EASY,
		D_EASY,
		D_NORMAL,
		D_HARD,
		D_VERY_HARD,
		D_SIZE
	};
	GameState();
	~GameState(void);
	bool hasTime;
	//int currentScore;
	int secondsTotal;
	int world, level, currentScore, maxScore;
	//bool hasReachedGoal;
	double timeSincePlaying;
	Difficulty difficulty;
	Real timeLeft;
	static String difficulty2string(Difficulty);
private:
};