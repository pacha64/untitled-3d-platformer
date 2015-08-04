#include "WorldBox.h"
#include "BtOgrePG.h"
#include "Playground.h"

WorldBox::WorldBox(StaticGeometry* geom, btTransform& transform, CustomMaterial material, bool isStatic)
	: mNode(0),
//	mSize(size),
	mMaterialId(material),
#if !FIX_INSERT_PLATFORM_EDITOR
	insertFix(false),
#endif
	mEntity(0),
	mIsEnabled(true)
{
	if(!ENABLE_STATIC_GEOMETRY)
	{
		isStatic = false;
	}

	if (mMaterialId < 0 || mMaterialId >= CM_SIZE) mMaterialId = CM_TUTORIAL_PATH_1;

	if (!ENABLE_PLATFORM_ROTATION)
	{
		transform.setRotation(TO_BULLET(Quaternion::IDENTITY));
	}
	btQuaternion rot = transform.getRotation();
	btVector3 pos = transform.getOrigin();
	
	if (isStatic)
	{
		geom->addEntity(
			mStaticBoxEntity[mMaterialId],
			TO_OGRE(pos),
			TO_OGRE(rot)//,
//			Vector3(mSize)
		);
	}
	else
	{
		mNode = mManager->getRootSceneNode()->createChildSceneNode(TO_OGRE(pos), TO_OGRE(rot));
//		mNode->scale(mSize);
		mEntity = mStaticBoxEntity[mMaterialId]->clone(requestNewEntityName());
		//mEntity->setMaterialName(mMaterials[mMaterialId]);
		mNode->attachObject(mEntity);
	}
	
	// circular
	if (mMaterialId == CM_TUTORIAL_PATH_5)
	{
		mShape = mAllShapes[CM_TUTORIAL_PATH_5];
	}
	else
	{
		mShape = new btBoxShape(TO_BULLET(typeSize(mMaterialId)) / 2);
	}
	mBody = new btCollisionObject();
	mBody->setCollisionShape(mShape);
	mBody->setRestitution(0);
	mBody->setWorldTransform(transform);
	mBody->customFlags = 3;

	Physics::getSingleton()->getWorld()->addCollisionObject(mBody,btBroadphaseProxy::StaticFilter,btBroadphaseProxy::AllFilter&(~(btBroadphaseProxy::StaticFilter | btBroadphaseProxy::KinematicFilter)));
	//btVector3 aabb1, aabb2;
	//btTransform trans;trans.setIdentity();
	//getBody()->getCollisionShape()->getAabb(trans, aabb1, aabb2);
	//mBoundingBox = aabb2 * 2;

	mWorld->updateAabbs();
	mWorld->getBroadphase()->calculateOverlappingPairs(mWorld->getDispatcher());

	//if (type == BoxType::BT_GOAL)
	//{
	//	ParticleSystem* particleSystem = mManager->createParticleSystem(requestNewParticleName() + "-death-particle", "Particles/GoalPlatformEffect");
	//	if (isStatic)
	//	{
	//		SceneNode* nodeHelper = mManager->getRootSceneNode()->createChildSceneNode();//->translate
	//		nodeHelper->setPosition(TO_OGRE(transform.getOrigin()) + Vector3(0, typeSize(type).y() / 2 + 5 /* margin */, 0));
	//		nodeHelper->scale(PLATFORM_SIZE_UNIT, PLATFORM_SIZE_UNIT, PLATFORM_SIZE_UNIT);
	//		nodeHelper->attachObject(particleSystem);
	//	}
	//	else
	//	{
	//		SceneNode* nodeHelper = mNode->createChildSceneNode();
	//		nodeHelper->translate(0, typeSize(type).y() / 2 / PLATFORM_SIZE_UNIT + 5 / PLATFORM_SIZE_UNIT, 0);
	//		nodeHelper->attachObject(particleSystem);
	//		_::aa = nodeHelper;
	//	}
	//}
}

WorldBox::~WorldBox(void)
{
	removePlatformEffects();
	if (mNode)
	{
		Playground::destroySceneNode(mNode);
	}
	mWorld->removeCollisionObject(mBody);
	delete mBody;
}

void WorldBox::removePlatformEffects()
{
	// no effects yet
}

void WorldBox::setPlatformEffects(CustomMaterial cm)
{
	// no effects yet
}

void WorldBox::setEnabled(bool enabled)
{
	assert(mNode && mEntity);
	
	if (mIsEnabled != enabled)
	{
		if (enabled)
		{
			Physics::getSingleton()->getWorld()->addCollisionObject(mBody);
		}
		else
		{
			Physics::getSingleton()->getWorld()->removeCollisionObject(mBody);
		}
	}

	mIsEnabled = enabled;
}

