#pragma once
#include "stdafx.h"
#include "balance.h"
#include "Keys.h"

class Platform;
class Monster;
class Player;

class btCollisionShape;
class btConvexShape;
class btRigidBody;
class btCollisionWorld;
class btCollisionDispatcher;

///btKinematicCharacterController is an object that supports a sliding motion in a world.
///It uses a ghost object and convex sweep test to test for upcoming collisions. This is combined with discrete collision detection to recover from penetrations.
///Interaction between btKinematicCharacterController and dynamic rigid bodies needs to be explicity implemented by the user.
class PlayerController
{
protected:
	//btVector3 mHorizontalDirection;
	btRigidBody* mBody;
	btScalar mTimeSinceJumping, mTimeOnGround;

	btTransform mTransformBefore;

	btVector3 mWalkDirection, mLastVelocity, mIceVelocity;
	std::vector<std::pair<Platform*, Vector3>> mTouchingPlatforms;
	bool mLastOnGround;
	Player* mPlayer;
public:
	PlayerController(Player*, btRigidBody*);
	~PlayerController();
	

	///btActionInterface interface
	virtual void updateAction( btCollisionWorld* collisionWorld,btScalar deltaTime);
	
	void _clearTouchingPlatforms()
	{
		mTouchingPlatforms.clear();
	}
	void _addTouchingPlatform(void* platform, Vector3 vector)
	{
		mTouchingPlatforms.push_back(std::pair<Platform*, Vector3>(static_cast<Platform*>(platform), vector));
	}
	btRigidBody* getBody()
	{
		return mBody;
	}

	Vector3 getVelocity()
	{
		return TO_OGRE(mBody->getLinearVelocity());
	}
	btScalar timeSinceLastJump()
	{
		return mTimeSinceJumping;
	}

	bool isTouchingKillBox()
	{
		// TODO
		return false;
	}
	Monster* getTouchingMonster()
	{
		// TODO
		return 0;
	}
	std::vector<std::pair<Platform*, Vector3>>& getTouchingPlatform()
	{
		// TODO
		return mTouchingPlatforms;
	}
	// returns first non-floor normal, or floor normal, if there are no non-floor normals
	btVector3 getTouchingNormal()
	{
		btVector3 toReturn;
		toReturn.setZero();
		for (int i = 0; i < mTouchingPlatforms.size(); i++)
		{
			if (toReturn.isZero() || !normalIsStandable(TO_BULLET(mTouchingPlatforms[i].second)))
			{
				toReturn = TO_BULLET(mTouchingPlatforms[i].second);
			}
		}
		return toReturn;
	}
	bool isFalling()
	{
		return mBody->getLinearVelocity().y() < 0;
	}

	void onRespawn();
	void stopWalking();
	void increaseVerticalSpeed(btScalar speed);

	virtual void setHorizontalDirection(btVector3& walkDirection);
	virtual void setJumpSpeed(const btVector3& walkDirection);

	void preStep(btCollisionWorld* collisionWorld);
	void playerStep(btCollisionWorld* collisionWorld, btScalar dt);

	/// the int is a Player::JumpType
	void jump(btVector3& velocity, int jumpType);

	bool onGround() const;
};