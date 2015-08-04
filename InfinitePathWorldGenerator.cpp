#include "InfinitePathWorldGenerator.h"
#include "Player.h"
#include "GameState.h"

InfinitePathWorldGenerator::InfinitePathWorldGenerator(SceneManager* sceneManager)
	: mNextGoalPlatform(0),
	mGoalPlatform(0),
	mTimeSinceGoalReach(0),
	mPreviousGoalPlatform(0),
	WorldGenerator(sceneManager),
	mDifficulty(GameState::Difficulty::D_EASY)
{
	memset(mTotalPlatforms, 0.0, sizeof(mTotalPlatforms));
	mPreviousPlatforms.clear();
	mCurrentPlatforms.clear();
	mNextPlatforms.clear();
	mAabb[0].first = Vector3::ZERO;
	mAabb[0].second = Vector3::ZERO;
	mAabb[1].first = Vector3::ZERO;
	mAabb[1].second = Vector3::ZERO;
	mAabb[2].first = Vector3::ZERO;
	mAabb[2].second = Vector3::ZERO;
}


InfinitePathWorldGenerator::~InfinitePathWorldGenerator(void)
{
}

void InfinitePathWorldGenerator::generate()
{
	recreateSkyBox();

	mWorldTopRight = btVector3(2000000, 2000000, 2000000);
	mWorldBottomLeft = btVector3(-2000000, -2000000, -2000000);
	
	
	mStaticGeometry = mSceneManager->createStaticGeometry(requestNewStaticGeometryName());
	//mStaticGeometryCircle = mSceneManager->createStaticGeometry(requestNewStaticGeometryName());
	mStaticGeometry->setRegionDimensions (Vector3(2000000, 2000000, 2000000) * 2);
	//mStaticGeometryCircle->setRegionDimensions (Vector3(1000000, 1000000, 1000000));
	mStaticGeometry->setCastShadows(true);
	mStaticGeometry->build();

	generateNextBatch();
	generateNextBatch();

}

void InfinitePathWorldGenerator::update(const FrameEvent& evt, std::vector<Player*>* player)
{
	WorldGenerator::update(evt, player);

	mTimeSinceGoalReach += evt.timeSinceLastFrame;

	if ((*player)[0]->getController()->onGround() && (*player)[0]->getStandingPlatform() == mGoalPlatform)
	{
		mGameState->currentScore++;
		generateNextBatch();
	}
	if ((*player)[0]->getController()->onGround() && (*player)[0]->getStandingPlatform() == mNextGoalPlatform)
	{
		mGameState->currentScore++;
		mGameState->currentScore++;
		generateNextBatch();
		generateNextBatch();
	}
	
	if (mGameState->maxScore < mGameState->currentScore) mGameState->maxScore = mGameState->currentScore;

	DEBUG_PRINT(12, %d, mGameState->currentScore);
}

