#include "WorldEditor.h"

WorldEditor::WorldEditor(SceneManager* sceneManager)
	: WorldGenerator(sceneManager)
{
}

WorldEditor::~WorldEditor(void)
{
}

void WorldEditor::update(const FrameEvent& evt, std::vector<Player*>* players)
{
	if (mIsEditing)
	{
	}
	else
	{
		WorldGenerator::update(evt, players);
	}
}

MovablePlatform* WorldEditor::upcastPlatform(Platform* plat)
{
	for (int i = 0; i < mPlatforms.size(); i++)
	{
		if (mPlatforms[i] == plat)
		{
			mPlatforms.erase(mPlatforms.begin() + i);
			break;
		}
	}
	WorldBox* box = plat->getBox();
	WorldActivator* activator = plat->getActivator();
	plat->_prepareForUpcast();
	delete plat;
	MovablePlatform* movablePlat = new MovablePlatform(box);
	mPlatforms.push_back(movablePlat);
	mMovablePlatforms.push_back(movablePlat);
	if (activator)
	{
		movablePlat->_setActivator(activator);
	}
	return movablePlat;
}

Platform* WorldEditor::newPlatform(Vector3 origin, Degree rotation)
{
	if (mPlatforms.size() >= MAX_PLATFORMS_NUM) return 0;
	btTransform transf;
	transf.setOrigin(TO_BULLET(origin));
	transf.setRotation(btQuaternion(btVector3(0, 1, 0), rotation.valueRadians()));
	Platform* plat = new Platform(new WorldBox(0, transf, WorldBox::CM_TUTORIAL_PATH_1, false));
	mPlatforms.push_back(plat);

#if !FIX_INSERT_PLATFORM_EDITOR
	plat->getBox()->insertFix = true;
#endif

	return plat;
}

void WorldEditor::deletePlatform(Platform* plat)
{
	if (plat->getActivator())
	{
		if (plat->getActivator() == mCurrentPlayerActivator)
		{
			mCurrentPlayerActivator = 0;
		}
		if (plat->getActivator() == mFirstRespawn)
		{
			mFirstRespawn = 0;
		}
		if (plat->getActivator()->getActivatorType() == WorldActivator::AT_POINTS)
		{
			mPoints.erase(remove(mPoints.begin(), mPoints.end(), plat->getActivator()), mPoints.end());
		}
		else if (plat->getActivator()->getActivatorType() == WorldActivator::AT_RESPAWN)
		{
			mRespawns.erase(remove(mRespawns.begin(), mRespawns.end(), plat->getActivator()), mRespawns.end());
		}
	}
	if (plat->Type() == PlatformType::PT_MOVABLE)
	{
		for (int i = 0; i < mMovablePlatforms.size(); i++)
		{
			if (mMovablePlatforms[i] == plat)
			{
				mMovablePlatforms.erase(mMovablePlatforms.begin() + i);
				break;
			}
		}
	}
	for (int i = 0; i < mPlatforms.size(); i++)
	{
		if (mPlatforms[i] == plat)
		{
			delete mPlatforms[i];
			mPlatforms.erase(mPlatforms.begin() + i);
			break;
		}
	}
}

void WorldEditor::setEditing(bool isEditing)
{
	mIsEditing = isEditing;

	for (int i = 0; i < mRespawns.size(); i++)
	{
		mRespawns[i]->_resetTime();
	}
	for (int i = 0; i < mPoints.size(); i++)
	{
		mPoints[i]->_resetTime();
	}
	resetAllMovablePlatforms();

	if (mCurrentPlayerActivator) mCurrentPlayerActivator->setActivation(!isEditing);
	if (mFirstRespawn) mFirstRespawn->setActivation(isEditing);
}

void WorldEditor::setInitialRespawn(Platform* plat)
{
	if (mFirstRespawn)
	{
		mFirstRespawn->setActivation(false);
		mFirstRespawn->isStartRespawnHelper = false;
	}
	mFirstRespawn = plat->getActivator();
	mFirstRespawn->setActivation(true);
	mFirstRespawn->isStartRespawnHelper = true;
}

void WorldEditor::reloadMapFormat()
{
	mCurrentFormat->blocks.clear();
	mCurrentFormat->firstRespawnOffset = -1;
	for (int i = 0; i < mPlatforms.size(); i++)
	{
		MapFormat::Block* block = new MapFormat::Block();
//		block->size = mPlatforms[i]->getBox()->size();
		block->origin = mPlatforms[i]->getBox()->getNode()->getPosition();
		block->rotation = mPlatforms[i]->getBox()->getNode()->getOrientation().getYaw();
		block->materialIndex = mPlatforms[i]->getBox()->getMaterial();

		if (mPlatforms[i]->getActivator())
		{
			if (mPlatforms[i]->getActivator()->getActivatorType() == WorldActivator::AT_RESPAWN)
			{
				block->isRespawn = true;
				if (mPlatforms[i]->getActivator()->isStartRespawnHelper) mCurrentFormat->firstRespawnOffset = i;
			}
			else if (mPlatforms[i]->getActivator()->getActivatorType() == WorldActivator::AT_POINTS)
			{
				block->isPoint = true;
			}
		}

		if (mPlatforms[i]->Type() == PlatformType::PT_MOVABLE)
		{
			MovablePlatform* plat = static_cast<MovablePlatform*>(mPlatforms[i]);
			if (plat->getLinearSpeed() > 0 && plat->getLinearArgument().length() > 0)
			{
				block->linearLocation = plat->getLinearArgument();
				block->linearSpeed = plat->getLinearSpeed();
				if (!plat->getIsLinearMovementType())
				{
					block->isRotationMovement = true;
					block->isRightRotationMovement = plat->getIsRotationRight();
				}
			}
			block->rotationSpeed = (int)fixAngleNumber(plat->getRotateSpeed().valueDegrees());
		}

		/*if (mPlatforms[i]->isBreakable())
		{
			block->extraArgsBit |= 1 << 1;
		}
		if (mPlatforms[i]->isKillBox())
		{
			block->extraArgsBit |= 1 << 2;
		}*/

		mCurrentFormat->blocks.push_back(block);
	}
}