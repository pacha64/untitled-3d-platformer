#include "MovablePlatform.h"
#include "Physics.h"
#include "TextRenderer.h"
#include "Globals.h"
#include "Player.h"

MovablePlatform::MovablePlatform(WorldBox* box)//, GameDifficulty difficulty)
	: Platform(box),
	mIsStandStartLinearMovement(0),
	//mDifficulty(difficulty),
	mDestiny(btVector3(0, 0, 0)),
	mTimeSinceStart(0),
	mLengthPath(0),
	mScaleLimit(1),
	mDestinyFlag(false),
	mIncreaseScaleFlag(false),
	mMovableFlags(0),
	mRotateSpeed(0),
	mLinearSpeed(0),
	mLastLinearTranslate(0, 0, 0),
	mOriginalRotationRadian(TO_OGRE(box->getBody()->getWorldTransform().getRotation()).getYaw().valueRadians()),
	mIsLinearMovement(true),
	mIsRotationRight(true),
	mStartMovement(false)
{
	//mMotionState = new MovablePlatformMotionState(mBox->getNode());
	mBox->getBody()->setActivationState(DISABLE_DEACTIVATION);
	//mBox->getBody()->setMotionState(mMotionState);
	mOrigin = box->getBody()->getWorldTransform().getOrigin();
	mDestiny = mOrigin;
	mLengthPath = 0;
	// TODO update rotation at runtime
	mInitRotation = TO_OGRE(mBox->getBody()->getWorldTransform().getRotation()).getYaw();

	//mBaseScaling = getBox()->getNode()->getScale();
}

MovablePlatform::~MovablePlatform(void)
{
}

void MovablePlatform::update(const FrameEvent& evt)
{
	if (!mActivator || (mActivator->getActivatorType() == WorldActivator::AT_POINTS && mActivator->isActivated()))
	{
		mTimeSinceStart += evt.timeSinceLastFrame;
		mLastLinearTranslate = btVector3(0, 0, 0);

		if (evt.timeSinceLastFrame < 0.00001)
		{
			return;
		}

		if (isFlagActive(MP_LINEAR) && (!mIsStandStartLinearMovement || mStartMovement))
		{
			linearUpdate(evt);
		}
		if (isFlagActive(MP_SCALING))
		{
			scaleUpdate(evt);
		}
		if (isFlagActive(MP_ROTATING))
		{
			rotateUpdate(evt);
		}

		mBox->getNode()->setOrientation(TO_OGRE(mBox->getBody()->getWorldTransform().getRotation()));
		mBox->getNode()->setPosition(TO_OGRE(mBox->getBody()->getWorldTransform().getOrigin()));
		if (mActivator)
		{
			mActivator->updatePosition(TO_OGRE(mBox->getBody()->getWorldTransform().getOrigin()));
		}
	}

	updateBreakble(evt);
}

void MovablePlatform::linearUpdate(const FrameEvent& evt)
{
	if (mIsLinearMovement)
	{
		btVector3 position = mBox->getBody()->getWorldTransform().getOrigin();
		btTransform newTransf = mBox->getBody()->getWorldTransform();
		btVector3 toIncrease = mDestiny - mOrigin;
		btScalar time = toIncrease.length() / mLinearSpeed;
		btScalar helper, helper2;
		helper2 = modf(mTimeSinceStart / time, &helper);
		btVector3 newPos = mOrigin;// = mOrigin + toIncrease * (time * (helper2));
		if ((int)(mTimeSinceStart / time) % 2 == 0)
		{
			newPos = mOrigin + toIncrease * (helper2);
		}
		else
		{
			newPos = mDestiny - toIncrease * (helper2);
		}

		mLastLinearTranslate = (newPos) - position;
		newTransf.setOrigin(newPos);
		mBox->getBody()->setWorldTransform(newTransf);
	}
	else
	{
		btVector3 position = mBox->getBody()->getWorldTransform().getOrigin();
		btTransform newTransf = mBox->getBody()->getWorldTransform();
		btVector3 toIncrease = mDestiny - mOrigin;
		btScalar time = toIncrease.length() / mLinearSpeed;
		btScalar helper, helper2;
		helper2 = modf(mTimeSinceStart / time, &helper);
		btVector3 newPos = mOrigin;// = mOrigin + toIncrease * (time * (helper2));
		
		newPos = mOrigin + TO_BULLET((Quaternion(Degree(360 * helper2 * (mIsRotationRight ? 1 : -1)), Vector3::UNIT_Y)) * TO_OGRE(toIncrease));

		mLastLinearTranslate = (newPos) - position;
		newTransf.setOrigin(newPos);
		newTransf.setRotation((btQuaternion(btVector3(0, 1, 0), (mInitRotation + Degree(360 * helper2 * (mIsRotationRight ? 1 : -1))).valueRadians())));
		mBox->getBody()->setWorldTransform(newTransf);
	}
}
void MovablePlatform::rotateUpdate(const FrameEvent& evt)
{
	btScalar rotateSpeed = getRotateSpeed().valueRadians() * evt.timeSinceLastFrame;
	/*
	btScalar helper;
	helper = modf(mTimeSinceStart, &helper);
	
	*/
	btTransform transf = getBox()->getBody()->getWorldTransform();
	Quaternion quat = TO_OGRE(transf.getRotation());
	btQuaternion quatBullet = btQuaternion(btVector3(0, 1, 0), mTimeSinceStart * getRotateSpeed().valueRadians());

	if (mBox->getMaterial() == WorldBox::CM_HORIZONTAL_VERTICAL_SPECIAL_1)
	{
		quatBullet *= btQuaternion(btVector3(1, 0, 0), mTimeSinceStart * getRotateSpeed().valueRadians());
	}
	
	transf.setRotation(quatBullet);
	mBox->getNode()->setOrientation(TO_OGRE(quatBullet));
	getBox()->getBody()->setWorldTransform(transf);//(btVector3(0, rotateSpeed, 0));
}