void InfinitePathWorldGenerator::generateNextBatch()
{
	int maxAngleRange;
	int walljumpPlatformHeight;
	int distanceBetweenPlatforms;
	int numPlatformsMin, numPlatformsMax;
	int platformDistanceMin, platformDistanceMax;
	int minBelowHeight, maxBelowHeight, maxHeight;
	Real distanceModifier;
	int movablePlatPercentage,
		breakablePlatPercentage,
		wallJumpPercentage,
		wallJumpKillboxPercentage,
		wallJumpKillboxNumberPercentage,
		maxWallJumps,
		wallJumpHeightRangeMin,
		wallJumpHeightRangeMax;

	switch (mDifficulty)
	{
	case GameState::Difficulty::D_VERY_HARD:
		maxAngleRange = 45;
		walljumpPlatformHeight = 5;
		distanceBetweenPlatforms = 7.5;
		numPlatformsMin = 8, numPlatformsMax = 11;
		platformDistanceMin = 25, platformDistanceMax = 30;
		minBelowHeight = -200;
		maxBelowHeight = -80;
		maxHeight = 12;
		distanceModifier = 1.1;
		movablePlatPercentage = 0;
		breakablePlatPercentage = 100;
		wallJumpPercentage = 80;
		wallJumpKillboxPercentage = 80;
		wallJumpKillboxNumberPercentage = 80;
		maxWallJumps = 5;
		wallJumpHeightRangeMin = -2;
		wallJumpHeightRangeMax = 0;
		break;
	case GameState::Difficulty::D_HARD:
		maxAngleRange = 45;
		walljumpPlatformHeight = 5;
		distanceBetweenPlatforms = 7.5;
		numPlatformsMin = 5, numPlatformsMax = 8;
		platformDistanceMin = 25, platformDistanceMax = 30;
		minBelowHeight = -150;
		maxBelowHeight = -50;
		maxHeight = 12;
		distanceModifier = 1;
		movablePlatPercentage = 0;
		breakablePlatPercentage = 50;
		wallJumpPercentage = 50;
		wallJumpKillboxPercentage = 50;
		wallJumpKillboxNumberPercentage = 50;
		maxWallJumps = 4;
		wallJumpHeightRangeMin = -2;
		wallJumpHeightRangeMax = 0;
		break;
	case GameState::Difficulty::D_NORMAL:
		maxAngleRange = 45;
		walljumpPlatformHeight = 5;
		distanceBetweenPlatforms = 7.5;
		numPlatformsMin = 4, numPlatformsMax = 7;
		platformDistanceMin = 25, platformDistanceMax = 30;
		minBelowHeight = -100;
		maxBelowHeight = -20;
		maxHeight = 7;
		distanceModifier = 0.9;
		movablePlatPercentage = 0;
		breakablePlatPercentage = 30;
		wallJumpPercentage = 25;
		wallJumpKillboxPercentage = 30;
		wallJumpKillboxNumberPercentage = 0;
		maxWallJumps = 2;
		wallJumpHeightRangeMin = 0;
		wallJumpHeightRangeMax = 0;
		break;
	case GameState::Difficulty::D_EASY:
		maxAngleRange = 45;
		walljumpPlatformHeight = 5;
		distanceBetweenPlatforms = 7.5;
		numPlatformsMin = 3, numPlatformsMax = 5;
		platformDistanceMin = 25, platformDistanceMax = 30;
		minBelowHeight = -80;
		maxBelowHeight = -20;
		maxHeight = 7;
		distanceModifier = 0.8;
		movablePlatPercentage = 0;
		breakablePlatPercentage = 20;
		wallJumpPercentage = 10;
		wallJumpKillboxPercentage = 0;
		wallJumpKillboxNumberPercentage = 0;
		maxWallJumps = 1;
		wallJumpHeightRangeMin = 0;
		wallJumpHeightRangeMax = 0;
		break;
	default:
	case GameState::Difficulty::D_VERY_EASY:
		maxAngleRange = 45;
		walljumpPlatformHeight = 5;
		distanceBetweenPlatforms = 7.5;
		numPlatformsMin = 3, numPlatformsMax = 4;
		platformDistanceMin = 25, platformDistanceMax = 30;
		minBelowHeight = -40;
		maxBelowHeight = -0;
		maxHeight = 6;
		distanceModifier = 0.8;
		movablePlatPercentage = 0;
		breakablePlatPercentage = 0;
		wallJumpPercentage = 0;
		wallJumpKillboxPercentage = 0;
		wallJumpKillboxNumberPercentage = 0;
		maxWallJumps = 0;
		wallJumpHeightRangeMin = 0;
		wallJumpHeightRangeMax = 0;
		break;
	}

	SEED_RAND();
	mPreviousGoalPlatform = mGoalPlatform;
	mGoalPlatform = mNextGoalPlatform;
	btVector3 startingPosition = btVector3(0, -30, -30);
	//startingPosition.setZero();
	if (mGoalPlatform)
	{
		startingPosition = (mGoalPlatform->getBox()->getBody()->getWorldTransform().getOrigin());
		if (mGoalPlatform->Type() == PlatformType::PT_MOVABLE)
		{
			startingPosition += TO_BULLET(static_cast<MovablePlatform*>(mGoalPlatform)->getLinearArgument());
		}
	}

	for (int i = 0; i < mPreviousPlatforms.size(); i++)
	{
		mPlatforms.erase(std::find(mPlatforms.begin(), mPlatforms.end(), mPreviousPlatforms[i]));
		if (mPreviousPlatforms[i]->Type() == PlatformType::PT_MOVABLE) mMovablePlatforms.erase(std::find(mMovablePlatforms.begin(), mMovablePlatforms.end(), mPreviousPlatforms[i]));
		delete mPreviousPlatforms[i];
	}
	mPreviousPlatforms.clear();

	mPreviousPlatforms = mCurrentPlatforms;
	mCurrentPlatforms = mNextPlatforms;
	mNextPlatforms.clear();

	btTransform transf;
	transf.setIdentity();
	int numPlatforms = RANGE_RANDOM(numPlatformsMin, numPlatformsMax);
	bool previousIsWallJump = false;
	Degree wallJumpAngle = Degree(0);
	int totalDistanceHelper = 0;
	Platform* oldPlatform = 0;
	for (int i = 0; i < numPlatforms; i++)
	{
		Real distance = RANGE_RANDOM(platformDistanceMin, platformDistanceMax);
		int height = RANGE_RANDOM(1, 8); // 1 -> below, 2 and 3 -> norma, rest -> up
		if (height == 1)
		{
			height  = RANGE_RANDOM(minBelowHeight, maxBelowHeight);
			distance = platformDistanceMin;
			distance += Math::Abs(height / 3);
		}
		else if (height == 2 && height == 3)
		{
			height  = RANGE_RANDOM(maxBelowHeight, 0);
			distance = 25;
			distance += Math::Abs(height / 2.5);
		}
		else
		{
			height = RANGE_RANDOM(0, maxHeight);
			if (height > 5) distance /= 2;
		}

		auto rotation = Quaternion(previousIsWallJump ? wallJumpAngle : Degree(RANGE_RANDOM(-maxAngleRange , maxAngleRange)), Vector3::UNIT_Y);
		startingPosition += TO_BULLET(rotation * Vector3(0, height, distance * distanceModifier));
		transf.setOrigin(startingPosition);
		transf.setRotation(TO_BULLET(rotation));
		auto plat = new Platform(new WorldBox(0, transf, WorldBox::CM_TUTORIAL_PATH_1, false));

		bool movingPlat = RANGE_RANDOM(1, 100) <= movablePlatPercentage;
		if (i + 1 != numPlatforms && movingPlat && height > -20 && height < 4)
		{
			WorldBox* box = plat->getBox();
			plat->_prepareForUpcast();
			delete plat;
			auto movablePlat = new MovablePlatform(box);
			auto zLocation = (RANGE_RANDOM(1, 2) + 1) * 15, xLocation = (RANGE_RANDOM(1, 2) + 1) * 15;
			if (zLocation / 2 < xLocation) xLocation = zLocation / 2;
			auto linearArg = TO_BULLET((rotation) * Vector3(0, 0, zLocation));//xLocation);
			movablePlat->setLinearMovementArgs(linearArg, 20);
			movablePlat->addFlag(MovablePlatform::MP_LINEAR);
			movablePlat->setIsStandStart(true);
			startingPosition += linearArg ; // 2 units margin
			mNextPlatforms.push_back(movablePlat);
		}
		else
		{
			if (height > -80 && RANGE_RANDOM(1, 100) <= breakablePlatPercentage) plat->setBreakable(true);
			//else plat->setIce(true);
			
			mNextPlatforms.push_back(plat);
		}

		if (i + 1 != numPlatforms && RANGE_RANDOM(1, 100) <= wallJumpPercentage)
		{
			previousIsWallJump = true;
			wallJumpAngle = Degree(RANGE_RANDOM(-maxAngleRange , maxAngleRange));

			int randomWall = 1, counter = 1;

			while (counter <= maxWallJumps && randomWall == 1)
			{
				if (counter == 1)
				{
					startingPosition += TO_BULLET(Quaternion(wallJumpAngle , Vector3::UNIT_Y) * Vector3(distanceBetweenPlatforms , walljumpPlatformHeight, 20));
				}
				else
				{
					auto wallJumpHeight = (walljumpPlatformHeight + RANGE_RANDOM_FLOAT(-1.0, 1.0));
					auto rangeDistance = (RANGE_RANDOM(wallJumpHeightRangeMin, wallJumpHeightRangeMax));
					startingPosition += TO_BULLET(
						Quaternion(wallJumpAngle , Vector3::UNIT_Y) *
						Vector3(
							distanceBetweenPlatforms * 2 * (counter % 2 == 1 ? 1 : -1),
							(rangeDistance * wallJumpHeight),
							rangeDistance <= 0 ? Math::Abs(rangeDistance) * wallJumpHeight / 2.0 + RANGE_RANDOM(15, 25) : (rangeDistance) * wallJumpHeight + RANGE_RANDOM(5, 10)
						)
					);
				}
				transf.setOrigin(startingPosition);
				transf.setRotation(btQuaternion(btVector3(0, 1, 0), wallJumpAngle.valueRadians()));
				
				totalDistanceHelper++;

				auto plat = new Platform(new WorldBox(0, transf, WorldBox::CM_TUTORIAL_PATH_4_TOP_KILLBOX, false));
				//startingPosition += TO_BULLET(Quaternion(wallJumpAngle , Vector3::UNIT_Y) * Vector3(0, 10, 20));
				mNextPlatforms.push_back(plat);

				randomWall = RANGE_RANDOM(0, 100) <= wallJumpPercentage ? 1 : 0;

				if (randomWall != 1 && counter == 1 && RANGE_RANDOM(1, 100) <= wallJumpKillboxPercentage)
				{
					int number = 1;
					while (RANGE_RANDOM(0, 100) <= wallJumpKillboxNumberPercentage)
					{
						number++;
						if (number == 3) break;
					}
					auto distance = RANGE_RANDOM_FLOAT(8, 12), speed = RANGE_RANDOM_FLOAT(2, 3.5);

					for (int i = 0; i < number; i++)
					{
						auto angle = Degree(360.0 / number * i);
						auto newTransf = transf;
						newTransf.setOrigin(transf.getOrigin() + btVector3(0, RANGE_RANDOM(0, 5), 0));
						newTransf.setRotation(btQuaternion(btVector3(0, 1, 0), (angle + Degree(90)).valueRadians()));
						auto movablePlat = new MovablePlatform(new WorldBox(0, newTransf, WorldBox::CM_TUTORIAL_PATH_4_KILLBOX, false));
						movablePlat->setLinearMovementType(false);
						movablePlat->setLinearMovementArgs(TO_BULLET((Quaternion(angle , Vector3::UNIT_Y) * Vector3(0, 0, distance))), speed);
						movablePlat->addFlag(MovablePlatform::MovablePlatformFlag::MP_LINEAR);
						mNextPlatforms.push_back(movablePlat);
					}
				}

				counter++;
			}
		}

		if (previousIsWallJump) startingPosition -= btVector3(0, walljumpPlatformHeight, 0);
		
		totalDistanceHelper ++;
		oldPlatform = plat;

		//startingPosition += btVector3(0, height, 0);
		//transf.setRotation(TO_BULLET(rotation));
	}

	mNextGoalPlatform = mNextPlatforms[mNextPlatforms.size() - 1];
	if (mNextGoalPlatform->isBreakable()) mNextGoalPlatform->setBreakable(false);

	mPlatforms.insert(mPlatforms.end(), mNextPlatforms.begin(), mNextPlatforms.end());
	for (int i = 0; i < mNextPlatforms.size(); i++)
	{
		if (mNextPlatforms[i]->Type() == PlatformType::PT_MOVABLE) mMovablePlatforms.push_back(static_cast<MovablePlatform*>(mNextPlatforms[i]));
	}

	mTotalPlatforms[2] = mTotalPlatforms[1];
	mTotalPlatforms[1] = mTotalPlatforms[0];
	mTotalPlatforms[0] = totalDistanceHelper;
	if (mGameState) mGameState->timeLeft = calculateTimeLeft();
	// TODO movable platform aabb
	mAabb[2] = mAabb[1];
	mAabb[1] = mAabb[0];
	Vector3 aabbMin = mNextPlatforms[0]->getBox()->getNode()->_getDerivedPosition() - mPlatforms[0]->getBox()->size() / 2;
	Vector3 aabbMax = mNextPlatforms[0]->getBox()->getNode()->_getDerivedPosition() + mPlatforms[0]->getBox()->size() / 2;
	for (int i = 1; i < mNextPlatforms.size(); i++)
	{
		Vector3 aabbMinHelper = mNextPlatforms[i]->getBox()->getNode()->_getDerivedPosition() - mNextPlatforms[i]->getBox()->size() / 2;
		Vector3 aabbMaxHelper = mNextPlatforms[i]->getBox()->getNode()->_getDerivedPosition() + mNextPlatforms[i]->getBox()->size() / 2;

		if (aabbMin.x > aabbMinHelper.x) aabbMin.x = aabbMinHelper.x;
		if (aabbMin.y > aabbMinHelper.y) aabbMin.y = aabbMinHelper.y;
		if (aabbMin.z > aabbMinHelper.z) aabbMin.z = aabbMinHelper.z;
		if (aabbMax.x < aabbMaxHelper.x) aabbMax.x = aabbMaxHelper.x;
		if (aabbMax.y < aabbMaxHelper.y) aabbMax.y = aabbMaxHelper.y;
		if (aabbMax.z < aabbMaxHelper.z) aabbMax.z = aabbMaxHelper.z;
	}
	auto worldAabbMargin = Vector3(WORLD_AABB_MARGIN, WORLD_AABB_MARGIN, WORLD_AABB_MARGIN);
	mAabb[0].first = aabbMin - worldAabbMargin;
	mAabb[0].second = aabbMax + worldAabbMargin;

	mWorldBottomLeft = TO_BULLET(mAabb[0].first);
	mWorldTopRight = TO_BULLET(mAabb[0].second);
	if (mAabb[1].first.length() > 0 && mAabb[1].second.length() > 0)
	{
		if (mWorldBottomLeft[0] > mAabb[1].first.x) mWorldBottomLeft[0] = mAabb[1].first.x;
		if (mWorldBottomLeft[1] > mAabb[1].first.y) mWorldBottomLeft[1] = mAabb[1].first.y;
		if (mWorldBottomLeft[2] > mAabb[1].first.z) mWorldBottomLeft[2] = mAabb[1].first.z;
		if (mWorldTopRight[0] < mAabb[1].second.x) mWorldTopRight[0] = mAabb[1].second.x;
		if (mWorldTopRight[1] < mAabb[1].second.y) mWorldTopRight[1] = mAabb[1].second.y;
		if (mWorldTopRight[2] < mAabb[1].second.z) mWorldTopRight[2] = mAabb[1].second.z;
	}
	if (mAabb[2].first.length() > 0 && mAabb[2].second.length() > 0)
	{
		if (mWorldBottomLeft[0] > mAabb[2].first.x) mWorldBottomLeft[0] = mAabb[2].first.x;
		if (mWorldBottomLeft[1] > mAabb[2].first.y) mWorldBottomLeft[1] = mAabb[2].first.y;
		if (mWorldBottomLeft[2] > mAabb[2].first.z) mWorldBottomLeft[2] = mAabb[2].first.z;
		if (mWorldTopRight[0] < mAabb[2].second.x) mWorldTopRight[0] = mAabb[2].second.x;
		if (mWorldTopRight[1] < mAabb[2].second.y) mWorldTopRight[1] = mAabb[2].second.y;
		if (mWorldTopRight[2] < mAabb[2].second.z) mWorldTopRight[2] = mAabb[2].second.z;
	}
}

