#include "LinearPlatform.h"

LinearPlatform::LinearPlatform(WorldBox* box, btVector3& translate, GameDifficulty difficulty)
	:MovablePlatform(box, difficulty),
	mDestinyFlag(true)
{
	mOrigin = box->getBody()->getWorldTransform().getOrigin();
}


LinearPlatform::~LinearPlatform(void)
{
}

void LinearPlatform::update(const FrameEvent& evt)
{
	btVector3 position = mBox->getBody()->getWorldTransform().getOrigin();
	btScalar distance;
	if (mDestinyFlag)
	{
		distance = mOrigin.distance(position);
		if (distance >= mLengthPath - 0.5)
		{
			mDestinyFlag = !mDestinyFlag;
		}
	}
	else
	{
		distance = mDestiny.distance(position);
		if (distance >= mLengthPath - 0.5)
		{
			mDestinyFlag = !mDestinyFlag;
		}
	}
	btVector3 translate;
	if (mDestinyFlag)
	{
		translate = mDestiny - mOrigin;
		translate = translate.normalize();
	}
	else
	{
		translate = mOrigin - mDestiny;
		translate = translate.normalize();
	}
	btScalar speed = getSpeed();
	translate.setX(translate.x() * speed);
	translate.setY(translate.y() * speed);
	translate.setZ(translate.z() * speed);
	mBox->getBody()->setLinearVelocity(translate);
	/*static int nigger = 0;
	if (nigger % 1000 < 500)
	{
		getBox()->getBody()->getCollisionShape()->setLocalScaling(getBox()->getBody()->getCollisionShape()->getLocalScaling() + btVector3(1* evt.timeSinceLastFrame, 0, 1 * evt.timeSinceLastFrame));
	}
	else
	{
		getBox()->getBody()->getCollisionShape()->setLocalScaling(getBox()->getBody()->getCollisionShape()->getLocalScaling() + btVector3(-1 * evt.timeSinceLastFrame, 0, -1 * evt.timeSinceLastFrame));
	}
	nigger++;*/
}