void WorldBox::initialize(SceneManager* manager, btDiscreteDynamicsWorld* world)
{
	/* mesh list:
	
	plane-normal.mesh
	plane-big.mesh
	tower-small.mesh
	plane-huge.mesh
	tower-small.mesh
	plane-150.mesh
	box-small.mesh
	
	*/ 
	mManager = manager;
	mWorld = world;
	
	mStaticBoxEntity[CM_TUTORIAL_PATH_1] = mManager->createEntity("plane-10-10-2.5.mesh");
	mStaticBoxEntity[CM_TUTORIAL_PATH_1]->setMaterialName("Platform/TutorialPath1");
	mCustomMaterialStrings[CM_TUTORIAL_PATH_1] = "Tutorial path 1";

	mStaticBoxEntity[CM_TUTORIAL_PATH_1_BREAKABLE] = mManager->createEntity("plane-10-10-2.5.mesh");
	mStaticBoxEntity[CM_TUTORIAL_PATH_1_BREAKABLE]->setMaterialName("Platform/TutorialPath1");
	mCustomMaterialStrings[CM_TUTORIAL_PATH_1_BREAKABLE] = "Tutorial path 1 breakable";


	mStaticBoxEntity[CM_TUTORIAL_PATH_2_KILLBOX] = mManager->createEntity("plane-10-10-2.5.mesh");
	mStaticBoxEntity[CM_TUTORIAL_PATH_2_KILLBOX]->setMaterialName("Platform/TutorialPath1");
	mCustomMaterialStrings[CM_TUTORIAL_PATH_2_KILLBOX] = "Tutorial path 2 kill box";

	mStaticBoxEntity[CM_TUTORIAL_PATH_3] = mManager->createEntity("plane-normal.mesh");
	mStaticBoxEntity[CM_TUTORIAL_PATH_3]->setMaterialName("Platform/TutorialPath1");
	mCustomMaterialStrings[CM_TUTORIAL_PATH_3] = "Tutorial path 3";

	mStaticBoxEntity[CM_TUTORIAL_PATH_4_TOP_KILLBOX] = mManager->createEntity("horizontal-plane-small.mesh");
	mStaticBoxEntity[CM_TUTORIAL_PATH_4_TOP_KILLBOX]->setMaterialName("Platform/TutorialPath1");
	mCustomMaterialStrings[CM_TUTORIAL_PATH_4_TOP_KILLBOX] = "Tutorial path 4 top kill box";

	mStaticBoxEntity[CM_TUTORIAL_PATH_4_KILLBOX] = mManager->createEntity("horizontal-plane-small.mesh");
	mStaticBoxEntity[CM_TUTORIAL_PATH_4_KILLBOX]->setMaterialName("Platform/TutorialPath1");
	mCustomMaterialStrings[CM_TUTORIAL_PATH_4_KILLBOX] = "Tutorial path 4 top kill box";
	
	mStaticBoxEntity[CM_TUTORIAL_PATH_5] = mManager->createEntity("circular-plane-normal.mesh");
	mStaticBoxEntity[CM_TUTORIAL_PATH_5]->setMaterialName("Platform/TutorialPath1");
	mCustomMaterialStrings[CM_TUTORIAL_PATH_5] = "Tutorial path 5";

	mStaticBoxEntity[CM_TUTORIAL_PATH_6] = mManager->createEntity("tower-small.mesh");
	mStaticBoxEntity[CM_TUTORIAL_PATH_6]->setMaterialName("Platform/TutorialPath1");
	mCustomMaterialStrings[CM_TUTORIAL_PATH_6] = "Tutorial path 6";

	mStaticBoxEntity[CM_OUT_MAP_1] = mManager->createEntity("plane-normal.mesh");
	mStaticBoxEntity[CM_OUT_MAP_1]->setMaterialName("Platform/TutorialPath1");
	mCustomMaterialStrings[CM_OUT_MAP_1] = "Map out 1";

	mStaticBoxEntity[CM_BOTTOM_LAIR_1] = mManager->createEntity("plane-normal.mesh");
	mStaticBoxEntity[CM_BOTTOM_LAIR_1]->setMaterialName("Platform/TutorialPath1");
	mCustomMaterialStrings[CM_BOTTOM_LAIR_1] = "Bottom lair 1";

	mStaticBoxEntity[CM_CHAPEL_1] = mManager->createEntity("plane-10-10-2.5.mesh");
	mStaticBoxEntity[CM_CHAPEL_1]->setMaterialName("Platform/TutorialPath1");
	mCustomMaterialStrings[CM_CHAPEL_1] = "Chapel 1";
		
	auto entity = mManager->createEntity("circular-plane-small.mesh");
	auto converter = BtOgre::StaticMeshToShapeConverter(entity);
	mAllShapes[CM_TUTORIAL_PATH_5] = converter.createTrimesh();
	mAllShapes[CM_TUTORIAL_PATH_5]->setLocalScaling(btVector3(2.0, 1.0, 2.0));
	
	mStaticBoxEntity[CM_HORIZONTAL_VERTICAL_SPECIAL_1] = mManager->createEntity("box-small.mesh");
	mStaticBoxEntity[CM_HORIZONTAL_VERTICAL_SPECIAL_1]->setMaterialName("Platform/TutorialPath1");
	mCustomMaterialStrings[CM_HORIZONTAL_VERTICAL_SPECIAL_1] = "Horizontal vertical special 1";
	// TODO rest of circulars shapes, and add to the array and the constructor the rest of the shapes
	
	static_cast<btBvhTriangleMeshShape*>(mAllShapes[CM_TUTORIAL_PATH_5])->buildOptimizedBvh();
	btTriangleInfoMap* triangleInfoMap = new btTriangleInfoMap();
	btGenerateInternalEdgeInfo(static_cast<btBvhTriangleMeshShape*>(mAllShapes[CM_TUTORIAL_PATH_5]),triangleInfoMap);

	//mStaticBoxEntity[CM_TUTORIAL_PLANE_2] = mManager->createEntity("plane-big.mesh");
	//mStaticBoxEntity[CM_TUTORIAL_PLANE_2]->setMaterialName("Platform/TutorialBigPlane");
	//mCustomMaterialStrings[CM_TUTORIAL_PLANE_2] = "Tutorial plane big";
	//
	//mStaticBoxEntity[CM_TUTORIAL_TOWER_1] = mManager->createEntity("tower-small.mesh");
	//mStaticBoxEntity[CM_TUTORIAL_TOWER_1]->setMaterialName("Platform/TutorialTowerSmall");
	//mCustomMaterialStrings[CM_TUTORIAL_TOWER_1] = "Tutorial tower small";
	//
	//mStaticBoxEntity[CM_TUTORIAL_PLANE_1_BREAKABLE] = mManager->createEntity("plane-normal.mesh");
	//mStaticBoxEntity[CM_TUTORIAL_PLANE_1_BREAKABLE]->setMaterialName("Platform/TutorialNormalPlane");
	//mCustomMaterialStrings[CM_TUTORIAL_PLANE_1_BREAKABLE] = "Tutorial plane normal breakable";
	//
	//mStaticBoxEntity[CM_TUTORIAL_TOWER_1_KILL_BOX] = mManager->createEntity("tower-small.mesh");
	//mStaticBoxEntity[CM_TUTORIAL_TOWER_1_KILL_BOX]->setMaterialName("Platform/TutorialTowerSmall");
	//mCustomMaterialStrings[CM_TUTORIAL_TOWER_1_KILL_BOX] = "Tutorial tower small kill box";

	//mStaticBoxEntity[CM_BLACK_CASTLE_BOX_SMALL] = mManager->createEntity("box-small.mesh");
	//mStaticBoxEntity[CM_BLACK_CASTLE_BOX_SMALL]->setMaterialName("Platform/BlackCastleBoxSmall");
	//mCustomMaterialStrings[CM_BLACK_CASTLE_BOX_SMALL] = "Black castle small box";
	//
	//mStaticBoxEntity[CM_BLACK_CASTLE_BOX_SMALL_KILL_BOX] = mManager->createEntity("box-small.mesh");
	//mStaticBoxEntity[CM_BLACK_CASTLE_BOX_SMALL_KILL_BOX]->setMaterialName("Platform/BlackCastleBoxSmall");
	//mCustomMaterialStrings[CM_BLACK_CASTLE_BOX_SMALL_KILL_BOX] = "Black castle small box kill box";

	//mStaticBoxEntity[CM_BLACK_CASTLE_BOX_SMALL_TOP_KILL_BOX] = mManager->createEntity("box-small.mesh");
	//mStaticBoxEntity[CM_BLACK_CASTLE_BOX_SMALL_TOP_KILL_BOX]->setMaterialName("Platform/BlackCastleBoxSmall");
	//mCustomMaterialStrings[CM_BLACK_CASTLE_BOX_SMALL_TOP_KILL_BOX] = "Black castle small box kill box top";
	//
	//mStaticBoxEntity[CM_BLACK_CASTLE_PLANE_HUGE] = mManager->createEntity("plane-huge.mesh");
	//mStaticBoxEntity[CM_BLACK_CASTLE_PLANE_HUGE]->setMaterialName("Platform/BlackCastleBoxSmall");
	//mCustomMaterialStrings[CM_BLACK_CASTLE_PLANE_HUGE] = "Black castle plane huge";

	//mStaticBoxEntity[CM_BLACK_CASTLE_PLANE_150] = mManager->createEntity("plane-150.mesh");
	//mStaticBoxEntity[CM_BLACK_CASTLE_PLANE_150]->setMaterialName("Platform/BlackCastleBoxSmall");
	//mCustomMaterialStrings[CM_BLACK_CASTLE_PLANE_150] = "Black castle plane 150";

	//mStaticBoxEntity[CM_BLACK_CASTLE_BOX_SMALL_BREAKABLE] = mManager->createEntity("box-small.mesh");
	//mStaticBoxEntity[CM_BLACK_CASTLE_BOX_SMALL_BREAKABLE]->setMaterialName("Platform/BlackCastleBoxSmall");
	//mCustomMaterialStrings[CM_BLACK_CASTLE_BOX_SMALL_BREAKABLE] = "Black castle small box breakable";
	
//	mStaticBoxEntity[CM_GRASS_AND_DIRT] = mManager->createEntity("plane-normal.mesh");
//	mStaticBoxEntity[CM_GRASS_AND_DIRT]->setMaterialName("Platform/GrassAndDirt");
	
	for (int i = 0; i < CM_SIZE; i++)
	{
		if (mStaticBoxEntity[i])
		{
			mStaticBoxEntity[i]->getMesh()->setAutoBuildEdgeLists(true);
			mStaticBoxEntity[i]->getMesh()->buildEdgeList();
			mStaticBoxEntity[i]->setCastShadows(false);
		}
	}
}

