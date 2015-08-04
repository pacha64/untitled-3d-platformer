#pragma once
#include "WorldGenerator.h"

class TutorialWorldGenerator : public WorldGenerator
{
public:
	TutorialWorldGenerator(SceneManager*);
	~TutorialWorldGenerator(void);
	//virtual void generate();
	//virtual void update(const FrameEvent&, std::vector<Player*>*);
	//void generateNextBatch();
	virtual WorldGeneratorType getGeneratorType()
	{
		return WorldGeneratorType::WGT_TUTORIAL;
	}
private:
	int mCurrentLevel;
};

