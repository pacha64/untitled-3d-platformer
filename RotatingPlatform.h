#pragma once
#include "MovablePlatform.h"

class RotatingPlatform : public MovablePlatform
{
public:
	RotatingPlatform(WorldBox* box, GameDifficulty difficulty);
	~RotatingPlatform();
	virtual void update(const FrameEvent& evt);
	virtual MovablePlatformType getPlatformType()
	{
		return MP_ROTATING;
	}
protected:
	btScalar getSpeed()
	{
		switch (mDifficulty)
		{
		case GD_EASY:
			return Radian(Degree(30)).valueRadians();
			break;
		case GD_NORMAL:
			return Radian(Degree(45)).valueRadians();
			break;
		case GD_HARD:
			return Radian(Degree(60)).valueRadians();
			break;
		}
	}
};