String WorldBox::materialId2string(WorldBox::CustomMaterial material)
{
	if (material >= 0 && material < CM_SIZE)
	{
		return mCustomMaterialStrings[material];
	}
	else
	{
		return "Error";
	}
}

Vector3 WorldBox::typeSize(CustomMaterial cm)
{
	String name = (mStaticBoxEntity[cm])->getMesh()->getName().c_str();
	if (name == "tower-small.mesh")
	{
		return Vector3(5, 15, 5);
	}
	else if (name == "plane-normal.mesh")
	{
		return Vector3(25, 5, 25);
	}
	else if (name == "plane-huge.mesh")
	{
		return Vector3(100, 5, 100);
	}
	else if (name == "plane-big.mesh")
	{
		return Vector3(50, 5, 50);
	}
	else if (name == "plane-100.mesh")
	{
		return Vector3(10, 5, 10);
	}
	else if (name == "plane-150.mesh")
	{
		return Vector3(15, 5, 15);
	}
	else if (name == "box-small.mesh")
	{
		return Vector3(5, 5, 5);
	}
	else if (name == "plane-10-10-2.5.mesh")
	{
		return Vector3(10, 2.5, 10);
	}
	else if (name == "horizontal-plane-small.mesh")
	{
		return Vector3(2.5, 5, 5);
	}
	else if (name == "circular-plane-small.mesh")
	{
		return Vector3(10, 2.5, 10);
	}
	
	return Vector3(5, 5, 5);
}

