#pragma once
#include "stdafx.h"
#include "Platform.h"
#include "WorldBox.h"
#include "balance.h"

class Player;

#define MOVABLE_PLATFORMS_ENABLED 0

class MovablePlatform : public Platform
{
public:
	enum MovablePlatformFlag
	{
		MP_ROTATING = 0x01,
		MP_LINEAR = 0x02,
		MP_SCALING = 0x04
	};
	MovablePlatform(WorldBox* box);//, WorldGenerator::GameDifficulty difficulty);
	~MovablePlatform(void);

	virtual void update(const FrameEvent& evt);
	void _resetTime();
	virtual void _setPosition(Vector3 position);
	void onTopHelper();

	virtual PlatformType Type()
	{
		return PlatformType::PT_MOVABLE;
	}
	btVector3 getLastLinearTranslate()
	{
		//return mBox->getBody()->getWorldTransform().getOrigin() + btVector3(0, WorldBox::typeSize(mBox->getBoxType()).y() / 2, 0);
		return mLastLinearTranslate;
	}
	void addFlag(MovablePlatformFlag flag)
	{
		mMovableFlags |= flag;
		getBox()->getBody()->setActivationState(DISABLE_DEACTIVATION);
	}
	bool isFlagActive(MovablePlatformFlag flag)
	{
		if (mMovableFlags & flag)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	void removeFlag(MovablePlatformFlag flag)
	{
		mMovableFlags ^= flag;
	}
	Real getLinearSpeed()
	{
		return mLinearSpeed;
	}
	void setLinearSpeed(btScalar speed)
	{
		mLinearSpeed = speed;
	}
	void setIsStandStart(bool is) { mIsStandStartLinearMovement = is; }
	void setLinearMovementArgs(btVector3& translate, btScalar speed)
	{
		mLinearSpeed = speed;
		mDestiny = mOrigin + translate;
		mLengthPath = mOrigin.distance(mDestiny);
	}
	Vector3 getLinearArgument()
	{
		return TO_OGRE(mDestiny - mOrigin);
	}
	void setRotateArgs(Degree speed)
	{
		mRotateSpeed = speed;
	}
	Degree getRotateSpeed()
	{
		return mRotateSpeed;
	}
	void setScaleArguments(btScalar limit, btVector3& flags)
	{
		mScaleFlags = btVector3(0, 0, 0);
		if (flags.x())
		{
			mScaleFlags.setX(1);
		}
		if (flags.y())
		{
			mScaleFlags.setY(1);
		}
		if (flags.z())
		{
			mScaleFlags.setZ(1);
		}
		mScaleLimit = limit;
	}
	bool _validMovable()
	{
		return mRotateSpeed != Degree(0) || (mLinearSpeed > 0 && (mDestiny - mOrigin).length() > 0);
	}
	void _setActivator(WorldActivator* activator)
	{
		mActivator = activator;
		mActivator->_newPlatform(this);
	}
	btVector3& getOrigin() { return mOrigin; }

	bool getIsLinearMovementType()
	{
		return mIsLinearMovement;
	}
	void setLinearMovementType(bool isLinear);
	bool getIsRotationRight()
	{
		return mIsRotationRight;
	}
	void setRotationRight(bool isRight)
	{
		mIsRotationRight = isRight;
	}

protected:
	bool
		mIncreaseScaleFlag,
		mDestinyFlag,
		mIsLinearMovement,
		mIsRotationRight,
		mIsStandStartLinearMovement,
		mStartMovement;
	btVector3
		mLastLinearTranslate,
		mOrigin,
		mDestiny,
		mScaleFlags;
	btScalar
		mLengthPath,
		mLinearSpeed,
		mScaleLimit,
		mTimeSinceStart,
		mOriginalRotationRadian;
	Vector3 mBaseScaling;
	Degree mRotateSpeed, mInitRotation;

	void linearUpdate(const FrameEvent& evt);
	void rotateUpdate(const FrameEvent& evt);
	void scaleUpdate(const FrameEvent& evt);
	int mMovableFlags;
	//WorldGenerator::GameDifficulty mDifficulty;
};