void MovablePlatform::scaleUpdate(const FrameEvent& evt)
{
	
#if MOVABLE_PLATFORMS_ENABLED
	btScalar toScale = getScaleFrequency() * evt.timeSinceLastFrame;
//	if (toScale > 1) toScale = 1;
	btVector3 scale = getBox()->getBody()->getCollisionShape()->getLocalScaling();
	if (
		(mScaleFlags.x() != 0 && scale.x() <= mScaleLimit + 0.0001) ||
		(mScaleFlags.y() != 0 && scale.y() <= mScaleLimit + 0.0001) ||
		(mScaleFlags.z() != 0 && scale.z() <= mScaleLimit + 0.0001))
	{
		mIncreaseScaleFlag = true;
	}
	else if (
		(mScaleFlags.x() != 0 && scale.x() >= 1) ||
		(mScaleFlags.y() != 0 && scale.y() >= 1) ||
		(mScaleFlags.z() != 0 && scale.z() >= 1))
	{
		mIncreaseScaleFlag = false;
	}

	btVector3 scaleFactor = mScaleFlags;
	if (!mIncreaseScaleFlag)
	{
		toScale *= -1;
	}

	scaleFactor.setX(scaleFactor.x() * (toScale / getBox()->getBoundingBox().x()));
	scaleFactor.setY(scaleFactor.y() * (toScale / getBox()->getBoundingBox().y()));
	scaleFactor.setZ(scaleFactor.z() * (toScale / getBox()->getBoundingBox().z()));

	btVector3 vectorScaleTotal = scale + scaleFactor;
	btDiscreteDynamicsWorld* world = Physics::getSingleton()->getWorld();
	btRigidBody* ptrRigidBody = getBox()->getBody();
	btCollisionObjectArray array = world->getCollisionObjectArray();
	if (vectorScaleTotal.x() <= 0.1 || vectorScaleTotal.y() <= 0.1 || vectorScaleTotal.y() <= 0.1)
	{
		for (int i = 0; i < array.size(); i++)
		{
			if (array[i] == ptrRigidBody)
			{
				world->removeRigidBody(ptrRigidBody);
				getBox()->getNode()->getAttachedObject(0)->setVisible(false);
			}
		}
	}
	else
	{
		bool alreadyAdded = false;
		for (int i = 0; i < array.size(); i++)
		{
			if (array[i] == ptrRigidBody)
			{
				alreadyAdded = true;
				break;
			}
		}
		if (!alreadyAdded)
		{
			world->addRigidBody(ptrRigidBody);
			getBox()->getNode()->getAttachedObject(0)->setVisible(true);
		}
	}

	// > 1
	if (vectorScaleTotal.x() > 1) vectorScaleTotal.setX(1);
	if (vectorScaleTotal.y() > 1) vectorScaleTotal.setY(1);
	if (vectorScaleTotal.z() > 1) vectorScaleTotal.setZ(1);
	// < 0
	if (vectorScaleTotal.x() < 0.00001) vectorScaleTotal.setX(0.00001);
	if (vectorScaleTotal.y() < 0.00001) vectorScaleTotal.setY(0.00001);
	if (vectorScaleTotal.z() < 0.00001) vectorScaleTotal.setZ(0.00001);

	getBox()->getBody()->getCollisionShape()->setLocalScaling(vectorScaleTotal);
	getBox()->getNode()->setScale(mBaseScaling * Vector3(vectorScaleTotal.x(), vectorScaleTotal.y(), vectorScaleTotal.z()));
#endif
}

void MovablePlatform::_resetTime()
{
	mStartMovement = false;
	mTimeSinceStart = 0;
	btTransform transf;// = mMovablePlatforms[i]->getBox()->getBody()->getWorldTransform();//mMovablePlatforms[i]->getBox()->getBody()->getWorldTransform().setOrigin(TO_BULLET(mMovablePlatforms[i]->getBox()->getNode()->getPosition()));
	transf.setOrigin(mOrigin);
//	transf.setOrigin((mMovablePlatforms[i]->getOrigin()));
	transf.setRotation(btQuaternion(btVector3(0, 1, 0), mOriginalRotationRadian));//TO_BULLET(Quaternion(Radian(0), Vector3::UNIT_Y)));
	getBox()->getBody()->setWorldTransform(transf);
	getBox()->getNode()->setOrientation(TO_OGRE(transf.getRotation()));
	getBox()->getNode()->setPosition(TO_OGRE(transf.getOrigin()));

	FrameEvent evt;
	evt.timeSinceLastEvent = 0.01;
	evt.timeSinceLastFrame = 0.01;
	update(evt);
	//if (mActivator)
	//{
	//	mActivator
	//}
}

void MovablePlatform::_setPosition(Vector3 position)
{
	Platform::_setPosition(position);
	auto destinyLength = mDestiny - mOrigin;
	mOrigin = mBox->getBody()->getWorldTransform().getOrigin();
	mDestiny = mOrigin + destinyLength;
}

void MovablePlatform::onTopHelper()
{
	if (!mStartMovement)
	{
		mStartMovement = true;
		mTimeSinceStart = 0.0;
	}
}

void MovablePlatform::setLinearMovementType(bool isLinear)
{
	auto oldIsLinear = mIsLinearMovement;
	mIsLinearMovement = isLinear;

	if (isLinear != oldIsLinear)
	{
		if (isLinear)
		{
		//	mDestiny = 
		}
		else
		{
		}
	}
}