bool InfinitePathWorldGenerator::isInsideWorld(Vector3 aabbMax, Vector3 aabbMin)
{
	for (int i = 0; i < mPlatforms.size(); i++)
	{
		auto pos = mPlatforms[i]->getBox()->getNode()->getPosition();
		if (pos.y - mPlatforms[i]->getBox()->size().y / 2 < aabbMax.y + WORLD_AABB_MARGIN &&
			(Vector2(pos.x, pos.z) - Vector2(aabbMin.x, aabbMin.z)).length() < WORLD_AABB_MARGIN * 5)
		{
			return true;
		}
	}

	return false;
	//return WorldGenerator::isInsideWorld(aabbMax, aabbMin);

	// TODO better "inside world" algorithm, ditch aabb and detect when the player can't save itself (land on a platform)
	for (int i = 0; i < sizeof(mAabb)/sizeof(mAabb[0]); i++)
	{
		if ((mAabb[i].second.x >= aabbMax.x && mAabb[i].first.x <= aabbMin.x) &&
			(mAabb[i].second.y >= aabbMax.y && mAabb[i].first.y <= aabbMin.y) &&
			(mAabb[i].second.z >= aabbMax.z && mAabb[i].first.z <= aabbMin.z))
		{
			return true;
		}
	}
	
	return false;
}

