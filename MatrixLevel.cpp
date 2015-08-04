#include "MatrixLevel.h"


MatrixLevel::MatrixLevel(void)
{
}


MatrixLevel::~MatrixLevel(void)
{
}

bool isInsideRhombus(int x, int y, int matrixSize)
{
	x++;
	y++;
	matrixSize--;
	int xNumber = 0;
	if (x == matrixSize / 2 + 1)
	{
		return true;
	}
	else if (x <= matrixSize / 2)
	{
		xNumber = (x - 1) * 2 + 1;
	}
	else
	{
		xNumber = (matrixSize - (x - 1)) * 2 + 1;
	}

	xNumber -= 1;
	xNumber /= 2;
	xNumber += 1;
	
	int yNumber = 0;

	if (y == matrixSize / 2 + 1)
	{
		return true;
	}
	else if (y <= matrixSize / 2)
	{
		yNumber = matrixSize / 2 - (y - 1);
	}
	else
	{
		yNumber = (y - 1) - matrixSize / 2;
	}

	if (xNumber > yNumber) return true; else return false;
}

void MatrixLevel::fillPlatforms(GameState::Difficulty difficulty, InfiniteMiniWorldGenerator::GameMode mode, std::vector<Platform*>& platforms)
{
	// platform range = 3x3 - 6x6
	// distance 15 - 25 - 35
	// chance special platform (breakable, killbox, missing platform): 20% - easy, 40% normal, 80% hard
	// chance "diagonal cube shape": 30
	SEED_RAND();

	
	int rangeX = RANGE_RANDOM(3, 6), rangeY = RANGE_RANDOM(3, 6), distance = RANGE_RANDOM(1, 3), sizeX = 20, sizeY = 20;
	Vector3 size = WorldBox::typeSize(WorldBox::CustomMaterial::CM_TUTORIAL_PATH_1);
	
	switch (distance)
	{
	case 1:
		sizeX = size.x / 2 + 15;
		sizeY = size.z / 2 + 15;
		break;
	case 2:
		sizeX = size.x / 2 + 20;
		sizeY = size.z / 2 + 20;
		break;
	case 3:
		sizeX = size.x / 2 + 25;
		sizeY = size.z / 2 + 25;
		break;
	}
	

	bool diagonalCubeShape = RANGE_RANDOM(0, 10) <= 3;
	if (diagonalCubeShape)
	{
		if (rangeX % 2 == 0) rangeX--;
		rangeY = rangeX;
	}

	for (int x = 0; x < rangeX; x++)
	{
		for (int y = 0; y < rangeY; y++)
		{
			bool center = (x == rangeX / 2 && y == rangeY / 2);

			if (diagonalCubeShape && !isInsideRhombus(x, y, rangeX)) continue;
			btTransform transf;
			transf.setIdentity();
			transf.setOrigin(btVector3(x * sizeX , -100, y * sizeY ));
			auto plat = new Platform(new WorldBox(0, transf, WorldBox::CustomMaterial::CM_TUTORIAL_PATH_1, false));
			
			int randomSpecial = RANGE_RANDOM(0, 100);
			bool special = false;
			switch (difficulty)
			{
			case GameState::D_EASY:
				if (randomSpecial < 20)
				{
					special = true;
				}
				break;
			case GameState::D_NORMAL:
				if (randomSpecial < 40)
				{
					special = true;
				}
				break;
			case GameState::D_HARD:
				if (randomSpecial < 80)
				{
					special = true;
				}
				break;
			}

			if (special && !center)
			{
				switch (RANGE_RANDOM(1, 3))
				{
				case 1:
					plat->setBreakable(true);
					break;
				case 2:
					// TODO kill box
					break;
				case 3:
					delete plat;
					plat = 0;
					break;
				}
			}

			if (plat) platforms.push_back(plat);
			if (center && plat)
			{
				plat->addActivator(WorldActivator::AT_RESPAWN);
				plat->getActivator()->setVisible(false);
			}
			//if (rand()
		}
	}
}