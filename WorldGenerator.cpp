#include "WorldGenerator.h"
#include "BtOgrePG.h"
#include "WorldBox.h"
#include "TextRenderer.h"
#include "Player.h"
#include "Playground.h"
#include "MonsterCometLauncher.h"
#include "MonsterState.h"
#include "HolyMarble.h"
#include "GameState.h"

WorldGenerator::WorldGenerator(SceneManager* sceneManager)
	: mSceneManager(sceneManager),
	mMovablePlatforms(),
	blockDimension(btVector3(700, 700, 700)),
	//mStartVector(btVector3(0, 0, 0)),
	mSeed("aaaaaa"),
	mWorldTopRight(0, 0, 0),
	mWorldBottomLeft(0, 0, 0),
	mMapWorld(0),
	mSkyBox(0),
	mStaticGeometry(0),
	mFirstRespawn(0),
	mTimeSinceStart(0),
	mCurrentPlayerActivator(0),
	mMapMonster(0),
	mCurrentFormat(0),
	mGameState(0)
	//mStaticGeometryCircle(0),
//	mMonsterState(new MonsterState())
{
	//mMeshMaterialName[MZ_MAPOUT].first = "map-out.mesh";
	//mMeshMaterialName[MZ_MAPOUT].second = "MapOut";
	//mMeshMaterialName[MZ_BLACK_CASTLE].first = "black-castle.mesh";
	//mMeshMaterialName[MZ_BLACK_CASTLE].second = "BlackCastle";
	//mMeshMaterialName[MZ_CHURCH].first = "chapel.mesh";
	//mMeshMaterialName[MZ_CHURCH].second = "Chapel";
	mMeshMaterialName[MZ_ICE_CORE].first = "Cube.mesh";
	mMeshMaterialName[MZ_ICE_CORE].second = "IceCore";
	mMeshMaterialName[MZ_CAVE].first = "cave.mesh";
	mMeshMaterialName[MZ_CAVE].second = "Cave";
	mMeshMaterialName[MZ_BOTTOM_LAIR].first = "bottom-lair.mesh";
	mMeshMaterialName[MZ_BOTTOM_LAIR].second = "BottomLair";
	//mMeshMaterialName[MZ_TUTORIAL_PATH].first = "tutorial-path.mesh";
	//mMeshMaterialName[MZ_TUTORIAL_PATH].second = "TutorialPath";

//	mMapZonesNames[MZ_MAPOUT] = "Outside";
//	mMapZonesNames[MZ_BLACK_CASTLE] = "Black Castle";
//	mMapZonesNames[MZ_CHURCH] = "Underground Church";
	mMapZonesNames[MZ_ICE_CORE] = "Ice core";
	mMapZonesNames[MZ_CAVE] = "Triangle witch's lair";
	mMapZonesNames[MZ_BOTTOM_LAIR] = "Bottom cave";
//	mMapZonesNames[MZ_TUTORIAL_PATH] = "Tutorial path";
	
	memset(mMapNodes, 0, sizeof(SceneNode*) * MapZones::MZ_SIZE);
	memset(mMapEntities, 0, sizeof(Entity*) * MapZones::MZ_SIZE);
	memset(mMapCollisionObject, 0, sizeof(btCollisionObject*) * MapZones::MZ_SIZE);
	memset(mMonsters, 0, sizeof(Monster*) * Monster::MT_SIZE);
	
	//mMapZonesAabb[MZ_MAPOUT].first = Vector3(-450, -450, -450);
	//mMapZonesAabb[MZ_MAPOUT].second = Vector3(450, 450, 450);
	//mMapZonesAabb[MZ_BLACK_CASTLE].first = Vector3(-180, 449, -180);
	//mMapZonesAabb[MZ_BLACK_CASTLE].second = Vector3(180, 930, 180);
	//mMapZonesAabb[MZ_CHURCH].first = Vector3(255, 390, -285); // TODO fix this
	//mMapZonesAabb[MZ_CHURCH].second = Vector3(45, 300, 435); // TODO fix this
	mMapZonesAabb[MZ_ICE_CORE].first = Vector3(-420, -100, -330); // TODO fix this
	mMapZonesAabb[MZ_ICE_CORE].second = Vector3(390, 60, 210); // TODO fix this
	mMapZonesAabb[MZ_CAVE].first = Vector3(-90, -195, -90);
	mMapZonesAabb[MZ_CAVE].second = Vector3(90, -105, 90);
	mMapZonesAabb[MZ_BOTTOM_LAIR].first = Vector3(-435, -435, 180);
	mMapZonesAabb[MZ_BOTTOM_LAIR].second = Vector3(-375, -375, 450);
	//mMapZonesAabb[MZ_TUTORIAL_PATH].first = Vector3(-421, 254, -286);
	//mMapZonesAabb[MZ_TUTORIAL_PATH].second = Vector3(451, 286, -254);

/*
	for (int i = MZ_MAPOUT; i < MZ_SIZE; i++)
	{
		mMapZonesAabb[(MapZones)i].first.z *= -1;
		mMapZonesAabb[(MapZones)i].second.z *= -1;
	}
*/
//	counter = 0;
	srand(getSeedNumeric());
	mCurrentInstance = (this);
	WorldBox::initialize(sceneManager, Physics::getSingleton()->getWorld());
	/*FILE* json = fopen("E:\\test.json", "r");
	fstream file("E:\\test.json");
	if(file.is_open())
	{
		int aa;
		aa = 43;
	}
	if (json == 0)
	{
		// No such file or directory
		String aa = strerror(errno);
	}
	if (json != 0)
	{*/
}

