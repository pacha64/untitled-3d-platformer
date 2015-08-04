/*
Bullet Continuous Collision Detection and Physics Library
Copyright (c) 2003-2008 Erwin Coumans  http://bulletphysics.com

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose, 
including commercial applications, and to alter it and redistribute it freely, 
subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/
#include "Globals.h"

#include "LinearMath/btIDebugDraw.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#include "BulletCollision/CollisionShapes/btMultiSphereShape.h"
#include "BulletCollision/BroadphaseCollision/btOverlappingPairCache.h"
#include "BulletCollision/BroadphaseCollision/btCollisionAlgorithm.h"
#include "BulletCollision/CollisionDispatch/btCollisionWorld.h"
#include "LinearMath/btDefaultMotionState.h"

#include "PlayerController.h"
#include "Physics.h"
#include "Player.h"

void tickCallback(btDynamicsWorld *world, btScalar timeStep)
{
	//static_cast<PlayerController*>(world->getWorldUserInfo())->_clearNormals();//(btVector3(0,0,0));
	
	static_cast<PlayerController*>(world->getWorldUserInfo())->_clearTouchingPlatforms();

	int numManifolds = world->getDispatcher()->getNumManifolds();
	for (int i=0;i<numManifolds;i++)
	{
		btPersistentManifold* contactManifold =  world->getDispatcher()->getManifoldByIndexInternal(i);
		const btCollisionObject* obA = (contactManifold->getBody0());
 		const btCollisionObject* obB = (contactManifold->getBody1());

		auto body = static_cast<PlayerController*>(world->getWorldUserInfo())->getBody();
		if (obA == body || obB == body)
		{
			int numContacts = contactManifold->getNumContacts();
			if (numContacts == 0)
			{
				int a;
				a = 2;
			}
			for (int j=0;j<numContacts;j++)
			{
				auto normal = TO_OGRE(contactManifold->getContactPoint(j).m_normalWorldOnB * (obB == body ? -1 : 1));
				if (obA->platformPointer)
				{
					static_cast<PlayerController*>(world->getWorldUserInfo())->_addTouchingPlatform(obA->platformPointer, normal);
				}
				if (obB->platformPointer)
				{
					static_cast<PlayerController*>(world->getWorldUserInfo())->_addTouchingPlatform(obB->platformPointer, normal);
				}
				//if (pt.getDistance()<0.f)
				//{
				//	btVector3 normalOnB = pt.m_normalWorldOnB;
				//	if (obB == body)
				//	{
				//		normalOnB *= -1;
				//	}
				//	
				//	static_cast<PlayerController*>(world->getWorldUserInfo())->_addNormal(normalOnB);
				//}
			}
		}
	}
}

///@todo Interact with dynamic objects,
///Ride kinematicly animated platforms properly
///More realistic (or maybe just a config option) falling
/// -> Should integrate falling velocity manually and use that in stepDown()
///Support jumping
///Support ducking
class btKinematicClosestNotMeRayResultCallback : public btCollisionWorld::ClosestRayResultCallback
{
public:
	btKinematicClosestNotMeRayResultCallback (btCollisionObject* me) : btCollisionWorld::ClosestRayResultCallback(btVector3(0.0, 0.0, 0.0), btVector3(0.0, 0.0, 0.0))
	{
		m_me = me;
	}

	virtual btScalar addSingleResult(btCollisionWorld::LocalRayResult& rayResult,bool normalInWorldSpace)
	{
		if (rayResult.m_collisionObject == m_me)
			return 1.0;

		return ClosestRayResultCallback::addSingleResult (rayResult, normalInWorldSpace);
	}
protected:
	btCollisionObject* m_me;
};

class btKinematicClosestNotMeConvexResultCallback : public btCollisionWorld::ClosestConvexResultCallback
{
public:
	btKinematicClosestNotMeConvexResultCallback (btCollisionObject* me, const btVector3& up, btScalar minSlopeDot)
	: btCollisionWorld::ClosestConvexResultCallback(btVector3(0.0, 0.0, 0.0), btVector3(0.0, 0.0, 0.0))
	, m_me(me)
	, m_up(up)
	, m_minSlopeDot(minSlopeDot)
	{
	}

	virtual btScalar addSingleResult(btCollisionWorld::LocalConvexResult& convexResult,bool normalInWorldSpace)
	{
		if (convexResult.m_hitCollisionObject == m_me)
			return btScalar(1.0);

		btVector3 hitNormalWorld;
		if (normalInWorldSpace)
		{
			hitNormalWorld = convexResult.m_hitNormalLocal;
		} else
		{
			///need to transform normal into worldspace
			hitNormalWorld = convexResult.m_hitCollisionObject->getWorldTransform().getBasis()*convexResult.m_hitNormalLocal;
		}

		btScalar dotUp = m_up.dot(hitNormalWorld);
		if (dotUp < m_minSlopeDot) {
			return btScalar(1.0);
		}

		return ClosestConvexResultCallback::addSingleResult (convexResult, normalInWorldSpace);
	}
protected:
	btCollisionObject* m_me;
	const btVector3 m_up;
	btScalar m_minSlopeDot;
};

PlayerController::PlayerController (Player* player, btRigidBody* body)
{
	mTransformBefore = body->getWorldTransform();
	mPlayer = player;
	mTouchingPlatforms.clear();
//	mIsOnIce = false;
	mLastOnGround = false;
	mWalkDirection.setZero();
	mIceVelocity.setZero();
	mBody = body;
	mLastVelocity = mBody->getLinearVelocity();
	mTimeSinceJumping = 0.0;
	mTimeOnGround = 0.0;
	Physics::getSingleton()->getWorld()->setInternalTickCallback(tickCallback, static_cast<void *>(this));
}

PlayerController::~PlayerController ()
{
}

void PlayerController::updateAction( btCollisionWorld* collisionWorld,btScalar deltaTime)
{
	mTimeSinceJumping += deltaTime;
	bool onGroundHelper = onGround();
	
	if (onGroundHelper)
	{
		if (mTimeSinceJumping > PLAYER_STAND_VELOCITY_Y_0_TIME && mTimeOnGround <= PLAYER_STAND_VELOCITY_Y_0_TIME)
		{
			mBody->setLinearVelocity(btVector3(mBody->getLinearVelocity().x(), 0, mBody->getLinearVelocity().z()));
		}
		mTimeOnGround += deltaTime;
	} 
	else
	{
		mTimeOnGround = 0;
	}
	
	bool isOnIce = mPlayer->getStandingPlatform() && mPlayer->getStandingPlatform()->isIce();
	if (isOnIce && ((!mLastOnGround && onGroundHelper) || !onGroundHelper))
	{
		mIceVelocity = mLastVelocity;
		mIceVelocity.setY(0);
	}
	
	// bouncing fix
	if (mLastVelocity.y() < 0.0 && mBody->getLinearVelocity().y() > 0.0 && mTimeSinceJumping > 0.05)
	{
		auto velocity = mBody->getLinearVelocity();
		velocity.setY(0);
		mBody->setLinearVelocity(velocity);
	}


	//if (mTouchingNormals.size() && mWalkDirection.length())
	//{
	//	for (int i = 0; i < mTouchingNormals.size(); i++)
	//	{
	//		Vector3 normal = TO_OGRE((mTouchingNormals[i]));
	//		if (!(normalIsStandable(mTouchingNormals[i])) && normal.angleBetween(TO_OGRE(mWalkDirection)).valueDegrees() >= 90)
	//		{
	//			mWalkDirection.setZero();
	//		}
	//	}
	//}

	// TODO fix this
	//if (onGround() && mWalkDirection.length())
	//{
	//	btTransform transf = mBody->getWorldTransform();
	//	transf.setOrigin(transf.getOrigin() + mWalkDirection * deltaTime);
	//	mBody->setWorldTransform(transf);
	//}

	if (onGroundHelper && mTimeSinceJumping >= 0.2)
	{
		auto velocity = mBody->getLinearVelocity();
		if (isOnIce )
		{
			mIceVelocity += mWalkDirection * deltaTime;
			if (mIceVelocity.length() > ICE_MAX_SPEED)
			{
				mIceVelocity.normalize();
				mIceVelocity *= ICE_MAX_SPEED;
			}

			if (mIceVelocity.length())
			{
				auto velocityHelper = mIceVelocity;
				velocityHelper.normalize();
				auto toReduce = mIceVelocity.length() - ICE_SLIDE_REDUCE_SECOND * deltaTime;
				velocityHelper *= toReduce > 0.0 ? toReduce : 0.0;
				mBody->setLinearVelocity(velocityHelper);
				mIceVelocity = velocityHelper;
			}
		}
		else
		{
			//if (velocity.x() != 0.0 || velocity.z() != 0.0)
			//{
			//	velocity.setX(0);
			//	velocity.setZ(0);
			//	mBody->setLinearVelocity(velocity);
			//}
		}
	}
	
	/*if (!mHorizontalDirection.isZero())
	{
		auto transf = mBody->getWorldTransform();
		transf.setOrigin(transf.getOrigin() + mHorizontalDirection * deltaTime);
		mBody->setWorldTransform(transf);
	}*/

	if ((mBody->getLinearVelocity().y()) < -PLAYER_MAX_FALL_SPEED)
	{
		// TODO fix this
		auto veloctiy = mBody->getLinearVelocity();
		veloctiy.setY(-PLAYER_MAX_FALL_SPEED);
		mBody->setLinearVelocity(veloctiy);
	}

	mLastVelocity = mBody->getLinearVelocity();

	for (int i = 0; i < mTouchingPlatforms.size(); i++)
	{
		if (
			mTouchingPlatforms[i].first->isKillBox() ||
			(mTouchingPlatforms[i].first->isTopKillBox() &&
			normalIsStandable(TO_BULLET(mTouchingPlatforms[i].second))))
		{
			mPlayer->shouldKillPlayer = true;
		}
	}
	
	if (onGroundHelper)
	{
		_::stats.totalActions.distanceAir += (mBody->getWorldTransform().getOrigin() - mTransformBefore.getOrigin()).length();
	}
	else
	{
		_::stats.totalActions.distanceWalking += (mBody->getWorldTransform().getOrigin() - mTransformBefore.getOrigin()).length();
	}

	mTransformBefore = mBody->getWorldTransform();
	mLastOnGround = onGroundHelper;
	DEBUG_PRINT_VECTOR_3(13, TO_OGRE(mBody->getWorldTransform().getOrigin()));
}

