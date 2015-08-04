#pragma once
#include "MovablePlatform.h"
#include "MovablePlatformMotionState.h"

class LinearPlatform : public MovablePlatform
{
public:
	LinearPlatform(WorldBox* box, btVector3& translate, GameDifficulty difficulty);
	~LinearPlatform();
	virtual void update(const FrameEvent& evt);
protected:
};