WorldGenerator::~WorldGenerator(void)
{

	destroy();
	/*for (int i = 0; i < mSouls.size(); i++)
	{
		delete mSouls[i];
	}*/

	/*for (int i = 0; i < mRespawns.size(); i++)
	{
		delete mRespawns[i];
	}*/

	//for (int i = 0; i < mPlatforms.size(); i++)
	//{
	//	delete mPlatforms[i];
	//}
}

bool WorldGenerator::validJson(json_t* root)
{
	return true;
	// fix
	try
	{
		// information
		json_t* jsonHelper, *jsonHelper2, *jsonHelper3, *jsonHelper4, *jsonHelper5, *jsonHelper6;

		jsonHelper = json_object_get(root, "meta");
		if (jsonHelper == 0)
		{
			return 0;
		}
		jsonHelper2 = json_object_get(jsonHelper, "name");
		if (jsonHelper2 == 0 || jsonHelper2->type != json_type::JSON_STRING)
		{
			throw 0;
		}
		jsonHelper2 = json_object_get(jsonHelper, "description");
		if (jsonHelper2 == 0 || jsonHelper2->type != json_type::JSON_STRING)
		{
			throw 0;
		}
		jsonHelper2 = json_object_get(jsonHelper, "creator");
		if (jsonHelper2 == 0 || jsonHelper2->type != json_type::JSON_STRING)
		{
			throw 0;
		}
		jsonHelper2 = json_object_get(jsonHelper, "group");
		if (jsonHelper2 == 0 || jsonHelper2->type != json_type::JSON_STRING)
		{
			throw 0;
		}

		// map-information
		jsonHelper = json_object_get(root, "information");
		if (jsonHelper == 0)
		{
			throw 0;
		}
		jsonHelper2 = json_object_get(jsonHelper, "respawn");
		if (jsonHelper2 == 0)
		{
			throw 0;
		}
		//jsonHelper3 = json_object_get(jsonHelper2, "respawn");
		if (jsonHelper2->type != json_type::JSON_ARRAY && json_array_size(jsonHelper2) != 3)
		{
			throw 0;
		}
		/*jsonHelper2 = json_object_get(jsonHelper, "origin");
		if (jsonHelper2 == 0)
		{
			throw 0;
		}
		if (jsonHelper2->type != json_type::JSON_ARRAY && json_array_size(jsonHelper2) != 3)
		{
			throw 0;
		}*/
		jsonHelper2 = json_object_get(jsonHelper, "size");
		if (jsonHelper2 == 0)
		{
			throw 0;
		}
		if (json_array_size(jsonHelper2) != 3)
		{
			throw 0;
		}
		
		jsonHelper = json_object_get(root, "map");
		if (jsonHelper == 0)
		{
			throw 0;
		}
		if (jsonHelper->type != json_type::JSON_ARRAY)
		{
			throw 0;
		}
		else
		{
			for (int i = 0; i < json_array_size(jsonHelper); i++)
			{
				jsonHelper2 = json_array_get(jsonHelper, i);
				if (jsonHelper2 == 0)
				{
					throw 0;
				}
				jsonHelper3 = json_object_get(jsonHelper2, "node");
				if (jsonHelper3 == 0)
				{
					throw 0;
				}
				// json_t* jsonHelper4 = json_string(jsonHelper3
				String type = json_string_value(jsonHelper3);
				if (type == "block")
				{
					jsonHelper3 = json_object_get(jsonHelper2, "type");
					if (jsonHelper3->type != json_type::JSON_STRING)
					{
						throw 0;
					}
					jsonHelper3 = json_object_get(jsonHelper2, "transform");
					jsonHelper4 = json_object_get(jsonHelper3, "origin");
					if (jsonHelper4->type != json_type::JSON_ARRAY && json_array_size(jsonHelper4) != 3)
					{
						throw 0;
					}
					jsonHelper4 = json_object_get(jsonHelper3, "size");
					if (jsonHelper4->type != json_type::JSON_ARRAY && json_array_size(jsonHelper4) != 3)
					{
						throw 0;
					}
				}
				else if (type == "path")
				{
					jsonHelper3 = json_object_get(jsonHelper2, "origin");
					if (jsonHelper3->type != json_type::JSON_ARRAY && json_array_size(jsonHelper3) != 3)
					{
						throw 0;
					}
					jsonHelper3 = json_object_get(jsonHelper2, "default-size");
					if (jsonHelper3->type != json_type::JSON_ARRAY && json_array_size(jsonHelper3) != 3)
					{
						throw 0;
					}
					jsonHelper3 = json_object_get(jsonHelper2, "type");
					if (jsonHelper3->type != json_type::JSON_STRING)// && json_array_size(jsonHelper3) != 3)
					{
						throw 0;
					}
					jsonHelper3 = json_object_get(jsonHelper2, "height");
					if (jsonHelper3->type != json_type::JSON_INTEGER)// && json_array_size(jsonHelper3) != 3)
					{
						throw 0;
					}
					jsonHelper3 = json_object_get(jsonHelper2, "margin");
					if (jsonHelper3->type != json_type::JSON_INTEGER)// && json_array_size(jsonHelper3) != 3)
					{
						throw 0;
					}
					jsonHelper3 = json_object_get(jsonHelper2, "steps");
					if (jsonHelper3->type != json_type::JSON_ARRAY)
					{
						throw 0;
					}
					for (int i = 0; i < json_array_size(jsonHelper3); i++)
					{
						jsonHelper4 = json_array_get(jsonHelper3, i);
						jsonHelper5 = json_object_get(jsonHelper4, "degree");
						if (jsonHelper5->type != json_type::JSON_INTEGER)
						{
							throw 0;
						}
						jsonHelper5 = json_object_get(jsonHelper4, "size");
						if (jsonHelper5 != 0 && jsonHelper5->type != json_type::JSON_ARRAY && json_array_size(jsonHelper5) != 3)
						{
							throw 0;
						}
						jsonHelper5 = json_object_get(jsonHelper4, "height");
						if (jsonHelper5 != 0 && jsonHelper5->type != json_type::JSON_INTEGER)// && json_array_size(jsonHelper5) != 3)
						{
							throw 0;
						}
						jsonHelper5 = json_object_get(jsonHelper4, "margin");
						if (jsonHelper5 != 0 && jsonHelper5->type != json_type::JSON_INTEGER)// && json_array_size(jsonHelper5) != 3)
						{
							throw 0;
						}
						jsonHelper5 = json_object_get(jsonHelper4, "type");
						if (jsonHelper5 != 0 && jsonHelper5->type != json_type::JSON_STRING)// && json_array_size(jsonHelper5) != 3)
						{
							throw 0;
						}
					}
				}
			}
		}
	}
	catch(...)
	{
		return false;
	}

	return true;
}