void PlayerController::onRespawn()
{
	mTransformBefore = mBody->getWorldTransform();
	mTouchingPlatforms.clear();
}

void PlayerController::stopWalking()
{
	// TODO might be bugged
	mWalkDirection.setZero();
	mBody->setLinearVelocity(btVector3(0, mBody->getLinearVelocity().y(), 0));
}

void PlayerController::increaseVerticalSpeed(btScalar speed)
{
	auto velocity = mBody->getLinearVelocity();
	velocity.setY(velocity.y() + speed);
	mBody->setLinearVelocity(velocity);
}

void PlayerController::setHorizontalDirection(btVector3& walkDirection)
{
//	const_cast<btVector3&>(walkDirection).setY(0);
	mWalkDirection = walkDirection;
	if (mPlayer->getStandingPlatform() && mPlayer->getStandingPlatform()->isIce())
	{
		walkDirection = walkDirection.normalize() * PLAYER_ICE_MOVEMENT_SPEED;
	}

	const_cast<btVector3&>(walkDirection).setY(mBody->getLinearVelocity().y());
	mBody->setLinearVelocity(walkDirection);
}

void PlayerController::setJumpSpeed(const btVector3& walkDirection)
{
	mBody->setLinearVelocity(walkDirection);
}

void PlayerController::jump (btVector3& velocity, int jumpType)
{
	switch(jumpType)
	{
	case Player::JumpType::JT_LONG:
		_::stats.totalActions.longJumps++;
		break;
	case Player::JumpType::JT_NORMAL_MOVING:
	case Player::JumpType::JT_NORMAL_STATIC:
		_::stats.totalActions.normalJumps++;
		break;
	case Player::JumpType::JT_WALL_JUMP_LONG:
		_::stats.totalActions.longWallJumps++;
		break;
	case Player::JumpType::JT_WALL_JUMP_NORMAL:
		_::stats.totalActions.normalWallJumps++;
		break;
	}

	if (mPlayer-> getStandingPlatform() && mPlayer->getStandingPlatform()->isIce())
	{
		if (
			jumpType != Player::JumpType::JT_WALL_JUMP_LONG &&
			jumpType != Player::JumpType::JT_WALL_JUMP_NORMAL)
		{
			velocity += mIceVelocity;
			velocity = TO_BULLET(Player::trimJumpVelocity(TO_OGRE(velocity), jumpType));
		}
	}

//	mBody->applyCentralForce(velocity);
	mBody->setLinearVelocity(velocity);
	mTimeSinceJumping = 0.0;

}

bool PlayerController::onGround () const
{
	for (int i = 0; i < mTouchingPlatforms.size(); i++)
	{
		if (normalIsStandable(TO_BULLET(mTouchingPlatforms[i].second)))
		{
			return true;
		}
	}

	return false;

	//auto verticalVelocity = mBody->getLinearVelocity().y();
	//return
	//	(verticalVelocity == 0.0) ||
	//	(verticalVelocity <= 0.0 && Physics::getSingleton()->isFloorBelow(
	//		mBody->getWorldTransform().getOrigin() -
	//		btVector3(0, PLAYER_BODY_Y - 0.3 /* margin */, 0)
	//	));
}