void InfinitePathWorldGenerator::platformShaderArgs()
{
	WorldGenerator::platformShaderArgs();

	auto vector4 = mGoalPlatform->getBox()->getEntity()->getSubEntity(0)->getCustomParameter(1);
	vector4.x = Platform::ShaderColors::SC_GOAL;
	mGoalPlatform->getBox()->getEntity()->getSubEntity(0)->setCustomParameter(1, vector4);

	vector4 = mNextGoalPlatform->getBox()->getEntity()->getSubEntity(0)->getCustomParameter(1);
	vector4.x = Platform::ShaderColors::SC_GOAL;
	mNextGoalPlatform->getBox()->getEntity()->getSubEntity(0)->setCustomParameter(1, vector4);

	if (mPreviousGoalPlatform)
	{
		vector4 = mPreviousGoalPlatform->getBox()->getEntity()->getSubEntity(0)->getCustomParameter(1);
		vector4.x = Platform::ShaderColors::SC_GOAL;
		mPreviousGoalPlatform->getBox()->getEntity()->getSubEntity(0)->setCustomParameter(1, vector4);
	}
}

Real InfinitePathWorldGenerator::calculateTimeLeft()
{
	return (getPlatformsToGoal() * WORLD_TIME_PER_UNIT);
}

Vector3 InfinitePathWorldGenerator::worldRespawn2vector(WorldActivator*)
{
	auto plat = mCurrentPlatforms[0];
	if (mPreviousPlatforms.size() > 0)
	{
		plat = mPreviousPlatforms[mPreviousPlatforms.size()-1];
	}
	return plat->getBox()->getNode()->_getDerivedPosition() + Vector3(0, RESPAWN_Y_MARGIN, 0);
}

Vector3 InfinitePathWorldGenerator::getInitialRespawn()
{
	return worldRespawn2vector(NULL);
}

Vector3 InfinitePathWorldGenerator::getRandomRespawn()
{
	return worldRespawn2vector(NULL);
}