bool WorldGenerator::isOnTopOfGoal(Player* player)
{
	for (int i = 0; i < mTouchableGoals.size(); i++)
	{
		if (mTouchableGoals[i]->isTouchingObject(player->getAabbMax(), player->getAabbMin()))
		{
			return true;
		}
	}
	return false;
}

void WorldGenerator::destroyWorldStructure(WorldStructure* worldStructure)
{
	for (int i = 0; i < worldStructure->map.blocks.size(); i++)
	{
		delete worldStructure->map.blocks[i];
	}
	delete worldStructure;
}

uint WorldGenerator::getSeedNumeric()
{
	MD5 md5(const_cast<unsigned char*>((unsigned char*)mSeed.c_str()), mSeed.size());
	uint toReturn;
	memcpy(&toReturn, md5.raw_digest(), sizeof(int));
	return toReturn;
}

void WorldGenerator::setMapStructure(MapFormat* world)
{
	mCurrentFormat = world;
}

void WorldGenerator::recreateSkyBox()
{
	if (mSkyBox != 0)
	{
		Playground::destroySceneNode(mSkyBox);
	}
	
	mSkyBox = mSceneManager->getRootSceneNode()->createChildSceneNode();
	Entity* entity = mSceneManager->createEntity("sky-box-sphere.mesh");
	entity->setRenderQueueGroup(RENDER_QUEUE_SKIES_EARLY);
	entity->setCastShadows(false);
	AxisAlignedBox box = entity->getBoundingBox();
	entity->setMaterialName("Skybox/Default");
	//yentity->setMaterialName("Skybox/General");
	
	/*Vector3 origin;
	Vector3 scale;
	for (int i = 0; i < 3; i++)
	{
		origin[i] = (mWorldTopRight[i] + mWorldBottomLeft[i]) / 2;
		scale[i] = (mWorldTopRight[i] - mWorldBottomLeft[i]);
	}*/

	mSkyBox->attachObject(entity);
	//mSkyBox->setScale(scale);
	//mSkyBox->setPosition(origin);
}

