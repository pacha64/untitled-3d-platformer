#include "RotatingPlatform.h"


RotatingPlatform::RotatingPlatform(WorldBox* box, GameDifficulty difficulty)
	:MovablePlatform(box, difficulty)
{
	mBox->getBody()->setLinearFactor(btVector3(0, 1, 0));
}

RotatingPlatform::~RotatingPlatform(void)
{
}

void RotatingPlatform::update(const FrameEvent& evt)
{
	mBox->getBody()->setAngularVelocity(btVector3(0, getSpeed(), 0));
	//mBox->getNode()->yaw(evt.timeSinceLastFrame * getSpeed(), Node::TS_LOCAL);
}