#pragma once
#include "stdafx.h"
#include "Platform.h"
#include "DebugDrawer.h"

extern ContactAddedCallback gContactAddedCallback;

inline btScalar	calculateCombinedFriction(float friction0,float friction1)
{
	return 0.f;
	btScalar friction = friction0 * friction1;

	const btScalar MAX_FRICTION  = 10.f;
	if (friction < -MAX_FRICTION)
		friction = -MAX_FRICTION;
	if (friction > MAX_FRICTION)
		friction = MAX_FRICTION;
	return friction;

}

inline btScalar	calculateCombinedRestitution(float restitution0,float restitution1)
{
	return restitution0 * restitution1;
}

static void customCallbackObj(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj, int partId, int index, bool invert)
{
    (void) partId;
    (void) index;
	const btCollisionShape *shape = colObj->m_shape;

    if (shape->getShapeType() != TRIANGLE_SHAPE_PROXYTYPE) return;
    const btTriangleShape *tshape =
            static_cast<const btTriangleShape*>(colObj->getCollisionShape());


    btTransform orient = colObj->getWorldTransform();
    orient.setOrigin( btVector3(0.0f,0.0f,0.0f ) );

    btVector3 v1 = tshape->m_vertices1[0];
    btVector3 v2 = tshape->m_vertices1[1];
    btVector3 v3 = tshape->m_vertices1[2];

    btVector3 normal = (v2-v1).cross(v3-v1);

    normal = orient * normal;
    normal.normalize();

    btScalar dot = normal.dot(cp.m_normalWorldOnB);
    btScalar magnitude = cp.m_normalWorldOnB.length();
    normal *= dot > 0 ? magnitude : -magnitude;

    cp.m_normalWorldOnB = normal * (invert ? -1 : 1);	

}

static bool CustomMaterialCombinerCallback(btManifoldPoint& cp,	const btCollisionObjectWrapper* colObj0Wrap,int partId0,int index0,const btCollisionObjectWrapper* colObj1Wrap,int partId1,int index1)
{
	
	//customCallbackObj(cp, colObj0Wrap, partId0, index0, false);
    //customCallbackObj(cp, 

	btAdjustInternalEdgeContacts(cp,colObj1Wrap,colObj0Wrap, partId1,index1, BT_TRIANGLE_CONCAVE_DOUBLE_SIDED);
	//if (enable)
	{
		//btAdjustInternalEdgeContacts(cp,colObj1,colObj0, partId1,index1, BT_TRIANGLE_CONVEX_BACKFACE_MODE);
		//btAdjustInternalEdgeContacts(cp,colObj1,colObj0, partId1,index1, BT_TRIANGLE_CONVEX_DOUBLE_SIDED+BT_TRIANGLE_CONCAVE_DOUBLE_SIDED);
	}

	float friction0 = colObj0Wrap->getCollisionObject()->getFriction();
	float friction1 = colObj1Wrap->getCollisionObject()->getFriction();
	float restitution0 = colObj0Wrap->getCollisionObject()->getRestitution();
	float restitution1 = colObj1Wrap->getCollisionObject()->getRestitution();

	if (colObj0Wrap->getCollisionObject()->getCollisionFlags() & btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK)
	{
		friction0 = 1.0;//partId0,index0
		restitution0 = 0.f;
	}
	if (colObj1Wrap->getCollisionObject()->getCollisionFlags() & btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK)
	{
		if (index1&1)
		{
			friction1 = 1.0f;//partId1,index1
		} else
		{
			friction1 = 0.f;
		}
		restitution1 = 0.f;
	}

	cp.m_combinedFriction = calculateCombinedFriction(friction0,friction1);
	cp.m_combinedRestitution = calculateCombinedRestitution(restitution0,restitution1);
	//cp.m_combinedFriction = calculateCombinedFriction(0.0,0.0);
	//cp.m_combinedRestitution = calculateCombinedRestitution(0.0,0.0);

	//this return value is currently ignored, but to be on the safe side: return false if you don't calculate friction
	return true;
}

class Physics
{
public:
	static Physics* getSingleton();
	~Physics(void);
	void initDebugDrawer(SceneManager*);
	OgreDebugDrawer* getDebugDrawer()
	{
		return mDebugDrawer;
	}
	btDiscreteDynamicsWorld* getWorld();
	btBroadphaseInterface* getBroadphase()
	{
		return mBroadphase;
	}
	btCollisionDispatcher* getDispatcher()
	{
		return mDispatcher;
	}
#if ENABLE_WALL_GRAB
	Platform* isTouchingForWallGrab(btVector3& start, btVector3& to);
#endif
	btScalar raycastCameraMap(Vector3 position, Vector3 normal, Real size);
	Platform* raycastPlatform(btVector3& start, btVector3& to);
	bool isFloorBelow(btVector3& start);
	void destroyWorld();
	void recreateWorld();
	btSoftBodyWorldInfo worldInfoTemplate;
private:
	OgreDebugDrawer* mDebugDrawer;
	btBroadphaseInterface* mBroadphase;
	btCollisionDispatcher* mDispatcher;// = new btCollisionDispatcher
	Physics(void);
	btDiscreteDynamicsWorld* mWorld;
	btDefaultCollisionConfiguration* mCollisionConfiguration;
	static Physics* mSingleton;
};