void WorldGenerator::destroy()
{
	mSceneManager->destroyStaticGeometry(mStaticGeometry);
	//mSceneManager->destroyStaticGeometry(mStaticGeometryCircle);
	mCurrentInstance = 0;
	for (int i = 0; i < mPlatforms.size(); i++)
	{
		delete mPlatforms[i];
	}
}

void WorldGenerator::generate()
{
	//Light* directionalLight = mSceneManager->createLight();
	//directionalLight->setType(Light::LightTypes::LT_DIRECTIONAL);
	//directionalLight->setDiffuseColour(1, 1, 1);
 //   directionalLight->setSpecularColour(1, 1, 1.);
	//directionalLight->setDirection(0, -1, 0);
	////asfasf->setPosition(0, 200, 0);
	//SceneNode* lightNode = mSceneManager->getRootSceneNode()->createChildSceneNode();
	//lightNode->translate(Vector3(0, 20000, 0));
	//lightNode->attachObject(directionalLight);

	mWorldTopRight = TO_BULLET(mCurrentFormat->size / 2);
	mWorldBottomLeft = TO_BULLET(-mCurrentFormat->size / 2);
	
	if (false)
	{
		for (int i = 0; i < MZ_SIZE; i++)
		{
			BtOgre::StaticMeshToShapeConverter aa = BtOgre::StaticMeshToShapeConverter();
			mMapEntities[i] = mSceneManager->createEntity(mMeshMaterialName[i].first);
			mMapEntities[i]->setMaterialName(mMeshMaterialName[i].second);
			mMapNodes[i] = mSceneManager->getRootSceneNode()->createChildSceneNode();
			mMapNodes[i]->attachObject(mMapEntities[i]);

			aa.addEntity(mMapEntities[i]);
			auto shape = aa.createTrimesh();
			shape->buildOptimizedBvh();
			btTriangleInfoMap* triangleInfoMap = new btTriangleInfoMap();
			btGenerateInternalEdgeInfo(shape,triangleInfoMap);

			mMapCollisionObject[i] = new btCollisionObject();//0, new btDefaultMotionState(), shape);
			mMapCollisionObject[i]->setCollisionShape(shape);
			mMapCollisionObject[i]->setRestitution(0);
			//body->setActivationState(ISLAND_SLEEPING); // very important (otherwise bodies on it might not fall asleep!)
			mMapCollisionObject[i]->customFlags = 4;
			btTransform transfa;
			transfa.setIdentity();
			mMapCollisionObject[i]->setWorldTransform(transfa);
			Physics::getSingleton()->getWorld()->addCollisionObject(mMapCollisionObject[i],btBroadphaseProxy::StaticFilter,btBroadphaseProxy::AllFilter&(~(btBroadphaseProxy::StaticFilter | btBroadphaseProxy::KinematicFilter)));

		}
	}
	//fixMapLimits();

	//mWorldBottomLeft.setY(mMapWorld->information.origin.y - mMapWorld->information.size.y / 2);
	//mWorldBottomLeft.setX(mMapWorld->information.origin.x - mMapWorld->information.size.x / 2);
	//mWorldBottomLeft.setZ(mMapWorld->information.origin.z - mMapWorld->information.size.z / 2);
	recreateSkyBox();

	mStaticGeometry = mSceneManager->createStaticGeometry(requestNewStaticGeometryName());
	//mStaticGeometryCircle = mSceneManager->createStaticGeometry(requestNewStaticGeometryName());
	mStaticGeometry->setRegionDimensions (mCurrentFormat->size);
	//mStaticGeometryCircle->setRegionDimensions (Vector3(1000000, 1000000, 1000000));

	for (int i = 0; i < mCurrentFormat->blocks.size(); i++)
	{
		Platform* plat = generateMapBlock(mCurrentFormat->blocks[i], getGeneratorType() != WGT_WORLD_EDITOR);
		
		/*if (plat->getBox()->getBoxType() == WorldBox::BoxType::BT_GOAL)
		{
			btVector3 sizeGoal = WorldBox::typeSize(WorldBox::BoxType::BT_GOAL);
			WorldTouchableObject* touchable = new WorldTouchableObject(TO_OGRE(plat->getBox()->getBody()->getWorldTransform().getOrigin()) + Vector3(0, sizeGoal.y(), 0), Vector3(sizeGoal.x() / 2, 20, sizeGoal.z() / 2));
			mTouchableGoals.push_back(touchable);
		}*/

		if (mCurrentFormat->blocks[i]->isPoint)
		{
			plat->addActivator(WorldActivator::AT_POINTS);
			//pointHelper->point = ;
			//pointHelper->respawn = ;
			mPoints.push_back(plat->getActivator());
		}
		else if (mCurrentFormat->blocks[i]->isRespawn)
		{
			plat->addActivator(WorldActivator::AT_RESPAWN);
			if (mCurrentFormat->firstRespawnOffset == i)
			{
				mFirstRespawn = plat->getActivator();
			}
			//pointHelper->point = ;
			//pointHelper->respawn = ;
			mRespawns.push_back(plat->getActivator());
		}
		mPlatforms.push_back(plat);
		if (plat->Type() == PlatformType::PT_MOVABLE)
		{
			mMovablePlatforms.push_back((MovablePlatform*)plat);
		}
	}
	
	if (!mFirstRespawn && mRespawns.size() > 0) mFirstRespawn = mRespawns[rand() % mRespawns.size()];

	// monster
	//mMonsterState = new MonsterState();

	mStaticGeometry->setCastShadows(true);
	mStaticGeometry->build();

	// monsters
//	mMonsters[Monster::MT_BLACK_KING] = new BlackKing();
//	mMonsters[Monster::MT_HOLY_MARBLE] = new HolyMarble();
}