bool WorldBox::materialIsBreakable(WorldBox::CustomMaterial cm)
{
	if (cm == CM_TUTORIAL_PATH_1_BREAKABLE)
		//cm == CM_TUTORIAL_PLANE_1_BREAKABLE ||
		//cm == CM_BLACK_CASTLE_BOX_SMALL_BREAKABLE)
		return true;
	else
		return false;
}

bool WorldBox::materialIsKillbox(WorldBox::CustomMaterial cm)
{
	if (cm == CM_TUTORIAL_PATH_2_KILLBOX ||
		cm == CM_TUTORIAL_PATH_4_KILLBOX)
		//cm == CM_TUTORIAL_TOWER_1_KILL_BOX ||
		//cm == CM_BLACK_CASTLE_BOX_SMALL_TOP_KILL_BOX ||
		//cm == CM_BLACK_CASTLE_BOX_SMALL_KILL_BOX)
	{
		return true;
	}
	else
	{
		
		return false;
	}
}

bool WorldBox::materialIsTopKillbox(CustomMaterial cm)
{
	if (cm == CM_TUTORIAL_PATH_4_TOP_KILLBOX)
	{
		return true;
	}
	else
	{
		return false;
	}
}

String WorldBox::mCustomMaterialStrings[CustomMaterial::CM_SIZE];
Entity* WorldBox::mStaticBoxEntity[CustomMaterial::CM_SIZE];
btCollisionShape* WorldBox::mAllShapes[CustomMaterial::CM_SIZE];
SceneManager* WorldBox::mManager = 0;
btDiscreteDynamicsWorld* WorldBox::mWorld = 0;
//String WorldBox::mMaterials[CustomMaterial::CM_SIZE];