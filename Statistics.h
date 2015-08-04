#pragma once
#include "GameState.h"

struct Statistics
{
public:
	struct GameHistory
	{
		GameHistory()
		{
			maxScore = 0;
			timePlayed = 0;
			numberOfGames = 0;
		}
		int numberOfGames;
		int maxScore;
		Real timePlayed;
	} scoreHistory[GameState::Difficulty::D_SIZE];
	struct TotalActions
	{
		TotalActions()
		{
			longJumps = 0;
			normalJumps = 0;
			distanceAir = 0;
			distanceWalking = 0;
			deaths = 0;
			longWallJumps = 0;
			normalWallJumps = 0;
		}
		int longJumps;
		int normalJumps;
		int normalWallJumps;
		int longWallJumps;
		Real distanceAir, distanceWalking;
		int deaths;
		int allJumps() { return longJumps + normalJumps + longWallJumps + normalWallJumps; }
	} totalActions;
	Statistics(void);
	~Statistics(void);

	void saveConfig(String);
	void loadConfig(String);
};