Vector3 WorldGenerator::worldRespawn2vector(WorldActivator* respawn)
{
	return respawn->getBase() + Vector3(0, RESPAWN_Y_MARGIN, 0);
	/*if (respawn->getParent()->Type() == PlatformType::PT_MOVABLE)
	{
		TO_OGRE(static_cast<MovablePlatform*>(respawn->getParent())->getOrigin()) + Vector3(0, RESPAWN_Y_MARGIN, 0);
	}
	else
	{
	}*/
}

Vector3 WorldGenerator::getInitialRespawn()
{
	if (mRespawns.size() >= 1)
	{
		if (mFirstRespawn)
		{
			mFirstRespawn->setActivation(true);
			return worldRespawn2vector(mFirstRespawn);
		}

		WorldActivator* point = (mRespawns[rand() % mRespawns.size()]);//->point + Vector3(0, RESPAWN_Y_MARGIN, 0);
		point->setActivation(true);
		mCurrentPlayerActivator  = point;
		return worldRespawn2vector(point);
	}
	else
	{
		return Vector3::ZERO;
	}
}

Vector3 WorldGenerator::getRandomRespawn()
{
	if (mRespawns.size() >= 1)
	{
		return worldRespawn2vector(mRespawns[rand() % mRespawns.size()]);//->point + Vector3(0, RESPAWN_Y_MARGIN, 0);
	}
	else
	{
		return Vector3::ZERO;
	}
}

