#include "Physics.h"
#include "BulletCollision\CollisionDispatch\btGhostObject.h"
#include "balance.h"
#include "TextRenderer.h"

Physics::Physics(void)
	: mDebugDrawer(0),
	mWorld(0)
{
	worldInfoTemplate.m_gravity = btVector3(0, -WORLD_GRAVITY, 0);
	worldInfoTemplate.m_sparsesdf.Initialize();
}

Physics::~Physics(void)
{
	//delete mDebugDrawer;
	delete mWorld;
	mSingleton = 0;
}

void Physics::destroyWorld()
{
	if (mWorld)
	{
		delete mWorld;
		mWorld = 0;
	}
}

void Physics::recreateWorld()
{
	if (mWorld) destroyWorld();
	//mBroadphase = new bt32BitAxisSweep3(btVector3(-1000,-1000,-1000), btVector3(1000,1000,1000), 10000);
	mBroadphase = new btDbvtBroadphase();
	mDispatcher = new btCollisionDispatcher(new btDefaultCollisionConfiguration());
	mWorld = new btDiscreteDynamicsWorld(
		mDispatcher,
		mBroadphase,//new btDbvtBroadphase()
		new btSequentialImpulseConstraintSolver(),
		new btDefaultCollisionConfiguration()
	);
	mWorld->setGravity(btVector3(0, -WORLD_GRAVITY, 0));
	mBroadphase->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
	btContactSolverInfo& info = mWorld->getSolverInfo();
	info.m_splitImpulse = 1;
	info.m_splitImpulsePenetrationThreshold = -0.5;
	
	//if (mWorld) destroyWorld();
	////mBroadphase = new bt32BitAxisSweep3(btVector3(-1000,-1000,-1000), btVector3(1000,1000,1000), 10000);
	//mBroadphase = new btDbvtBroadphase();
	//auto conf = new btDefaultCollisionConfiguration();
	//mDispatcher = new btCollisionDispatcher(conf);

	//worldInfoTemplate.m_dispatcher = mDispatcher;
	//worldInfoTemplate.m_broadphase = mBroadphase;
	//
	//mWorld = new btSoftRigidDynamicsWorld(
	//	mDispatcher,
	//	mBroadphase,
	//	new btSequentialImpulseConstraintSolver(),
	//	conf
	//);

	//mWorld->setGravity(btVector3(0, -WORLD_GRAVITY, 0));
	//mBroadphase->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
	//btContactSolverInfo& info = mWorld->getSolverInfo();

//	gContactAddedCallback = &CustomMaterialCombinerCallback;
}

btDiscreteDynamicsWorld* Physics::getWorld()
{
	return mWorld;
}

#if ENABLE_WALL_GRAB
Platform* Physics::isTouchingForWallGrab(btVector3& start, btVector3& to)
{
	btScalar distanceHelper = PLAYER_BODY_X + PLAYER_WALL_GRAB_OFFSET_Z;
	distanceHelper *= 1;
	btCollisionWorld::ClosestRayResultCallback result((start), start + to * distanceHelper);
	mWorld->rayTest((start), start + to * distanceHelper, result);
	if (result.hasHit())
	{
		btScalar distance = result.m_hitPointWorld.distance(start);
		Platform* platformPtr = ((Platform*)result.m_collisionObject->platformPointer);
		if (platformPtr == 0) return 0;
		double yMaxPlatform = platformPtr->getBox()->getYmax();
		if (Math::Abs(start.y() - yMaxPlatform) <= PLAYER_WALL_GRAB_OFFSET_Y)
		{
			return platformPtr;
		}
		//result.m_collisionObject
	}

	return 0;
}
#endif

btScalar Physics::raycastCameraMap(Vector3 position, Vector3 normal, Real size)
{
	btCollisionWorld::AllHitsRayResultCallback result(TO_BULLET(position), TO_BULLET(position) + TO_BULLET(normal * size));
	mWorld->rayTest(TO_BULLET(position), TO_BULLET(position) + TO_BULLET(normal * size), result);
	if (result.hasHit())
	{
		for (int i = 0; i < result.m_collisionObjects.size(); i++)
		{
			if (result.m_collisionObjects[i]->customFlags == 4)
			{
				return (TO_OGRE(result.m_hitPointWorld[i]) - position).length();
			}
		}
	}

	return -1;
}

Platform* Physics::raycastPlatform(btVector3& start, btVector3& to)
{
	btCollisionWorld::ClosestRayResultCallback result((start), to);
	mWorld->rayTest((start), to, result);
	if (result.hasHit())
	{
		Platform* platformPtr = ((Platform*)result.m_collisionObject->platformPointer);
		if (platformPtr == 0) return 0;
		else return platformPtr;
		//result.m_collisionObject
	}

	return 0;
}

bool Physics::isFloorBelow(btVector3& start)
{
	btVector3 to = start - btVector3(0, 0.5, 0);
	btCollisionWorld::ClosestRayResultCallback result((start), to);
	mWorld->rayTest((start), to, result);
	if (result.hasHit())
	{
		
		if (normalIsStandable(result.m_hitNormalWorld))
		{
			return true;
		}
		//result.m_collisionObject
	}

	return false;
}

Physics* Physics::getSingleton()
{
	if (Physics::mSingleton == 0)
	{
		Physics::mSingleton = new Physics();
	}
	return Physics::mSingleton;
}

void Physics::initDebugDrawer(SceneManager* sceneManager)
{
	mDebugDrawer = new OgreDebugDrawer(sceneManager);//->getRootSceneNode(), world);
	mDebugDrawer->setDebugMode(0);
	mWorld->setDebugDrawer(mDebugDrawer);
}

Physics* Physics::mSingleton = 0;