Platform* WorldGenerator::generateMapBlock(MapFormat::Block* block, bool useStatic)
{
	btTransform transf;
	transf.setIdentity();
	transf.setOrigin(TO_BULLET(block->origin));
	transf.setRotation(btQuaternion(btVector3(0, 1, 0), block->rotation.valueRadians()));//
	TO_BULLET(block->origin);
	
	if (block->materialIndex < 0 || block->materialIndex >= WorldBox::CM_SIZE)
	{
		block->materialIndex = (WorldBox::CM_TUTORIAL_PATH_1);
	}

	if ((block->linearSpeed > 0 && block->linearLocation.length() > 0) || block->rotationSpeed)//.isLinear || block->movableArgs.isRotate || block->movableArgs.isScale)
	{
		MovablePlatform* plat = new MovablePlatform(new WorldBox(0, transf, block->materialIndex, false));

		if ((block->linearSpeed && block->linearSpeed))
		{
			plat->addFlag(MovablePlatform::MP_LINEAR);
			plat->setLinearMovementArgs(TO_BULLET(block->linearLocation), block->linearSpeed);

			if (block->isRotationMovement)
			{
				plat->setLinearMovementType(false);
				plat->setRotationRight(block->isRightRotationMovement);
			}
		}
		if (block->rotationSpeed)
		{
			plat->addFlag(MovablePlatform::MP_ROTATING);
			plat->setRotateArgs(Degree(block->rotationSpeed));
		}
		
		if (block->extraArgsBit & (1 << 1))
		{
			plat->setBreakable(true);
		}
		
		if (block->extraArgsBit & (1 << 2))
		{
			//plat->setKillBox(true);
		}

		return plat;
	}
	else
	{
		Platform* plat = 0;
		
		if (WorldBox::materialIsBreakable((WorldBox::CustomMaterial)block->materialIndex))//block->extraArgsBit & (1 << 1))
		{
			plat = new Platform(new WorldBox(mStaticGeometry, transf, block->materialIndex, false));
			plat->setBreakable(true);
		}
		else
		{
			plat = new Platform(new WorldBox(mStaticGeometry, transf, block->materialIndex, useStatic));
		}
		
		if (WorldBox::materialIsKillbox((WorldBox::CustomMaterial)block->materialIndex))//(block->extraArgsBit & (1 << 2))
		{
//			plat->setKillBox(true);
//			if (WorldBox::materialIsTopKillBox(WorldBox::CustomMaterial)block->materialIndex)
//			{
//			}
		}

		return plat;
		//mPlatforms.push_back();
	}
}

void WorldGenerator::resetAllMovablePlatforms()
{
	for (int i = 0; i < mMovablePlatforms.size(); i++)
	{
		mMovablePlatforms[i]->_resetTime();
	}

	mTimeSinceStart = 0;
}

void WorldGenerator::generatePlatformAt(btVector3& origin, int x, int y, int maxX, int maxY)
{
}

bool WorldGenerator::isInsideWorld(Vector3 aabbMax, Vector3 aabbMin)
{
	if ((mWorldTopRight.x() >= aabbMax.x && mWorldBottomLeft.x() <= aabbMin.x) &&
		(mWorldTopRight.z() >= aabbMax.z && mWorldBottomLeft.z() <= aabbMin.z) &&
		(mWorldTopRight.y() >= aabbMax.y && mWorldBottomLeft.y() <= aabbMin.y))
	{
		return true;
	}
	else
	{
		return false;
	}
	//if (
	//	mWorldBottomLeft.x() < position.x() &&
	//	mWorldBottomLeft.y() < position.y() &&
	//	mWorldBottomLeft.z() < position.z() &&
	//	mWorldTopRight.x() > position.x() &&
	//	mWorldTopRight.y() > position.y() &&
	//	mWorldTopRight.z() > position.z())
	//{
	//	return true;
	//}
	//else
	//{
	//	return false;
	//}
}

WorldGenerator::MapZones WorldGenerator::getCurrentMapZone(Vector3 position)
{
	for (int i = (MapZones)(0); i < MZ_SIZE; i++)
	{
		if ((position.x <= mMapZonesAabb[i].second.x && position.x >= mMapZonesAabb[i].first.x) &&
			(position.y <= mMapZonesAabb[i].second.y && position.y >= mMapZonesAabb[i].first.y) &&
			(position.z <= mMapZonesAabb[i].second.z && position.z >= mMapZonesAabb[i].first.z))
		{
			return (MapZones)i;
		}
	}

	return WorldGenerator::MapZones::MZ_ICE_CORE;
}

void WorldGenerator::platformShaderArgs()
{
	for (int i = 0; i < mPlatforms.size(); i++)
	{
		mPlatforms[i]->getBox()->getEntity()->getSubEntity(0)->setCustomParameter(1, Vector4(mPlatforms[i]->getShaderColorType(), 0, 0, 0));
	}
}

void WorldGenerator::playerPlatformShaderArgs(Vector4 crashCoordinates, Vector3 playerPos)
{
	platformShaderArgs();
	
	for (int i = 0; i < mPlatforms.size(); i++)
	{
		if (mPlatforms[i]->isStatic()) continue;

		mPlatforms[i]->getBox()->getEntity()->getSubEntity(0)->setCustomParameter(2, crashCoordinates);
		Vector4 args = Vector4::ZERO;
		if (mPlatforms[i]->isBreakable() && mPlatforms[i]->startedBreaking())
		{
			if (mPlatforms[i]->getTimeSinceStartedBreaking() <= BREAKABLE_PLATFORM_TIME_TO_BREAK)
			{
				args.x = 1;
				args.y = 1;
				args.z = mPlatforms[i]->getTimeSinceStartedBreaking() / BREAKABLE_PLATFORM_TIME_TO_BREAK;
			}
			else if (mPlatforms[i]->getTimeSinceStartedBreaking() >= BREAKABLE_PLATFORM_TIME_TO_BREAK + BREAKABLE_PLATFORM_TIME_TO_RECREATE - BREAKABLE_PLATFORM_TIME_RECONSTRUCT)
			{
				args.x = 1;
				args.y = 2;
				args.z = mPlatforms[i]->getTimeSinceStartedBreaking() / (BREAKABLE_PLATFORM_TIME_TO_BREAK + BREAKABLE_PLATFORM_TIME_TO_RECREATE - BREAKABLE_PLATFORM_TIME_RECONSTRUCT);
			}
			else
			{
				args.x = 1;
				args.y = 3;
			}
		}
		mPlatforms[i]->getBox()->getEntity()->getSubEntity(0)->setCustomParameter(3, args);
		//mPlatforms[i]->getBox()->getEntity()->getSubEntity(0)->setCustomParameter(2, Vector4(playerPos.x, playerPos.y, playerPos.z, 0.0));
		auto size = mPlatforms[i]->getBox()->size();
		mPlatforms[i]->getBox()->getEntity()->getSubEntity(0)->setCustomParameter(0, Vector4(size.x, size.y, size.z, 0.0));
	}

#if ENABLE_STATIC_GEOMETRY
	if (mStaticGeometry)
	{
		StaticGeometry::RegionIterator regIt = mStaticGeometry->getRegionIterator();
		while (regIt.hasMoreElements())
		{
			StaticGeometry::Region* region = regIt.getNext();
			StaticGeometry::Region::LODIterator lodIt = region->getLODIterator();
			while (lodIt.hasMoreElements())
			{
				StaticGeometry::LODBucket* bucket = lodIt.getNext();
				StaticGeometry::LODBucket::MaterialIterator matIt = bucket->getMaterialIterator();
				while (matIt.hasMoreElements())
				{
					StaticGeometry::MaterialBucket* mat = matIt.getNext();
					StaticGeometry::MaterialBucket::GeometryIterator geomIt = mat->getGeometryIterator();
					while (geomIt.hasMoreElements())
					{
						StaticGeometry::GeometryBucket* geom = geomIt.getNext();
						geom->setCustomParameter(0, crashCoordinates);
						geom->setCustomParameter(1, Vector4::ZERO);
					}
				}
			}
		}
	}
#endif
}

WorldActivator* WorldGenerator::getRespawnAt(Vector3& aabbMax, Vector3& aabbMin)
{
	for (int i = 0; i < mRespawns.size(); i++)
	{
		if (mRespawns[i]->isTouchingObject(aabbMax, aabbMin))
		{
			return mRespawns[i];
		}
	}
	return 0;
}

WorldActivator* WorldGenerator::getPointAt(Vector3& aabbMax, Vector3& aabbMin)
{
	for (int i = 0; i < mPoints.size(); i++)
	{
		if (mPoints[i]->isTouchingObject(aabbMax, aabbMin))
		{
			return mPoints[i];
		}
	}
	return 0;
}

void WorldGenerator::activateRespawn(WorldActivator* point)
{
	for (int i = 0; i < mRespawns.size(); i++)
	{
		mRespawns[i]->setActivation(false);
	}
	point->setActivation(true);
	mCurrentPlayerActivator = point;
}

void WorldGenerator::generateMiddlePlatform(btVector3& position, Radian rotation)
{
}

void WorldGenerator::fillGeneratorArray()
{
}

void WorldGenerator::newCharacterRespawn(Player* player)
{
	resetAllMovablePlatforms();
	for (int i = 0; i < mPlatforms.size(); i++)
	{
		if (mPlatforms[i]->isBreakable())
		{
			mPlatforms[i]->rebuildBreak();
		}
		if (mPlatforms[i]->getActivator() && mPlatforms[i]->getActivator()->isActivated() && mPlatforms[i]->getActivator()->getActivatorType() == WorldActivator::AT_POINTS)
		{
			mPlatforms[i]->getActivator()->setActivation(false);
		}
	}
	//Playground::destroySceneNode(mDeathNode);
}

void WorldGenerator::newCharacterDied(Player* player, Vector3 normal)
{
	if (mGameState->currentScore > mGameState->maxScore)
	{
		mGameState->maxScore = mGameState->currentScore;
	}
	//Radian yaw = player->getBodyNode()->_getDerivedOrientation().getYaw();
	//yaw += Degree(180);
	//mDeathNode = mSceneManager->getRootSceneNode()->createChildSceneNode();
	//mDeathNode->translate(player->getLocation() + Vector3(0, 10, 0));
	//mDeathNode->setOrientation(Quaternion(yaw, normal));
	//Entity* deathSymbol = mSceneManager->createEntity("death-plane");
	//deathSymbol->setMaterialName("	");
	//mDeathNode->attachObject(deathSymbol);
}

void WorldGenerator::update(const FrameEvent& evt, std::vector<Player*>* players)
{
	mTimeSinceStart += evt.timeSinceLastFrame;

	for (int i = 0; i < mRespawns.size(); i++)
	{
		mRespawns[i]->update(evt);
	}
	for (int i = 0; i < mPoints.size(); i++)
	{
		mPoints[i]->update(evt);
	}

	if (mTimeSinceStart >= PLATFORM_UPDATE_TIME_START)
	{
		for (int i = 0; i < mPlatforms.size(); i++)
		{
			mPlatforms[i]->update(evt);
		}
	}
	
	if (mMapMonster)
	{
		mMapMonster->update((*players)[0], evt);
	}

	for (int i = 0; i < Monster::MT_SIZE; i++)
	{
		if (mMonsters[i])
		{
			mMonsters[i]->update((*players)[0], evt);
		}
	}

	//for (int i = 0; i < mMovablePlatforms.size(); i++)
	//{
	//	mMovablePlatforms[i]->update(evt);
	//}
}

WorldGenerator* WorldGenerator::mCurrentInstance = 0;