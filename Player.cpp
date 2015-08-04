#include "Player.h"
#include "Physics.h"
#include "TextRenderer.h"
#include "Playground.h"
#include <BulletSoftBody\btSoftBody.h>
#include <BulletSoftBody\btSoftBodyHelpers.h>
//#include "BlackKing.h"

void GetMeshInformation(
	Entity *entity,
	size_t &vertex_count,
	Ogre::Vector3* &vertices,
	size_t &index_count,
	Ogre::uint32* &indices,
	const Ogre::Vector3 &position,
	const Ogre::Quaternion &orient,
	const Ogre::Vector3 &scale)
{
	bool added_shared = false;
	size_t current_offset = 0;
	size_t shared_offset = 0;
	size_t next_offset = 0;
	size_t index_offset = 0;
	vertex_count = index_count = 0;

	Ogre::MeshPtr mesh = entity->getMesh();
 
 
	bool useSoftwareBlendingVertices = entity->hasSkeleton();
 
	if (useSoftwareBlendingVertices)
	{
	  entity->_updateAnimation();
	}
 
	// Calculate how many vertices and indices we're going to need
	for (unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i)
	{
		Ogre::SubMesh* submesh = mesh->getSubMesh( i );
 
		// We only need to add the shared vertices once
		if(submesh->useSharedVertices)
		{
			if( !added_shared )
			{
				vertex_count += mesh->sharedVertexData->vertexCount;
				added_shared = true;
			}
		}
		else
		{
			vertex_count += submesh->vertexData->vertexCount;
		}
 
		// Add the indices
		index_count += submesh->indexData->indexCount;
	}
 
 
	// Allocate space for the vertices and indices
	vertices = new Ogre::Vector3[vertex_count];
	indices = new Ogre::uint32[index_count];
 
	added_shared = false;
 
	// Run through the submeshes again, adding the data into the arrays
	for ( unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i)
	{
		Ogre::SubMesh* submesh = mesh->getSubMesh(i);
 
		//----------------------------------------------------------------
		// GET VERTEXDATA
		//----------------------------------------------------------------
		Ogre::VertexData* vertex_data;
 
		//When there is animation:
		if(useSoftwareBlendingVertices)
			vertex_data = submesh->useSharedVertices ? entity->_getSkelAnimVertexData() : entity->getSubEntity(i)->_getSkelAnimVertexData();
		else
			vertex_data = submesh->useSharedVertices ? mesh->sharedVertexData : submesh->vertexData;
 
 
		if((!submesh->useSharedVertices)||(submesh->useSharedVertices && !added_shared))
		{
			if(submesh->useSharedVertices)
			{
				added_shared = true;
				shared_offset = current_offset;
			}
 
			const Ogre::VertexElement* posElem =
				vertex_data->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION);
 
			Ogre::HardwareVertexBufferSharedPtr vbuf =
				vertex_data->vertexBufferBinding->getBuffer(posElem->getSource());
 
			unsigned char* vertex =
				static_cast<unsigned char*>(vbuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
 
			// There is _no_ baseVertexPointerToElement() which takes an Ogre::Real or a double
			//  as second argument. So make it float, to avoid trouble when Ogre::Real will
			//  be comiled/typedefed as double:
			//      Ogre::Real* pReal;
			float* pReal;
 
			for( size_t j = 0; j < vertex_data->vertexCount; ++j, vertex += vbuf->getVertexSize())
			{
				posElem->baseVertexPointerToElement(vertex, &pReal);
 
				Ogre::Vector3 pt(pReal[0], pReal[1], pReal[2]);
 
				vertices[current_offset + j] = (orient * (pt * scale)) + position;
			}
 
			vbuf->unlock();
			next_offset += vertex_data->vertexCount;
		}
 
 
		Ogre::IndexData* index_data = submesh->indexData;
		size_t numTris = index_data->indexCount / 3;
		Ogre::HardwareIndexBufferSharedPtr ibuf = index_data->indexBuffer;
 
		bool use32bitindexes = (ibuf->getType() == Ogre::HardwareIndexBuffer::IT_32BIT);
 
		void* hwBuf = ibuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY);
 
		size_t offset = (submesh->useSharedVertices)? shared_offset : current_offset;
		size_t index_start = index_data->indexStart;
		size_t last_index = numTris*3 + index_start;
 
		if (use32bitindexes) {
			Ogre::uint32* hwBuf32 = static_cast<Ogre::uint32*>(hwBuf);
			for (size_t k = index_start; k < last_index; ++k)
			{
				indices[index_offset++] = hwBuf32[k] + static_cast<Ogre::uint32>( offset );
			}
		} else {
			Ogre::uint16* hwBuf16 = static_cast<Ogre::uint16*>(hwBuf);
			for (size_t k = index_start; k < last_index; ++k)
			{
				indices[ index_offset++ ] = static_cast<Ogre::uint32>( hwBuf16[k] ) +
					static_cast<Ogre::uint32>( offset );
			}
		}
 
		ibuf->unlock();
		current_offset = next_offset;
	}
}

Player::Player(WorldGenerator* generator, Camera* camera, SceneManager* sceneManager, RenderWindow* window)
	: mLastStandingYaw(0),
	mGenerator(generator),
	mReachedGoal(false),
	mSceneManager(sceneManager),
	shouldKillPlayer(false),
	mTimeOnGround(0.0),
	//mPressJumpLastUpdate(false),
	mLastVectorVelocity(Vector3(0, 0, 0)),
#if ENABLE_WALL_GRAB
	mGrabbingPointer(0),
#endif
	mParticleHolder(0),
	mLastVerticalVelocity(0),
	mTimeAfterLengthyJump(0),
	mTimeSinceLengthyJump(0),
	mRunningStep(0),
	mTimeOnAir(0),
	mSoundManager(0),
	mDeathBillboardSet(0),
	mDeathBillboard(0),
	mStillLandingAfterLengthyJump(false),
	mGameOver(false),
	mTimeLastJump(0),
	mLastJumpType(JumpType::JT_NONE),
	mTimeSinceWallJump(0),
	mSecondsSinceDie(0),
#if ENABLE_WALL_GRAB
	mGrabbingWall(false),
	mStartedClimbingUpLedge(false),
	mTimeSinceClimbingUpLedge(0),
	mTimeForClimbingUpLedge(0),
	mTimeSinceLastWallGrab(0),
#endif
	mMovablePlatformBelow(0),
	mRigidBody(0),
//	mCapeSofyBody(0),
	mCurrentZoom(ZM_2),
	mCurrentTrail(0),
	mTimeSinceLastTrail(0.0)
{
	mPressJumpLastUpdate[0] = false; mPressJumpLastUpdate[1] = false; mPressJumpLastUpdate[2] = false;

	mSoundManager = SoundManager::singleton;

	mCurrentRespawnPoint = generator->getInitialRespawn();
	mMainNode = sceneManager->getRootSceneNode()->createChildSceneNode();
	mMainNode->translate((mCurrentRespawnPoint));
	mBodyNode = mMainNode->createChildSceneNode();
	mCameraPivot = mMainNode->createChildSceneNode();//
	mCameraYaw = mCameraPivot->createChildSceneNode();
	mCameraPitch = mCameraYaw ->createChildSceneNode();
	mCameraHolder = mCameraPitch->createChildSceneNode();
	mCameraHolder->translate(Vector3(0, PLAYER_BODY_Y / 3, CAMERA_ZOOM_DEFAULT));
	
	mCamera = camera;
	_onPlayerToggle();
	
	/*SceneNode* aasfasf = ((SceneNode*)_::aa)->createChildSceneNode();// attachObject(mCamera);
	aasfasf->translate(10, 2, 0);
	aasfasf->attachObject(mCamera);
	aasfasf->lookAt(Vector3(0, -1500, 0), Node::TS_WORLD);*/
	
	mPlayerEntity = sceneManager->createEntity("main-character.mesh");
	mPlayerEntity->setMaterialName("Character/Default");//("Character/Default");
	mPlayerEntity->getMesh()->setAutoBuildEdgeLists(true);
	mPlayerEntity->getMesh()->buildEdgeList();
	mPlayerEntity->setCastShadows(true);
	setupAnimations();
	mBodyNode->attachObject(mPlayerEntity);
	
	btDiscreteDynamicsWorld* world = Physics::getSingleton()->getSingleton()->getWorld();

	Vector3 size(PLAYER_BODY_X, PLAYER_BODY_Y, PLAYER_BODY_Z);
	mBodyDimensions = TO_BULLET(size);
	btConvexShape* shape2 = new btCylinderShape(btVector3(PLAYER_BODY_X, PLAYER_BODY_Y, PLAYER_BODY_Z));
//	btConvexShape* shape2 = new btCapsuleShape(PLAYER_BODY_X, PLAYER_BODY_Y);
	mRigidBody = new btRigidBody(1, new btDefaultMotionState(), shape2, btVector3(0, 0, 0));
	mRigidBody->setActivationState(DISABLE_DEACTIVATION);
	mRigidBody->setCollisionFlags (mRigidBody->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
	mRigidBody->setRestitution(0);
	mRigidBody->setFriction(0);
	mRigidBody->setAngularFactor(btVector3(0, 0, 0));
	mRigidBody->customFlags = 1;
//	mRigidBody->setLinearFactor(btVector3(0, 1, 0));
	mKinematicController = new PlayerController(this, mRigidBody);
	
	btTransform transf;
	transf.setIdentity();
	transf.setOrigin(TO_BULLET(mCurrentRespawnPoint));
	mRigidBody->setWorldTransform(transf);

	//world->addAction(mKinematicController);
	world->addRigidBody(mRigidBody);//, btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::StaticFilter|btBroadphaseProxy::DefaultFilter);
	resetCamera();
	mCameraPitch->pitch(Degree(-30));

	//mCapeEntity = mSceneManager->createEntity("character-cape.mesh");
	//auto softbodyinfo = new btSoftBodyWorldInfo();
	//auto converter = BtOgre::StaticMeshToSoftBodyConverter(softbodyinfo, mCapeEntity);
	//mCapeSofyBody = converter.createSoftBodyFromTrimesh();
	//btTransform a;
	//a.setIdentity();
	//mCapeSofyBody->setWorldTransform(a);
	//CapeSofyBody->appendAnchor(0, mRigidBody, btVector3(0, 0, 3));

	mDeathBillboardSet = mSceneManager->createBillboardSet();
	mDeathBillboardSet->setMaterialName("General/DeathEffect");
	mDeathBillboard = mDeathBillboardSet->createBillboard(0, 0, 0);
	SceneNode* node = mBodyNode->createChildSceneNode();
	node->scale(1./3, 1./3, 1./3);
	node->attachObject(mDeathBillboardSet);
	mDeathBillboardSet->setVisible(false);
	mDeathBillboardSet->setRenderQueueGroup(RENDER_QUEUE_6);
	
	onCameraChange(mCurrentZoom);

	Physics::getSingleton()->getWorld()->getBroadphase()->getOverlappingPairCache()->cleanProxyFromPairs(
		mRigidBody->getBroadphaseHandle(),
		Physics::getSingleton()->getWorld()->getDispatcher()
	);
	
	for (int i = 0; i < PLAYER_MAX_COPY_TRAIL; i++)
	{
		mPlayerTrails[i] = new Trail();
	}

	/*Entity* ent = mSceneManager->createEntity("Cube.mesh");
	ent->setMaterialName("Platform/SmallBox");
	mSceneManager->getRootSceneNode()->createChildSceneNode()->attachObject(ent);
	BtOgre::StaticMeshToShapeConverter converter(ent);
	btBvhTriangleMeshShape* colshape = converter.createTrimesh();
	btTransform transform;
	transform.setIdentity();
	btRigidBody* body = new btRigidBody(0, new btDefaultMotionState(), colshape);
	body->setWorldTransform(transform);
	body->setLinearFactor(btVector3(0, 0, 0));
	body->setAngularFactor(btVector3(0, 0, 0));
	Physics::getSingleton()->getWorld()->addRigidBody(body);*/
}

Player::~Player(void)
{
	for (int i = 0; i < mCrashEffects.size(); i++)
	{
		delete mCrashEffects[i];
	}

	delete mKinematicController;
//	Playground::destroySceneNode(mMainNode);
}

Player::CrashEffect::CrashEffect(Vector3 position, Quaternion orientation, CrashEffectType type)
	: shouldDelete(false),
	node(0),
	entity(0),
	time(0),
	secondsEffect(0.5),
	type(type)
{
	switch(type)
	{
	case CrashEffect::WALL_JUMP:
		secondsEffect = SECONDS_WALL_JUMP_EFFECT;
		break;
	case CrashEffect::LONG_JUMP:
		secondsEffect = SECONDS_LONG_JUMP_EFFECT;
		break;
	case CrashEffect::NORMAL_STATIC_JUMP:
	case CrashEffect::NORMAL_MOVING_JUMP:
		secondsEffect = SECONDS_NORMAL_JUMP_EFFECT;
		break;
	case CrashEffect::AFTER_LANDING:
		secondsEffect = SECONDS_AFTER_FALLING_EFFECT;
		break;
	case CrashEffect::AFTER_LANDING_SMALL:
		secondsEffect = SECONDS_AFTER_FALLING_EFFECT_SMALL;
		break;
#if ENABLE_BACKWARDS_JUMP
	case CrashEffect::HIGH_JUMP:
		secondsEffect = SECONDS_HIGH_JUMP_EFFECT;
#endif
	}
			
	node = Root::getSingleton().getSceneManager(SCENE_MANAGER_NAME)->getRootSceneNode()->createChildSceneNode();
	entity = Root::getSingleton().getSceneManager(SCENE_MANAGER_NAME)->createEntity("jump-crash.mesh");
	material = static_cast<MaterialPtr>(MaterialManager::getSingleton().getByName("Character/JumpEffect"));
	material = material->clone(requestNewRandomMaterialName());

	entity->setMaterialName("Character/JumpEffect");
	entity->setCastShadows(false);
			
	node->attachObject(entity);
	node->translate(position);
	Real scaleBase = 3;

	if (type == AFTER_LANDING || type == AFTER_LANDING_SMALL)
	{
		if (type == AFTER_LANDING)
		{
			node->scale(3,3, 1);
		}
		else
		{
			node->scale(1.7,1.7, 1);
		}
		orientation = orientation * Quaternion(Degree(-90), Vector3::UNIT_X);
	}
	else
	{
		if (type == WALL_JUMP || type == NORMAL_STATIC_JUMP || type == NORMAL_MOVING_JUMP)
		{
			node->scale(1.7,1.7, 1);
		}
		else
		{
			node->scale(1.7,1.7, 1);
		}
	}
	node->setOrientation(orientation);
}
Player::CrashEffect::~CrashEffect()
{
	Playground::destroySceneNode(node);
	MaterialManager::getSingleton().remove(material->getName());
}
void Player::CrashEffect::update(const FrameEvent& evt)
{

	time += evt.timeSinceLastFrame;
	Real alpha = time / secondsEffect;
	if (alpha > 1.0)
	{
		alpha = 1.0;
	}
	if (alpha < 0.0)
	{
		alpha = 0.0;
	}
	if (!material.isNull())
	{
		//material->setAmbient(ColourValue(JUMP_EFFECT_DIFFUSE_RGB, 1.0 - alpha));
		material->setDiffuse(JUMP_EFFECT_DIFFUSE_RGB, 1.0 - alpha);
		entity->setMaterial(material);
	}

	if (time >= secondsEffect)
	{
		shouldDelete = true;
	}
}

Player::Trail::Trail()
{
	entity = Root::getSingleton().getSceneManager(SCENE_MANAGER_NAME)->createEntity("main-character.mesh");
	entity->setMaterialName("Character/Trail");
	node = Root::getSingleton().getSceneManager(SCENE_MANAGER_NAME)->getRootSceneNode()->createChildSceneNode();
	node->attachObject(entity);
	entity->setVisible(false);
	colorIndex = rand() % 5;
	resetFlag = false;
}

void Player::Trail::update(const FrameEvent& evt)
{
	timeSinceReset += evt.timeSinceLastFrame;
	if (resetFlag || timeSinceReset >= PLAYER_TIME_PER_COPY)
	{
		entity->setVisible(false);
	}
	else
	{
		entity->getSubEntity(0)->setCustomParameter(0, Vector4(possibleColors[colorIndex].r, possibleColors[colorIndex].g, possibleColors[colorIndex].b, timeSinceReset / PLAYER_TIME_PER_COPY));
	}
}

void Player::Trail::startTrail()
{
	resetFlag = false;
	timeSinceReset = 0;
	entity->setVisible(true);
}

void Player::Trail::reset(Vector3 position, Quaternion rotation, AnimationState* animationState)
{
	String animNames[] = {
		"Standing",
		"Falling",
		"Running",
		"HeapJump",
		"HighJump",
		"NormalJump",
		"WallJump",
		"GrabbingWall",
		"GoUpWall",
		"LandingAfterJump",
		"Dying",
		"Celebrate",
		"ActionSpin"
	};
	
	resetFlag = true;

	node->setPosition(position);
	node->setOrientation(rotation);
	colorIndex = rand() % 5;
	entity->getSubEntity(0)->setCustomParameter(0, Vector4(possibleColors[colorIndex].r, possibleColors[colorIndex].g, possibleColors[colorIndex].b, 0.0));
	
	auto animStates = entity->getAllAnimationStates();
	auto animIterator = animStates->getAnimationStateIterator();
	for (auto iterator = animIterator.begin(); iterator != animIterator.end(); iterator++)
	{
		iterator->second->setEnabled(false);
	}
	entity->getAnimationState(animationState->getAnimationName())->setEnabled(true);
	entity->getAnimationState(animationState->getAnimationName())->copyStateFrom(*animationState);
	
	entity->setVisible(false);

//	int randColor = rand();
}

void Player::update(const Ogre::FrameEvent& evt)//, bool isPaused)
{
	static double lastMovement = 0;
	Keys* keysSingleton = keysSingleton->singleton;

	if (keysSingleton->keystroke(Keys::K_ACTION))
	{
		//btCollisionShape* a = new btBoxShape(btVector3(PLAYER_BODY_X, PLAYER_BODY_Y, PLAYER_BODY_Z));
		//btVector3 asd;
		//a->calculateLocalInertia(1, asd);
		//btRigidBody* aa = new btRigidBody(1, new btDefaultMotionState(), a, asd);
		//aa->setAngularFactor(btVector3(0, 0, 0));
		//btTransform transf;transf.setIdentity();
		//transf.setOrigin(mRigidBody->getWorldTransform().getOrigin());
		//aa->setWorldTransform(transf);
		//Vector3 dir = Vector3::UNIT_Z * 500;
		//dir = Quaternion(mBodyNode->_getDerivedOrientation().getYaw(), Vector3::UNIT_Y) * dir;
		//aa->applyCentralImpulse(btVector3(dir.x, 200, dir.z));
		//Physics::getSingleton()->getWorld()->addRigidBody(aa);
	}

#if ENABLE_BACKWARDS_JUMP
	if (keysSingleton->isKeyDown(Keys::K_JUMP_LONG) && keysSingleton->isKeyDown(Keys::K_JUMP_NORMAL))
	{
		keysSingleton->onHighJumpClick();
	}
#endif
	
	for (int i = 0; i < PLAYER_MAX_COPY_TRAIL; i++)
	{
		mPlayerTrails[i]->update(evt);
	}
	
	Vector3 posCurrent = mCameraHolder->getPosition();
	if (keysSingleton->Wheel != 0 || keysSingleton->keystroke(Keys::K_CAMERA_ZOOM_ADD) || keysSingleton->keystroke(Keys::K_CAMERA_ZOOM_REDUCE))
	{
		if (keysSingleton->keystroke(Keys::K_CAMERA_ZOOM_ADD) || keysSingleton->Wheel < 0)
		{
			if (mCurrentZoom < ZM_SIZE - 1)
			{
				mCurrentZoom = (ZoomLevel)(mCurrentZoom + 1);
			}
		}
		else if (keysSingleton->keystroke(Keys::K_CAMERA_ZOOM_REDUCE) || keysSingleton->Wheel > 0)
		{
			if (mCurrentZoom > ZM_1)
			{
				mCurrentZoom = (ZoomLevel)(mCurrentZoom - 1);
			}
		}

		onCameraChange(mCurrentZoom);

	}

	if (_::options.useMouseForCameraMovement)
	{
		if (keysSingleton->MouseY)
		{
			mCameraPitch->pitch(Degree((_::options.invertY ? -1 : 1) * (CAMERA_SPEED_MIN + _::options.mouseSensitivity * CAMERA_SPEED_MAX_INCREASE))*evt.timeSinceLastFrame*keysSingleton->MouseY);
			onCameraChange(mCurrentZoom);
		}
		if (keysSingleton->MouseX)
		{
			mCameraYaw->yaw(Degree((_::options.invertX ? -1 : 1) * (CAMERA_SPEED_MIN + _::options.mouseSensitivity * CAMERA_SPEED_MAX_INCREASE))*evt.timeSinceLastFrame*keysSingleton->MouseX);
			onCameraChange(mCurrentZoom);
		}
		if (mCameraPitch->getOrientation().getPitch().valueAngleUnits() < -90)
		{
			mCameraPitch->pitch(-Degree((_::options.invertY ? -1 : 1) * (CAMERA_SPEED_MIN + _::options.mouseSensitivity * CAMERA_SPEED_MAX_INCREASE))*evt.timeSinceLastFrame*keysSingleton->MouseY);
		}
		if (mCameraPitch->getOrientation().getPitch().valueAngleUnits() > CAMERA_MAX_ANGLE_PITCH)
		{
			mCameraPitch->pitch(-Degree((_::options.invertY ? -1 : 1) * (CAMERA_SPEED_MIN + _::options.mouseSensitivity * CAMERA_SPEED_MAX_INCREASE))*evt.timeSinceLastFrame*keysSingleton->MouseY);
		}
	}

	if (keysSingleton->isKeyDown(Keys::K_CAMERA_LEFT))
	{
		mCameraYaw->yaw(Degree((_::options.invertX ? -1 : 1) * (CAMERA_SPEED_MIN + _::options.mouseSensitivity * CAMERA_SPEED_MAX_INCREASE))*evt.timeSinceLastFrame*-CAMERA_SPEED_KEY);
	}
	if (keysSingleton->isKeyDown(Keys::K_CAMERA_RIGHT))
	{
		mCameraYaw->yaw(Degree((_::options.invertX ? -1 : 1) * (CAMERA_SPEED_MIN + _::options.mouseSensitivity * CAMERA_SPEED_MAX_INCREASE))*evt.timeSinceLastFrame*CAMERA_SPEED_KEY);
	}
	if (keysSingleton->isKeyDown(Keys::K_CAMERA_TOP))
	{
		mCameraPitch->pitch(Degree((_::options.invertY ? -1 : 1) * (CAMERA_SPEED_MIN + _::options.mouseSensitivity * CAMERA_SPEED_MAX_INCREASE))*evt.timeSinceLastFrame*-CAMERA_SPEED_KEY);
	}
	if (keysSingleton->isKeyDown(Keys::K_CAMERA_BOTTOM))
	{
		mCameraPitch->pitch(Degree((_::options.invertY ? -1 : 1) * (CAMERA_SPEED_MIN + _::options.mouseSensitivity * CAMERA_SPEED_MAX_INCREASE))*evt.timeSinceLastFrame*CAMERA_SPEED_KEY);
	}
	if (mCameraPitch->getOrientation().getPitch().valueAngleUnits() < -90)
	{
		mCameraPitch->pitch(Degree((_::options.invertY ? -1 : 1) * (CAMERA_SPEED_MIN + _::options.mouseSensitivity * CAMERA_SPEED_MAX_INCREASE))*evt.timeSinceLastFrame*-CAMERA_SPEED_KEY);
	}
	if (mCameraPitch->getOrientation().getPitch().valueAngleUnits() > 90)
	{
		mCameraPitch->pitch(Degree((_::options.invertY ? -1 : 1) * (CAMERA_SPEED_MIN + _::options.mouseSensitivity * CAMERA_SPEED_MAX_INCREASE))*evt.timeSinceLastFrame*CAMERA_SPEED_KEY);
	}

	if (keysSingleton->isKeyDown(keysSingleton->K_CAMERA_ZOOM_RESET) && !keysSingleton->isKeyDownLastUpdate(keysSingleton->K_CAMERA_ZOOM_RESET))
	{
		resetCamera();
	}


	if (mGameOver)
	{
		mSecondsSinceDie += evt.timeSinceLastFrame;
		//if (mSecondsUntilRespawn < 0.0) mSecondsUntilRespawn = 0.0;
	}
	
	updateAnimations(evt.timeSinceLastFrame);
	

	bool isPressingMovement = false;
	// = isJumping();
	/*if (mRigidBody->getLinearVelocity().y() == 0)
	{
		mRigidBody->setLinearVelocity(btVector3(0, 100, 0));
	}*/
	Vector3 movement(0,0,0);
	//Radian rad = mCameraCenterMovementFlag ? mCameraCenterYaw : mCameraYaw->getOrientation().getYaw();
	Radian rad = mCameraYaw->getOrientation().getYaw();
	Degree toRotate(0);
	//btVector3 vector = BtOgre::Convert::toBullet(Quaternion(rad, Vector3::UNIT_Y) * BtOgre::Convert::toOgre(mPlayerBody->getLinearVelocity()));
	btVector3 vector = btVector3(0, 0, 0);//mPlayerBody->getLinearVelocity();
	btVector3 vectorHelper(0, 0, 0);
	
	if (mReachedGoal && mAnimations[PA_CELEBRATE]->getLength() <= mAnimations[PA_CELEBRATE]->getTimePosition())
	{
		setAnimation(PA_STANDING, true);
	}

#if ENABLE_WALL_GRAB
	if (mGrabbingWall)
	{
		mTimeOnAir = 0;
		
		mLastStandingYaw = mBodyNode->getOrientation().getYaw();

		if (mGrabbingPointer && mGrabbingPointer->isBreakable() && mGrabbingPointer->isBroken())
		{
			mGrabbingWall = false;
			setAnimation(PlayerAnimation::PA_FALLING_START);
			mTimeSinceLastWallGrab = 0;
			mTimeSinceClimbingUpLedge = 0;
			mStartedClimbingUpLedge = false;
		}
		else if (!mStartedClimbingUpLedge)
		{
			if (!keysSingleton->isKeyDownLastUpdate(keysSingleton->K_MOVEMENT_FORWARD) && keysSingleton->isKeyDown(keysSingleton->K_MOVEMENT_FORWARD))
			{
				setAnimation(PlayerAnimation::PA_GO_UP_WALL, true);
				mTimeSinceClimbingUpLedge = 0;
				mStartedClimbingUpLedge = true;
			}
			else if (!keysSingleton->isKeyDownLastUpdate(keysSingleton->K_MOVEMENT_BACK) && keysSingleton->isKeyDown(keysSingleton->K_MOVEMENT_BACK))
			{
				mGrabbingWall = false;
				setAnimation(PlayerAnimation::PA_FALLING_START);
				mTimeSinceLastWallGrab = 0;
			}
		}
		else
		{
			updateClimbUpAnimation(evt.timeSinceLastFrame);
		}
	}
	else if (!shouldBlockMovement())
#else
	if (!shouldBlockMovement())
#endif
	{
		if ((isTouchingBelow() || mStillLandingAfterLengthyJump))
		{
			mTimeOnGround += evt.timeSinceLastFrame;
			if (mTimeOnGround >= PLAYER_JUMP_NORMAL_TIME_INCREASE_HEIGHT)
			{
				mLastJumpType = JumpType::JT_NONE;
			}

			if (mTimeOnAir > 0)
			{
				mLastStandingYaw = mBodyNode->getOrientation().getYaw();
			}

			if (mTimeOnAir >= PLAYER_TIME_LENGTHY_JUMP / 2)
			{
				setAnimation(PlayerAnimation::PA_LANDING_AFTER_JUMP, true);
				if (mTimeOnAir >= PLAYER_TIME_LENGTHY_JUMP)
				{
					mBodyNode->setOrientation(Quaternion(mLastStandingYaw, Vector3::UNIT_Y));
					mStillLandingAfterLengthyJump = true;
					mTimeSinceLengthyJump = 0;
					mLastVerticalVelocity = 0;
					mCrashEffects.push_back(new CrashEffect(getLocation() - Vector3(0, PLAYER_BODY_Y * 0.85, 0), mBodyNode->_getDerivedOrientation(), CrashEffect::AFTER_LANDING));
				}
				else
				{
					//mCrashEffects.push_back(new CrashEffect(getLocation() - Vector3(0, PLAYER_BODY_Y * 0.85, 0), mBodyNode->_getDerivedOrientation(), CrashEffect::AFTER_LANDING_SMALL));
				}
			}
			if (mTimeOnAir >= PLAYER_TIME_LENGTHY_JUMP && Math::Abs(mLastVerticalVelocity) >= PLAYER_MAX_FALL_SPEED_CRASH)
			{
			}
			double lastTimeOnAir = mTimeOnAir;
			mTimeOnAir = 0;
			

			if (mStillLandingAfterLengthyJump)
			{
				lastMovement = 0;
				mKinematicController->stopWalking();

				isPressingMovement = false;
				mTimeSinceLengthyJump += evt.timeSinceLastFrame;
				if (mTimeSinceLengthyJump >= mTimeAfterLengthyJump)
				{
					mStillLandingAfterLengthyJump = false;
				}
			}
			else
			{
				if (mTimeOnAir >= PLAYER_JUMP_NORMAL_TIME_INCREASE_HEIGHT)
				{
					mLastJumpType = JumpType::JT_NONE;
				}
				lastMovement += evt.timeSinceLastFrame;

				vectorHelper = btVector3(0, 0, 0); 

				if (keysSingleton->useDegreeMovement)
				{
					if (keysSingleton->movementVector.length() != 0)
					{
						toRotate = Math::ATan2((keysSingleton->movementVector.x), (keysSingleton->movementVector.z));
						vectorHelper = TO_BULLET(Quaternion(rad, Vector3::UNIT_Y) * keysSingleton->movementVector) * PLAYER_MOVEMENT_SPEED;
					}
					vectorHelper.setY(0);
					//vectorHelper = TO_BULLET(Quaternion(rad, Vector3::UNIT_Y) * TO_OGRE(vectorHelper));
					//Quaternion(Math::ATan2(touching.x(), touching.z()), Vector3::UNIT_Y)
				}
				
				
				if (vectorHelper.length() == 0)
				{
					if (keysSingleton->isKeyDown(keysSingleton->K_MOVEMENT_FORWARD))
					{
						if (keysSingleton->isKeyDown(keysSingleton->K_MOVEMENT_LEFT))
						{
							vectorHelper.setZ(-PLAYER_MOVEMENT_SPEED * Math::Cos(rad + Degree(45)));
							vectorHelper.setX(-PLAYER_MOVEMENT_SPEED * Math::Sin(rad + Degree(45)));
							toRotate = Degree(180+45);
						}
						else if (keysSingleton->isKeyDown(keysSingleton->K_MOVEMENT_RIGHT))
						{
							vectorHelper.setZ(-PLAYER_MOVEMENT_SPEED * Math::Cos(rad + Degree(360-45)));
							vectorHelper.setX(-PLAYER_MOVEMENT_SPEED * Math::Sin(rad + Degree(360-45)));
							toRotate = Degree(90 + 45);
						}
						else
						{
							vectorHelper.setZ(PLAYER_MOVEMENT_SPEED * Math::Cos(rad + Degree(180)));
							vectorHelper.setX(PLAYER_MOVEMENT_SPEED * Math::Sin(rad + Degree(180)));
							toRotate = Degree(180);
						}
					}
					else if (keysSingleton->isKeyDown(keysSingleton->K_MOVEMENT_BACK))
					{
						if (keysSingleton->isKeyDown(keysSingleton->K_MOVEMENT_LEFT))
						{
							vectorHelper.setZ(PLAYER_MOVEMENT_SPEED * Math::Cos(rad + Degree(360-45)));
							vectorHelper.setX(PLAYER_MOVEMENT_SPEED * Math::Sin(rad + Degree(360-45)));
							toRotate = Degree(360-45);
						}
						else if (keysSingleton->isKeyDown(keysSingleton->K_MOVEMENT_RIGHT))
						{
							vectorHelper.setZ(PLAYER_MOVEMENT_SPEED * Math::Cos(rad + Degree(45)));
							vectorHelper.setX(PLAYER_MOVEMENT_SPEED * Math::Sin(rad + Degree(45)));
							toRotate = Degree(45);
						}
						else
						{
							vectorHelper.setZ(PLAYER_MOVEMENT_SPEED * Math::Cos(rad));
							vectorHelper.setX(PLAYER_MOVEMENT_SPEED * Math::Sin(rad));
							toRotate = Degree(0);
						}
					}
					else if (keysSingleton->isKeyDown(keysSingleton->K_MOVEMENT_LEFT))
					{
						vectorHelper.setZ(-PLAYER_MOVEMENT_SPEED * Math::Cos(rad + Degree(90)));
						vectorHelper.setX(-PLAYER_MOVEMENT_SPEED * Math::Sin(rad + Degree(90)));
						toRotate = Degree(270);
					}
					else if (keysSingleton->isKeyDown(keysSingleton->K_MOVEMENT_RIGHT))
					{
						vectorHelper.setZ(PLAYER_MOVEMENT_SPEED * Math::Cos(rad + Degree(90)));
						vectorHelper.setX(PLAYER_MOVEMENT_SPEED * Math::Sin(rad + Degree(90)));
						toRotate = Degree(90);
					}
				}

				if (vectorHelper.length() != 0)
				{
					//Quaternion quat = Quaternion((rad + toRotate), Vector3::UNIT_Y);
					//mBodyNode->setOrientation(quat);

					mLastStandingYaw = (rad + toRotate);
					mBodyNode->setOrientation(Quaternion(mLastStandingYaw, Vector3::UNIT_Y));//, TO_OGRE(mKinematicController->getTouchingNormal())));
				//	mRigidBody->getWorldTransform().setRotation(TO_BULLET(quat));
					
					if (lastMovement < PLAYER_BEGIN_MOVEMENT_TIME)
					{
						vectorHelper *= PLAYER_BEGIN_MOVEMENT_SPEED + (lastMovement * (1 - PLAYER_BEGIN_MOVEMENT_SPEED) / PLAYER_BEGIN_MOVEMENT_TIME);//PLAYER_BEGIN_MOVEMENT_SPEED + (;
					}
					mKinematicController->setHorizontalDirection(TO_BULLET((Quaternion(mLastStandingYaw, Vector3::UNIT_Y) * Vector3::UNIT_Z).normalisedCopy() * PLAYER_MOVEMENT_SPEED));//vectorHelper);
					if (mAnimationID != PA_RUNNING)
					{
						setAnimation(PA_RUNNING, true);
					}
					isPressingMovement = true;
	
				}
				else
				{
					lastMovement = 0;
					if (
						lastTimeOnAir >= PLAYER_TIME_LENGTHY_JUMP / 2 &&
						(mAnimationID == PA_WALL_JUMP && mAnimationID == PA_FALLING_START || mAnimationID == PA_HEAP_JUMP || mAnimationID == PA_NORMAL_JUMP || mAnimationID == PA_HIGH_JUMP))
					{
						setAnimation(PA_LANDING_AFTER_JUMP, true);
					}

					if (mAnimationID != PA_LANDING_AFTER_JUMP && mAnimationID != PA_STANDING)
					{
						setAnimation(PA_STANDING, true);
					}

					if (mKinematicController->timeSinceLastJump() >= 0.5)
					{
						mKinematicController->stopWalking();
					}
					isPressingMovement = false;

					//mBodyNode->setOrientation(Quaternion(mLastStandingYaw, Vector3::UNIT_Y));
				}
			}
		}
		else
		{
			mBodyNode->setOrientation(Quaternion(mLastStandingYaw, Vector3::UNIT_Y));

			mLastVerticalVelocity = mKinematicController->getVelocity().y;
			mTimeOnAir += evt.timeSinceLastFrame;

			if (
				mAnimationID != PA_WALL_JUMP &&
				mAnimationID != PA_FALLING_START &&
				mAnimationID != PA_HEAP_JUMP &&
				mAnimationID != PA_NORMAL_JUMP &&
				mAnimationID != PA_HIGH_JUMP)
			{
				setAnimation(PA_FALLING_START, true);
			}
			
			vectorHelper = btVector3(0, 0, 0);

			//mState->setEnabled(true);
			if (keysSingleton->useDegreeMovement)
			{
				if (keysSingleton->movementVector.length() != 0)
				{
					vectorHelper = TO_BULLET(Quaternion(rad, Vector3::UNIT_Y) * keysSingleton->movementVector) * PLAYER_JUMP_SPEED;
				}
				vectorHelper.setY(0);
			}
			
			if (vectorHelper.length() == 0)
			{
				if (keysSingleton->isKeyDown(keysSingleton->K_MOVEMENT_FORWARD))
				{
					if (keysSingleton->isKeyDown(keysSingleton->K_MOVEMENT_LEFT))
					{
						vectorHelper.setZ(-PLAYER_JUMP_SPEED * Math::Cos(rad + Degree(45)));
						vectorHelper.setX(-PLAYER_JUMP_SPEED * Math::Sin(rad + Degree(45)));
						rad += Degree(180+45);
					}
					else if (keysSingleton->isKeyDown(keysSingleton->K_MOVEMENT_RIGHT))
					{
						vectorHelper.setZ(-PLAYER_JUMP_SPEED * Math::Cos(rad + Degree(360-45)));
						vectorHelper.setX(-PLAYER_JUMP_SPEED * Math::Sin(rad + Degree(360-45)));
						rad += Degree(90+45);
					}
					else
					{
						vectorHelper.setZ(PLAYER_JUMP_SPEED * Math::Cos(rad + Degree(180)));
						vectorHelper.setX(PLAYER_JUMP_SPEED * Math::Sin(rad + Degree(180)));
						rad += Degree(180);
					}
				}
				else if (keysSingleton->isKeyDown(keysSingleton->K_MOVEMENT_BACK))
				{
					if (keysSingleton->isKeyDown(keysSingleton->K_MOVEMENT_LEFT))
					{
						vectorHelper.setZ(PLAYER_JUMP_SPEED * Math::Cos(rad + Degree(360-45)));
						vectorHelper.setX(PLAYER_JUMP_SPEED * Math::Sin(rad + Degree(360-45)));
						rad += Degree(360-45);
					}
					else if (keysSingleton->isKeyDown(keysSingleton->K_MOVEMENT_RIGHT))
					{
						vectorHelper.setZ(PLAYER_JUMP_SPEED * Math::Cos(rad + Degree(45)));
						vectorHelper.setX(PLAYER_JUMP_SPEED * Math::Sin(rad + Degree(45)));
						rad += Degree(45);
					}
					else
					{
						vectorHelper.setZ(PLAYER_JUMP_SPEED * Math::Cos(rad));
						vectorHelper.setX(PLAYER_JUMP_SPEED * Math::Sin(rad));
					}
				}
				else if (keysSingleton->isKeyDown(keysSingleton->K_MOVEMENT_LEFT))
				{
					vectorHelper.setZ(-PLAYER_JUMP_SPEED * Math::Cos(rad + Degree(90)));
					vectorHelper.setX(-PLAYER_JUMP_SPEED * Math::Sin(rad + Degree(90)));
					rad += Degree(-90);
				}
				else if (keysSingleton->isKeyDown(keysSingleton->K_MOVEMENT_RIGHT))
				{
					vectorHelper.setZ(PLAYER_JUMP_SPEED * Math::Cos(rad + Degree(90)));
					vectorHelper.setX(PLAYER_JUMP_SPEED * Math::Sin(rad + Degree(90)));
					rad += Degree(90);
				}
			}

			if (vectorHelper.length() != 0 && mTimeSinceWallJump >= TIME_FOR_MOVEMENT_AFTER_WALL_JUMP)
			{
				if (!mStillLandingAfterLengthyJump)
				{
					double modifier = 1;
					switch (mLastJumpType)
					{
					case JumpType::JT_LONG:
					case JumpType::JT_WALL_JUMP_LONG:
						modifier = PLAYER_JUMP_SPEED_MODIFIER_LONG;
						break;
#if ENABLE_BACKWARDS_JUMP
					case JumpType::JT_BACKWARDS:
						modifier = PLAYER_JUMP_SPEED_MODIFIER_BACKWARDS;
						break;
#endif
					case JumpType::JT_WALL_JUMP_NORMAL:

						modifier = PLAYER_JUMP_SPEED_MODIFIER_WALL;
						break;
					case JumpType::JT_NORMAL_MOVING:
					case JumpType::JT_NORMAL_STATIC:
					case JumpType::JT_NONE:

						modifier = PLAYER_JUMP_SPEED_MODIFIER_NORMAL;
						break;
					}

					auto speed = mKinematicController->getVelocity();
					Real verticalSpeed = speed.y;
					speed.y = 0;
					btVector3 newVelocity = TO_BULLET(speed) + vectorHelper * (evt.timeSinceLastFrame) * modifier;
					
					newVelocity = TO_BULLET(trimJumpVelocity(TO_OGRE(newVelocity), mLastJumpType));

					newVelocity.setY(verticalSpeed);
					mKinematicController->setJumpSpeed(newVelocity);

//					btVector3 newVelocity = mKinematicController->getHorizontalSpeed() + vectorHelper * (evt.timeSinceLastFrame) * modifier;
//					if (newVelocity.length() > PLAYER_LONG_JUMP_SPEED_MAX)
//					{
//						newVelocity = newVelocity.normalize() * PLAYER_LONG_JUMP_SPEED_MAX;
//					}
//					switch (mLastJumpType)
//					{
//					case JumpType::JT_LONG:
//					case JumpType::JT_WALL_JUMP_LONG:
//						if (newVelocity.length() > PLAYER_LONG_JUMP_SPEED_MAX)
//						{
//							newVelocity = newVelocity.normalize() * PLAYER_LONG_JUMP_SPEED_MAX;
//						}
//						break;
//#if ENABLE_BACKWARDS_JUMP
//					case JumpType::JT_BACKWARDS:
//						if (newVelocity.length() > PLAYER_BACKWARDS_JUMP_SPEED_MAX)
//						{
//							newVelocity = newVelocity.normalize() * PLAYER_BACKWARDS_JUMP_SPEED_MAX;
//						}
//						break;
//#endif
//					case JumpType::JT_NORMAL_MOVING:
//					case JumpType::JT_NORMAL_STATIC:
//					case JumpType::JT_WALL_JUMP_NORMAL:
//					case JumpType::JT_NONE:
//						if (newVelocity.length() > PLAYER_NORMAL_JUMP_SPEED_MAX)
//						{
//							newVelocity = newVelocity.normalize() * PLAYER_NORMAL_JUMP_SPEED_MAX;
//						}
//						break;
//					}
//					mKinematicController->setHorizontalSpeed(newVelocity);
				}
			}
			else
			{
				isPressingMovement = false;
				//mKinematicController->stopWalking();
			}
		}
	}

	static bool lastTrailFlag = false;
	
	if (
		//isPressingMovement ||
		!isTouchingBelow() &&
		!mGameOver
		//(mMovablePlatformBelow &&
		//	(!mMovablePlatformBelow->getActivator() ||
		//	mMovablePlatformBelow->getActivator()->isActivated())
		//)
	)
	{
		mTimeSinceLastTrail += evt.timeSinceLastFrame;
		if (mTimeSinceLastTrail >= PLAYER_TIME_BETWEEN_COPY)
		{
			if (lastTrailFlag)
			{
				if (mCurrentTrail == 0)
				{
					mPlayerTrails[PLAYER_MAX_COPY_TRAIL - 1]->startTrail();
				}
				else
				{
					mPlayerTrails[mCurrentTrail - 1]->startTrail();
				}
			}
			lastTrailFlag = true;
			mPlayerTrails[mCurrentTrail++]->reset(
				mBodyNode->_getDerivedPosition(),
				mBodyNode->getOrientation(),
				mAnimations[mAnimationID]
			);
			mTimeSinceLastTrail = 0;
			if (mCurrentTrail == PLAYER_MAX_COPY_TRAIL) mCurrentTrail = 0;
		}
	}
	else
	{
		if (lastTrailFlag)
		{
			if (mCurrentTrail == 0)
			{
				mPlayerTrails[PLAYER_MAX_COPY_TRAIL - 1]->startTrail();
			}
			else
			{
				mPlayerTrails[mCurrentTrail - 1]->startTrail();
			}
		}
		mTimeSinceLastTrail = 0;
		lastTrailFlag = false;
	}
	
	if (
		mKinematicController->getVelocity().y > 0.0 &&
		(mLastJumpType == JumpType::JT_NORMAL_MOVING || mLastJumpType == JumpType::JT_NORMAL_STATIC) &&
		mKinematicController->timeSinceLastJump() <= PLAYER_JUMP_NORMAL_TIME_INCREASE_HEIGHT &&
		keysSingleton->isKeyDown(Keys::K_JUMP_NORMAL) &&
		mPressJumpLastUpdate[0])
	{
		mKinematicController->increaseVerticalSpeed(evt.timeSinceLastFrame * (1 - mTimeOnAir / PLAYER_JUMP_NORMAL_TIME_INCREASE_HEIGHT) * PLAYER_VERTICAL_SPEED_INCREASE_SECOND);
	}

	if (mKinematicController->isTouchingKillBox())
	{
		shouldKillPlayer = true;
	}

	bool pressedJumpButton = false;
	bool hasJumped = false;
	bool backwardsJump = false;
	//static bool backwardsJumpLastUpdate = false;

	mTimeSinceWallJump += evt.timeSinceLastFrame;

	for (int i = 0; i < mCrashEffects.size(); i++)
	{
		mCrashEffects[i]->update(evt);
		if (mCrashEffects[i]->shouldDelete)
		{
			delete mCrashEffects[i];
			mCrashEffects.erase(mCrashEffects.begin() + i);
		}
	}

	int jumpType = 0;
	btVector3 touching = mKinematicController->getTouchingNormal();

	if (!shouldBlockMovement())
	{
		if (keysSingleton->isKeyDownLastUpdate(keysSingleton->K_JUMP_NORMAL))// && !keysSingleton->isKeyDownLastUpdate(keysSingleton->K_JUMP_NORMAL))
		{
			jumpType = 0;
			pressedJumpButton = true;
			mTimeLastJump += evt.timeSinceLastFrame;
			if (
				mKinematicController->timeSinceLastJump() >= PLAYER_TIME_BETWEEN_JUMPS &&
#if ENABLE_WALL_GRAB
				!mGrabbingWall && 
#endif
#if ENABLE_BACKWARDS_JUMP
				(mTimeLastJump >= TIME_FOR_BACKWARDS_JUMP || isPressingMovement || !keysSingleton->isKeyDown(keysSingleton->K_JUMP_NORMAL)) &&
#endif
				isTouchingBelow() &&
				(!mPressJumpLastUpdate[0] || _::options.bunnyJumping) &&
#if ENABLE_BACKWARDS_JUMP
				!keysSingleton->isKeyDown(keysSingleton->K_JUMP_BACKWARD) &&
#endif
				!mStillLandingAfterLengthyJump)
			{
				mPressJumpLastUpdate[0] = true;
				btVector3 jumpVector;
				jumpVector = TO_BULLET(Quaternion(mLastStandingYaw, Vector3::UNIT_Y) * TO_OGRE(PLAYER_JUMP_SPEED_MOVING));
				mLastJumpType = JumpType::JT_NORMAL_MOVING;
				if (!isPressingMovement)
				{
					mLastJumpType = JumpType::JT_NORMAL_STATIC;
					jumpVector = PLAYER_JUMP_SPEED_STATIC;
					Vector3 helper = Quaternion(mLastStandingYaw, Vector3::UNIT_Y) * Vector3::UNIT_Z;
					mLastVectorVelocity.x = helper.x;
					mLastVectorVelocity.z = helper.z;
				}
				else
				{
					mLastVectorVelocity.x = jumpVector.x();
					mLastVectorVelocity.z = jumpVector.z();
				}
				hasJumped = true;
				mKinematicController->jump(jumpVector, mLastJumpType);
				mTimeOnGround = 0;
				setAnimation(PlayerAnimation::PA_NORMAL_JUMP, true);
				mSoundManager->playNewSound2D(SoundManager::SoundEffects::JUMP_NORMAL_GRASS);

				if (mLastJumpType == JumpType::JT_NORMAL_STATIC)
				{
					if (_::options.showJumpEffects)
					{
						mCrashEffects.push_back(new CrashEffect(getLocation(), Quaternion(mBodyNode->getOrientation().getYaw(), Vector3::UNIT_Y) * Quaternion(Degree(-90), Vector3::UNIT_X), CrashEffect::NORMAL_STATIC_JUMP));
					}
				}
				else
				{
					if (_::options.showJumpEffects)
					{
						mCrashEffects.push_back(new CrashEffect(getLocation(), Quaternion(mBodyNode->getOrientation().getYaw(), Vector3::UNIT_Y) * Quaternion(Degree(-NORMAL_JUMP_DEGREE_EFFECT), Vector3::UNIT_X), CrashEffect::NORMAL_MOVING_JUMP));
					}
				}
				//mPlayerBody->setLinearVelocity(jumpVector);
				//mPlayerBody->setAngularVelocity(btVector3(1, 1, 0));
			}
			/*mController->setJumpSpeed(100);
			mController->jump();*/
		}

		if (keysSingleton->isKeyDownLastUpdate(keysSingleton->K_JUMP_LONG))// && !keysSingleton->isKeyDownLastUpdate(keysSingleton->K_JUMP_LONG))
		{
			jumpType = 1;
			pressedJumpButton = true;
			mTimeLastJump += evt.timeSinceLastFrame;
			if (
				mKinematicController->timeSinceLastJump() >= PLAYER_TIME_BETWEEN_JUMPS &&
#if ENABLE_WALL_GRAB
				!mGrabbingWall && 
#endif
#if ENABLE_BACKWARDS_JUMP
				(mTimeLastJump >= TIME_FOR_BACKWARDS_JUMP || isPressingMovement || !keysSingleton->isKeyDown(keysSingleton->K_JUMP_LONG)) &&
				!keysSingleton->isKeyDown(keysSingleton->K_JUMP_BACKWARD) &&
#endif
				isTouchingBelow() &&
				(!mPressJumpLastUpdate[1] || _::options.bunnyJumping) &&
				!mStillLandingAfterLengthyJump)
			{
				mPressJumpLastUpdate[1] = true;
				hasJumped = true;
				mLastJumpType = JumpType::JT_LONG;
				btVector3 jumpVector = TO_BULLET(Quaternion(mBodyNode->_getDerivedOrientation().getYaw(), Vector3::UNIT_Y) * TO_OGRE(PLAYER_HIGH_JUMP_SPEED));
				mLastVectorVelocity.x = jumpVector.x();
				mLastVectorVelocity.z = jumpVector.z();
				mKinematicController->jump(jumpVector, JumpType::JT_LONG);
				mTimeOnGround = 0.0;
				setAnimation(PlayerAnimation::PA_HEAP_JUMP, true);
				mSoundManager->playNewSound2D(SoundManager::SoundEffects::JUMP_LONG_GRASS);
			
				if (_::options.showJumpEffects)
				{
					mCrashEffects.push_back(new CrashEffect(getLocation() + Vector3(0, PLAYER_BODY_Y * 0.6666, 0), Quaternion(mBodyNode->_getDerivedOrientation().getYaw(), Vector3::UNIT_Y) * Quaternion(Degree(-LONG_JUMP_DEGREE_EFFECT), Vector3::UNIT_X), CrashEffect::LONG_JUMP));
				}
			}
			/*else if (keysSingleton->isKeyDown(keysSingleton->K_JUMP_NORMAL))
			{
				jumpType = 2;
				backwardsJump = true;
			}*/
			/*mController->setJumpSpeed(100);
			mController->jump();*/
		}

	#if ENABLE_BACKWARDS_JUMP
		if (keysSingleton->isKeyDown(keysSingleton->K_JUMP_BACKWARD))// && !keysSingleton->isKeyDownLastUpdate(keysSingleton->K_JUMP_LONG))
		{
			jumpType = 2;
			pressedJumpButton = true;
			mTimeLastJump += evt.timeSinceLastFrame;
			if (
				!mGrabbingWall &&
				isTouchingBelow() &&
				(!mPressJumpLastUpdate[2] || _::options.bunnyJumping) &&
				!mStillLandingAfterLengthyJump)
			{
		//if (
		//	!shouldBlockMovement() &&
		//	((keysSingleton->isKeyDown(Keys::K_JUMP_BACKWARD) ||
		//	backwardsJump) && (/*(backwardsJump && !backwardsJumpLastUpdate) || (!keysSingleton->isKeyDownLastUpdate(Keys::K_JUMP_BACKWARD) && keysSingleton->isKeyDown(Keys::K_JUMP_BACKWARD)) &&*/
		//	(!keysSingleton->isKeyDownLastUpdate(keysSingleton->K_JUMP_LONG) && !keysSingleton->isKeyDownLastUpdate(keysSingleton->K_JUMP_NORMAL)))) &&
		//	(!mPressJumpLastUpdate || _::options.bunnyJumping))
		//{
				jumpType = 2;
				mPressJumpLastUpdate[2] = true;
				hasJumped = true;
				mLastJumpType = JumpType::JT_BACKWARDS;
				btVector3 jumpVector = TO_BULLET(mBodyNode->_getDerivedOrientation() * TO_OGRE(PLAYER_JUMP_SPEED_BACKWARD));
				mLastVectorVelocity.x = jumpVector.x();
				mLastVectorVelocity.z = jumpVector.z();
				mKinematicController->initJump(jumpVector, false);
				setAnimation(PlayerAnimation::PA_HIGH_JUMP, true);
				//mPlayerBody->setLinearVelocity(jumpVector);
				mCrashEffects.push_back(new CrashEffect(getLocation(), Quaternion(mBodyNode->getOrientation().getYaw(), Vector3::UNIT_Y) * Quaternion(Degree(-HIGH_JUMP_DEGREE_EFFECT), Vector3::UNIT_X), CrashEffect::HIGH_JUMP));

				mPressJumpLastUpdate[0] = true; mPressJumpLastUpdate[1] = true;
			}
			pressedJumpButton = true;
		}
	#endif
		
		if (
			//!shouldBlockMovement() &&
			(!hasJumped &&
			mKinematicController->timeSinceLastJump() >= PLAYER_TIME_BETWEEN_JUMPS &&
			pressedJumpButton &&
			(!mPressJumpLastUpdate[jumpType] || _::options.bunnyJumping) &&
			mTimeSinceWallJump >= TIME_FOR_WALL_JUMP_AGAIN
#if ENABLE_WALL_GRAB
			&& !mStartedClimbingUpLedge
#endif
			)
		)// && (mPlayerBody->getLinearVelocity().x() != 0 || mPlayerBody->getLinearVelocity().z() != 0))
		{
			btVector3 newTouching = touching;
			newTouching.setY(0);
			newTouching.normalize();
			double angle = Math::Abs(mLastVectorVelocity.angleBetween(TO_OGRE(newTouching)).valueDegrees());

			if (
				(touching.length() > 0 &&
				(angle < 90 - PLAYER_WALL_JUMP_MAX_RANGE || angle > 90 + PLAYER_WALL_JUMP_MAX_RANGE) &&
				!isTouchingBelow() &&
				normalValidWallJump(touching) &&
				touching.y() < 0.99) //!normalIsStandableSlope(touching))
#if ENABLE_WALL_GRAB
				|| mGrabbingWall
#endif
			)
			{
				//2*(V dot N)*N - V
				// restart
#if ENABLE_WALL_GRAB
				mTimeSinceWallJump = 0;
				mTimeSinceLastWallGrab = 0;
#endif

				// fix
#if ENABLE_USE_LAST_HORIZONTAL_HELPER_WALL_JUMP
				Vector3 lastHorizontal = TO_OGRE(mKinematicController->getLastHorizontalHelper());
				lastHorizontal.x *= -1;
				lastHorizontal.z *= -1;
				lastHorizontal.y = 0;
				lastHorizontal.normalise();
#endif

				mLastVectorVelocity.x *= -1;
				mLastVectorVelocity.z *= -1;
				mLastVectorVelocity.y = 0;
				mLastVectorVelocity.normalise();
				touching = touching.normalize();

				Vector3 helperVector = Vector3::ZERO;
#if ENABLE_WALL_GRAB
				if (mGrabbingWall)
				{
					int cameraAngle = fixAngleNumber((int)mCameraYaw->_getDerivedOrientation().getYaw().valueAngleUnits());
					int bodyAngle = fixAngleNumber((int)mBodyNode->_getDerivedOrientation().getYaw().valueAngleUnits());
				
					angle = cameraAngle - bodyAngle;
					angle = ((int)angle + 180) % 360 - 180;
				
					if (Math::Abs(angle) < 90 - PLAYER_WALL_JUMP_MAX_RANGE)
					{
						helperVector = Quaternion(mCameraYaw->_getDerivedOrientation().getYaw(), Vector3::UNIT_Y) * Vector3::UNIT_Z;
					}
					else
					{
						helperVector = Quaternion(mBodyNode->getOrientation().getYaw(), Vector3::UNIT_Y) * Vector3::UNIT_Z;
					}
					helperVector.y = 0;
					helperVector *= -1;
					helperVector.normalise();
				}
#endif				
				Quaternion quat;
#if ENABLE_WALL_GRAB
				if (mGrabbingWall)
				{
					quat = Quaternion(Math::ATan2(helperVector.x, helperVector.z), Vector3::UNIT_Y);
				}
				else
#endif
				{
					quat = Quaternion(Math::ATan2(touching.x(), touching.z()), Vector3::UNIT_Y);
					if (angle < 90 - PLAYER_WALL_JUMP_MAX_RANGE)
					{
						helperVector = mLastVectorVelocity * -1;
					}
					else
					{
#if ENABLE_USE_LAST_HORIZONTAL_HELPER_WALL_JUMP
						if (lastHorizontal.length() == 0)
						{
							helperVector = TO_OGRE(touching);
						}
						else
						{
							helperVector = reflectVector3(lastHorizontal, TO_OGRE(touching));//2 * (mLastVectorVelocity.dotProduct(TO_OGRE(touching))) * TO_OGRE(touching) - mLastVectorVelocity;
						}
#else
						if (mLastVectorVelocity.length() == 0)// || mLastJumpType == JT_NORMAL_STATIC)
						{
							helperVector = TO_OGRE(touching);
						}
						else
						{
							helperVector = reflectVector3(mLastVectorVelocity, TO_OGRE(touching));//2 * (mLastVectorVelocity.dotProduct(TO_OGRE(touching))) * TO_OGRE(touching) - mLastVectorVelocity;
						}
#endif
					}

				}

				if (_::options.showJumpEffects)
				{
					mCrashEffects.push_back(new CrashEffect(getLocation(), quat, CrashEffect::WALL_JUMP));
				}


#if ENABLE_WALL_GRAB
				mGrabbingWall = false;
#endif
				//mLastVectorVelocity = Vector3::ZERO;//(0, 0, 0);

				touching.setZero();// = btVector3(0, 0, 0);

				if (jumpType == 0)
				{
					touching.setX(helperVector.x * PLAYER_WALL_JUMP_SPEED.z());
					touching.setY(PLAYER_WALL_JUMP_SPEED.y());
					touching.setZ(helperVector.z * PLAYER_WALL_JUMP_SPEED.z());
					mLastJumpType = JumpType::JT_WALL_JUMP_NORMAL;
				}
				else if (jumpType == 1)
				{
					touching.setX(helperVector.x * PLAYER_WALL_HIGH_JUMP_SPEED.z());
					touching.setY(PLAYER_WALL_HIGH_JUMP_SPEED.y());
					touching.setZ(helperVector.z * PLAYER_WALL_HIGH_JUMP_SPEED.z());
					mLastJumpType = JumpType::JT_WALL_JUMP_LONG;
				}
	#if ENABLE_BACKWARDS_JUMP
				else
				{
					touching.setX(helperVector.x * PLAYER_WALL_LONG_JUMP_SPEED.z());
					touching.setY(PLAYER_WALL_LONG_JUMP_SPEED.y());
					touching.setZ(helperVector.z * PLAYER_WALL_LONG_JUMP_SPEED.z());
					mLastJumpType = JumpType::JT_WALL_JUMP_FAST;
				}
	#endif

				mLastVectorVelocity = TO_OGRE(touching);
				mLastVectorVelocity.normalise();

				mKinematicController->jump(touching, mLastJumpType);
				mTimeOnGround = 0.0;
				setAnimation(PlayerAnimation::PA_WALL_JUMP, true);
				mSoundManager->playNewSound3D(SoundManager::WALL_JUMP_WOOD, mBodyNode->_getDerivedPosition());
				touching.setY(0);

				quat = Quaternion(Math::ATan2(touching.x(), touching.z()), Vector3::UNIT_Y);
				//mBodyNode->setOrientation(quat);
				mLastStandingYaw = Math::ATan2(touching.x(), touching.z());

				mTimeOnAir = 0;

				mPressJumpLastUpdate[jumpType] = true;
			}
		}

		if (!keysSingleton->isKeyDown(Keys::K_JUMP_NORMAL) && !keysSingleton->isKeyDown(Keys::K_JUMP_LONG))
		{
			mTimeLastJump = 0;
		}

		int counter = 0;
		if (!keysSingleton->isKeyDown(Keys::K_JUMP_NORMAL))
		{
			counter++;
			mPressJumpLastUpdate[0] = false;
		}
		if (!keysSingleton->isKeyDown(Keys::K_JUMP_LONG))
		{
			counter++;
			mPressJumpLastUpdate[1] = false;
		}
	#if ENABLE_BACKWARDS_JUMP
		if (!keysSingleton->isKeyDown(Keys::K_JUMP_BACKWARD))
		{
			counter++;
			mPressJumpLastUpdate[2] = false;
		}
	#endif

	#if ENABLE_BACKWARDS_JUMP
		if (counter == 3)
	#else
		if (counter == 2)
	#endif
		{
			mTimeLastJump = 0;
		}
#if ENABLE_WALL_GRAB
		if (!mGrabbingWall && mGrabbingPointer)
		{
			mGrabbingPointer = 0;
		}
#endif
	}
	


#if ENABLE_WALL_GRAB

	mTimeSinceLastWallGrab += evt.timeSinceLastFrame;
	if (!mGrabbingWall && mTimeSinceLastWallGrab >= PLAYER_WALL_GRAB_TIME_FOR_ANOTHER_ONE)
	{
		mGrabbingPointer = Physics::getSingleton()->isTouchingForWallGrab(getTopPositionForWallGrab(), getOrientationForWallGrab());
		if ((keysSingleton->isKeyDown(Keys::K_JUMP_NORMAL) || keysSingleton->isKeyDown(Keys::K_JUMP_LONG)))
		{
			mGrabbingPointer = 0;
		}
		//if (mGrabbingPointer && (mGrabbingPointer->getBox()->getBoxType() == WorldBox::BT_KILL_BOX_TOP || mGrabbingPointer->getBox()->getBoxType() == WorldBox::BT_KILL_BOX)) mGrabbingPointer = 0;
	}

	if (
		!mGrabbingWall &&
		mGrabbingPointer &&
		mKinematicController->isFalling() &&
		!shouldBlockMovement()
		)
	{
		mSoundManager->playNewSound2D(SoundManager::ON_GRAB_WALL);
		mGrabbingWall = true;
		double y = mGrabbingPointer->getBox()->getYmax();
		mRigidBody->getWorldTransform().getOrigin().setY(y - PLAYER_BODY_Y);
		//btScalar orientation = btScalar(mBodyNode->getOrientation().getYaw().valueDegrees());
		//if (orientation < 45 && orientation >= -45)
		//{
		//	orientation = 0;
		//}
		//else if (orientation >= 45 && orientation <= 90 + 45)
		//{
		//	orientation = 90;
		//}
		//else if (Math::Abs(orientation) >= 90 + 45)
		//{
		//	orientation = 180;
		//}
		//else if (orientation < -45 && orientation >= -(90 + 45))
		//{
		//	orientation = -90;
		//}

		mBodyNode->setOrientation(mGrabbingPointer->orientationGrabbingPointer(getTopPositionForWallGrab(), getOrientationForWallGrab()));

		setAnimation(PA_GRABBING_WALL);
	}
#endif

	
	if (keysSingleton->isKeyDown(keysSingleton->K_EXTRA) && !keysSingleton->isKeyDownLastUpdate(keysSingleton->K_EXTRA))
	{
		/*if (mCamera->getPolygonMode() == PolygonMode::PM_SOLID)
		{
			mCamera->setPolygonMode(PolygonMode::PM_WIREFRAME);
		}
		else
		{
			mCamera->setPolygonMode(PolygonMode::PM_SOLID);
		}*/
		//_::options.changeSkin("cian-neon");
	}
	if (keysSingleton->isKeyDown(keysSingleton->K_EXTRA2))
	{
		//_::options.changeSkin("sapphire");
	}
	
	if (mAnimationID == PA_RUNNING)
	{
		int percentage = Math::ICeil(mAnimations[PA_RUNNING]->getTimePosition() / mAnimations[PA_RUNNING]->getLength() * 100);

		if ((percentage <= 50 && mRunningStep == 2) || mRunningStep > 2)
		{
			mRunningStep = 0;
		}

		static bool changeType = false;
		if (percentage > mRunningStep * 50)
		{
			int offset = 0;
			if (changeType)
			{
				offset = 2;
			}
			
			mSoundManager->playNewSound2D((SoundManager::SoundEffects)(SoundManager::STEP_GRASS_1 + offset + mRunningStep));//, mMainNode->_getDerivedPosition());

			if (mRunningStep == 2)
			{
				changeType = !changeType;
			}
			//mSoundManager->playNewSound2D((SoundManager::SoundEffects)(SoundManager::STEP_MOSAIC_1 - 1 + mRunningStep));//, mMainNode->_getDerivedPosition());
			mRunningStep++;
		}
	}
	else
	{
		mRunningStep = 0;
	}

	for (int i = 0; i < mKinematicController->getTouchingPlatform().size(); i++)
	{
		Platform* plat = mKinematicController->getTouchingPlatform()[i].first;
		if (plat && mKinematicController->onGround() && plat->isBreakable() && !plat->startedBreaking())
		{
			plat->startBreak();
		}
#if ENABLE_WALL_GRAB
		else if (mGrabbingPointer && mGrabbingPointer->isBreakable() && !mGrabbingPointer->startedBreaking())
		{
			mGrabbingPointer->startBreak();
		}
#endif
	}
	
	Monster* monster = mKinematicController->getTouchingMonster();
	if (monster)
	{
		// process monster collision
		//switch (monster->getMonsterType())
		//{
		//case Monster::MT_BLACK_KING:
		//	BlackKing* king = static_cast<BlackKing*>(monster);
		//	if (king->isBodyWithThunder())
		//	{
		//		shouldKillPlayer = true;
		//	}
		//	break;
		//}
	}

// PH	mKinematicController->shouldBlockMovement(shouldBlockMovement());
	
	mKinematicController->updateAction(Physics::getSingleton()->getWorld(), evt.timeSinceLastFrame);
	updatePlatformBelow(evt);


	//ph mKinematicController->endUpdate();
	//mBodyNode->setOrientation(Quaternion(mCameraHolder->_getDerivedOrientation().getYaw(), TO_OGRE(mKinematicController->getTouchingNormal())));

	Vector4 deathStateArgs = Vector4::ZERO;
	if (mGameOver)
	{
		deathStateArgs.x = 1;
		deathStateArgs.y = mSecondsSinceDie;
	}
	else
	{
		deathStateArgs.x = 0;
	}
	
	mPlayerEntity->getSubEntity(0)->setCustomParameter(0, deathStateArgs);
	
	if (!mGameOver)
	{
		mMainNode->setPosition(TO_OGRE(mRigidBody->getWorldTransform().getOrigin()));
		//auto transf = mRigidBody->getWorldTransform();
		//transf.setOrigin(TO_BULLET(mBodyNode->_getDerivedPosition()));
		//mRigidBody->setWorldTransform(transf);
		//mMainNode->setPosition(TO_OGRE(mRigidBody->getWorldTransform().getOrigin()));
	}
	Vector3 derivedPos = mBodyNode->_getDerivedPosition();
	Platform* belowShadowPlatform = Physics::getSingleton()->raycastPlatform(TO_BULLET(derivedPos) - btVector3(0, PLAYER_BODY_Y / 2, 0), TO_BULLET(derivedPos) - btVector3(0, 50000, 0));
	double crashed = 0.0;
	if (belowShadowPlatform && !mGameOver)//&& mSecondsSinceDie <= 2)
	{
		crashed = 1.0;
		derivedPos.y = (belowShadowPlatform->getBox()->getBody()->getWorldTransform().getOrigin().y() + belowShadowPlatform->getBox()->size().y / 2);
	}
	
	mGenerator->playerPlatformShaderArgs(Vector4(derivedPos.x, derivedPos.y, derivedPos.z, crashed), mBodyNode->_getDerivedPosition());
	

	//Ogre::SubMesh* submesh = mCapeEntity->getMesh()->getSubMesh(0);
 //   Ogre::VertexData* vertex_data = submesh->useSharedVertices ? mCapeEntity->getMesh()->sharedVertexData : submesh->vertexData;

 //   if (!submesh->useSharedVertices)
 //   {
 //       const Ogre::VertexElement* posElem =vertex_data->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION);
 //       Ogre::HardwareVertexBufferSharedPtr vbuf= vertex_data->vertexBufferBinding->getBuffer(posElem->getSource());
 //       const unsigned int vSize = (unsigned int)vbuf->getVertexSize();
 //       unsigned char* vertex =static_cast<unsigned char*>(vbuf->lock(Ogre::HardwareBuffer::HBL_DISCARD));

 //       float* pReal;

 //       //get the vertex data from bullet.
	//	btSoftBody::tNodeArray&   nodes(mCapeSofyBody->m_nodes);

 //       //now we simply update the coords of the vertices. Of course this only works
 //       //properly when the Ogre data and the Bullet data have the same number of vertices
 //       //and when they are in the same order. It seems that bullet doesn't shuffle the
 //       //vertices, but this is only an observation!
 //       for (int j=0;j<nodes.size();++j)
 //       {
 //           posElem->baseVertexPointerToElement(vertex, &pReal);
 //           vertex += vSize;
 //           *pReal++= nodes[j].m_x.x();
 //           *pReal++= nodes[j].m_x.y();
 //           *pReal++= nodes[j].m_x.z();
 //       }
 //       vbuf->unlock();
 //   }
}

void Player::onMovablePlatformMovement(MovablePlatform* plat, const FrameEvent& evt)
{
	btTransform transf = mRigidBody->getWorldTransform();
	if (plat->isFlagActive(MovablePlatform::MP_ROTATING))
	{
		Degree speed = plat->getRotateSpeed();
		Vector3 helper = TO_OGRE(transf.getOrigin()), helper2 = plat->getBox()->getNode()->_getDerivedPosition();
		helper.y = 0; helper2.y = 0;
		helper -= helper2;
		helper2 = Quaternion(speed * evt.timeSinceLastFrame, Vector3::UNIT_Y) * helper;
		mBodyNode->yaw(speed * evt.timeSinceLastFrame);
		mLastStandingYaw = mBodyNode->_getDerivedOrientation().getYaw();
		transf.setOrigin(transf.getOrigin() + TO_BULLET(helper2 - helper));
	}
	if (plat->isFlagActive(MovablePlatform::MP_LINEAR))
	{
		btVector3 origin = transf.getOrigin();
		transf.setOrigin(origin + plat->getLastLinearTranslate());
	}
	if (plat->isFlagActive(MovablePlatform::MP_SCALING))
	{
		// not implemented
	}
	mRigidBody->setWorldTransform(transf);
}

void Player::updatePlatformBelow(const FrameEvent& evt)
{
	Platform* plat = getStandingPlatform();
	if (plat && plat->Type() == PlatformType::PT_MOVABLE)
	{
		mMovablePlatformBelow = static_cast<MovablePlatform*>(plat);
		if (mKinematicController->onGround())
		{
			mMovablePlatformBelow->onTopHelper();
		}

		onMovablePlatformMovement(mMovablePlatformBelow, evt);
		//ph mKinematicController->standingInMovable = true;
	}
	else
	{
		mMovablePlatformBelow = 0;
	}

	/*if (plat && plat->getBox()->getBoxType() == WorldBox::BT_KILL_BOX_TOP)
	{
		shouldKillPlayer = true;
	}*/
#if ENABLE_WALL_GRAB
	if (mMovablePlatformBelow == 0 && mGrabbingPointer && mGrabbingPointer->Type() == PlatformType::PT_MOVABLE)
	{
		mMovablePlatformBelow = (MovablePlatform*)mGrabbingPointer;
	}
#endif
}

void Player::resetCamera()
{
	mCameraYaw->setOrientation(Quaternion(mBodyNode->_getDerivedOrientation().getYaw() - Degree(180), Vector3::UNIT_Y));
	mCurrentZoom = ZM_2;
	onCameraChange(mCurrentZoom);
}

void Player::killCharacter()
{
	if (mParticleHolder)
	{
		Playground::destroySceneNode(mParticleHolder);
		mParticleHolder = 0;
	}
	mParticleHolder = mSceneManager->getRootSceneNode()->createChildSceneNode(mBodyNode->_getDerivedPosition());
	ParticleSystem* particleSystem = mSceneManager->createParticleSystem(requestNewParticleName(), "Particles/CharacterDying");
	mParticleHolder->attachObject(particleSystem);

	mSecondsSinceDie = 0;//PLAYER_RESPAWN_TIME;
	mGameOver = true;
	//mBodyNode->setVisible(false);
	setAnimation(PA_DYING, true);

	mDeathBillboardSet->setVisible(false);
}

void Player::respawnCharacter()
{
	mDeathBillboardSet->setVisible(false);
	if (mParticleHolder)
	{
		Playground::destroySceneNode(mParticleHolder);
		mParticleHolder = 0;
	}
	//ph mKinematicController->touchingKillBox = false;
	shouldKillPlayer = false;
	mGameOver = false;
	//mBodyNode->setOrientation(Quaternion(Radian(0), Vector3::UNIT_Y));
	btTransform trans;
	trans.setIdentity();
	trans.setOrigin(TO_BULLET(mCurrentRespawnPoint));
	//trans.setOrigin(btVector3(-48.59, 1500, 195.64));
	//trans.setOrigin(btVector3(150.0, 90.0, 90.0));
	mMainNode->setPosition(mCurrentRespawnPoint);
	mRigidBody->setWorldTransform(trans);
// PH	mKinematicController->onRespawn();
	mStillLandingAfterLengthyJump = false;
	mTimeSinceLengthyJump = 0;
	mTimeOnAir = 0;
#if ENABLE_WALL_GRAB
	mGrabbingWall = false;
	mGrabbingPointer = 0;
#endif
	mRigidBody->setLinearVelocity(btVector3(0, 0, 0));
	mKinematicController->onRespawn();
}

#if ENABLE_WALL_GRAB
void Player::updateClimbUpAnimation(Real elapsedTime)
{
	const int yTranslate = 2.109866 * 2;
	const int xTranslate = 1.043234 * 2;

	mTimeSinceClimbingUpLedge += elapsedTime;
	if (mTimeSinceClimbingUpLedge >= mTimeForClimbingUpLedge)
	{
		mStartedClimbingUpLedge = false;
		mGrabbingWall = false;
		mTimeSinceLastWallGrab = 0;
		btVector3 translate = mRigidBody->getWorldTransform().getOrigin();
		translate += TO_BULLET((mBodyNode->getOrientation()) * Vector3(0, yTranslate, xTranslate));
		btTransform transf = mRigidBody->getWorldTransform();//.setOrigin(translate);
		transf.setOrigin(translate);
		mRigidBody->setWorldTransform(transf);
		setAnimation(PlayerAnimation::PA_STANDING, true);
		elapsedTime -= mTimeSinceClimbingUpLedge - mTimeForClimbingUpLedge;
	}

	Quaternion quat = mCameraYaw->getOrientation();
	//Vector3 helper = mBodyNode->getOrientation() * Vector3(xTranslate * elapsedTime, yTranslate * elapsedTime, 0);
	//mCameraHolder->translate(helper);

	if (mTimeSinceClimbingUpLedge >= mTimeForClimbingUpLedge)
	{
		//Vector3 helper = mBodyNode->getOrientation() * Vector3(-xTranslate, -yTranslate, 0);
		//mCameraHolder->translate(helper);
	}
}
#endif

void Player::onCameraChange(ZoomLevel level)
{
	btScalar zLength = CAMERA_ZOOM_DEFAULT + level * CAMERA_ZOOM_DISTANCE;
	
	// TODO remove this, debug. fix camera
	mCameraHolder->setPosition(Vector3(0, 0, zLength));
	return;
	// end TODO
	
	auto normal = (mCameraHolder->_getDerivedPosition() - mBodyNode->_getDerivedPosition()).normalisedCopy();
	btScalar queryLength = Physics::getSingleton()->raycastCameraMap(mBodyNode->_getDerivedPosition(), normal, zLength);

	if (queryLength != -1 && queryLength < zLength)
	{
		zLength = queryLength;
	}

	mCameraHolder->setPosition(Vector3(0, 0, zLength));
	
	//static auto query = mSceneManager->createRayQuery(Ogre::Ray(), Ogre::SceneManager::WORLD_GEOMETRY_TYPE_MASK);
	//if (query) query->setSortByDistance(true);
	//if (query)
	//{
	//	auto normal = (mCameraHolder->_getDerivedPosition() - mMainNode->_getDerivedPosition()).normalisedCopy();
	//	auto ray = Ray(mMainNode->_getDerivedPosition() + normal * CAMERA_ZOOM_DEFAULT, normal);
	//	query->setRay(ray);

	//	auto result = query->execute();
	//	if (result.size() == 0)
	//	{
	//		mCameraHolder->setPosition(Vector3(0, 0, zLength));
	//	}
	//	else
	//	{
	//		Ogre::Real closest_distance = -1.0f;
	//		Ogre::Vector3 closest_result;
	//		Ogre::RaySceneQueryResult &query_result = query->getLastResults();
	//		for (size_t qr_idx = 0; qr_idx < query_result.size(); qr_idx++)
	//		{
	//			// stop checking if we have found a raycast hit that is closer
	//			// than all remaining entities
	//			if ((closest_distance >= 0.0f) &&
	//				(closest_distance < query_result[qr_idx].distance))
	//			{
	//					break;
	//			}
 //
	//			// only check this result if its a hit against an entity
	//			if ((query_result[qr_idx].movable != NULL) &&
	//				(query_result[qr_idx].movable->getMovableType().compare("Entity") == 0))
	//			{
	//				// get the entity to check
	//				Ogre::Entity *pentity = static_cast<Ogre::Entity*>(query_result[qr_idx].movable);           
 //
	//				// mesh data to retrieve         
	//				size_t vertex_count;
	//				size_t index_count;
	//				Ogre::Vector3 *vertices;
	//				uint32 *indices;
 //
	//				// get the mesh information
	//				GetMeshInformation(
	//					pentity,
	//					vertex_count,
	//					vertices,
	//					index_count,
	//					indices,             
	//					pentity->getParentNode()->_getDerivedPosition(),
	//					pentity->getParentNode()->_getDerivedOrientation(),
	//					pentity->getParentNode()->_getDerivedScale()
	//				);
 //
	//				// test for hitting individual triangles on the mesh
	//				bool new_closest_found = false;
	//				for (int i = 0; i < static_cast<int>(index_count); i += 3)
	//				{
	//					// check for a hit against this triangle
	//					std::pair<bool, Ogre::Real> hit = Ogre::Math::intersects(ray, vertices[indices[i]],
	//						vertices[indices[i+1]], vertices[indices[i+2]], true, false);
 //
	//					// if it was a hit check if its the closest
	//					if (hit.first)
	//					{
	//						if ((closest_distance < 0.0f) ||
	//							(hit.second < closest_distance))
	//						{
	//							// this is the closest so far, save it off
	//							closest_distance = hit.second;
	//							new_closest_found = true;
	//						}
	//					}
	//				}
 //
	//				// free the verticies and indicies memory
	//				delete[] vertices;
	//				delete[] indices;
 //
	//				// if we found a new closest raycast for this object, update the
	//				// closest_result before moving on to the next object.
	//				if (new_closest_found)
	//				{
	//					closest_result = ray.getPoint(closest_distance);               
	//				}
	//			}       
	//		}

	//		if (closest_distance > 0 && closest_distance < zLength)
	//		{
	//			zLength = closest_distance;
	//		}
	//	}
	//}
	
}

void Player::setupAnimations()
{
	mPlayerEntity->getSkeleton()->setBlendMode(SkeletonAnimationBlendMode::ANIMBLEND_AVERAGE);
	
	String animNames[] = {
		"Standing",
		"Falling",
		"Running",
		"HeapJump",
		"HighJump",
		"NormalJump",
		"WallJump",
		"GrabbingWall",
		"GoUpWall",
		"LandingAfterJump",
		"Dying",
		"Celebrate",
		"ActionSpin"
	};
	
#if ENABLE_ANIMATION_BLENDING
	memset(mFadingIn, false, sizeof(mFadingIn));
	memset(mFadingOut, false, sizeof(mFadingOut));
#endif

	for (int i = 0; i < PA_NUM_ANIMS; i++)
	{
		mAnimations[i] = mPlayerEntity->getAnimationState(animNames[i]);
		mAnimations[i]->setLoop(true);
		mAnimations[i]->setEnabled(false);
		
#if ENABLE_ANIMATION_BLENDING
		mFadingIn[i] = false;
		mFadingOut[i] = false;
#endif
	}
	
	mAnimations[PA_FALLING_START]->setLoop(false);
	mAnimations[PA_HEAP_JUMP]->setLoop(false);
	mAnimations[PA_NORMAL_JUMP]->setLoop(false);
	mAnimations[PA_HIGH_JUMP]->setLoop(false);
	mAnimations[PA_WALL_JUMP]->setLoop(false);
	mAnimations[PA_GO_UP_WALL]->setLoop(false);
	mAnimations[PA_LANDING_AFTER_JUMP]->setLoop(false);
	mAnimations[PA_DYING]->setLoop(false);
	mAnimations[PA_CELEBRATE]->setLoop(false);

#if ENABLE_WALL_GRAB
	mTimeForClimbingUpLedge = mAnimations[PA_GO_UP_WALL]->getLength() / 2;
#endif
	mTimeAfterLengthyJump = mAnimations[PA_LANDING_AFTER_JUMP]->getLength();
	
	setAnimation(PA_STANDING, true);
}

void Player::setAnimation(PlayerAnimation animation, bool reset)
{
	//if (mTopAnimID >= 0 && mTopAnimID < NUM_ANIMS)
	//{
	//        // if we have an old animation, fade it out
	//        mFadingIn[mTopAnimID] = false;
	//        mFadingOut[mTopAnimID] = true;
	//}

	//mTopAnimID = id;

	//if (id != ANIM_NONE)
	//{
	//        // if we have a new animation, enable it and fade it in
	//        mAnims[id]->setEnabled(true);
	//        mAnims[id]->setWeight(0);
	//        mFadingOut[id] = false;
	//        mFadingIn[id] = true;
	//        if (reset) mAnims[id]->setTimePosition(0);
	//}
	if (mAnimationID >= 0 && mAnimationID < PA_NUM_ANIMS && animation != mAnimationID)
	{
#if ENABLE_ANIMATION_BLENDING
		mFadingIn[mAnimationID] = false;
		mFadingOut[mAnimationID] = true;
#endif
		mAnimations[mAnimationID]->setEnabled(false);
	}
	
	mAnimationID = animation;
	mAnimations[mAnimationID]->setEnabled(true);
#if ENABLE_ANIMATION_BLENDING
	mAnimations[mAnimationID]->setWeight(0.0);
	mFadingOut[mAnimationID] = false;
	mFadingIn[mAnimationID] = true;
#endif
	if (reset) mAnimations[mAnimationID]->setTimePosition(0);
}

void Player::updateAnimations(Real deltaTime)
{
	if (mAnimationID >= 0 && mAnimationID < PA_NUM_ANIMS)
	{
		if (mAnimationID == PA_GO_UP_WALL) deltaTime *= 2;
		mAnimations[mAnimationID]->addTime(deltaTime);
		if (mAnimations[mAnimationID]->getLoop() && mAnimations[mAnimationID]->getTimePosition() >= mAnimations[mAnimationID]->getLength())
		{
			mAnimations[mAnimationID]->setTimePosition(0);
		}
#if ENABLE_ANIMATION_BLENDING
		fadeAnimations(deltaTime);
#endif
	}
}

#if ENABLE_ANIMATION_BLENDING
void Player::fadeAnimations(Real deltaTime)
{
	for (int i = 0; i < PA_NUM_ANIMS; i++)
	{
		if (mFadingIn[i])
		{
			Real newWeight = mAnimations[i]->getWeight() + deltaTime * PLAYER_ANIM_FADE_SPEED;
			mAnimations[i]->setWeight(Math::Clamp<Real>(newWeight, 0, 1));
			if (newWeight >= 1) mFadingIn[i] = false;
		}
		else if (mFadingOut[i])
		{
			Real newWeight = mAnimations[i]->getWeight() - deltaTime * PLAYER_ANIM_FADE_SPEED;
			mAnimations[i]->setWeight(Math::Clamp<Real>(newWeight, 0, 1));
			if (newWeight <= 0)
			{
				mAnimations[i]->setEnabled(false);
				mFadingOut[i] = false;
			}
		}
	}
}
#endif

void Player::onGoalReaching()
{
	setAnimation(PA_CELEBRATE, true);
	mReachedGoal = true;
}

Vector3 Player::trimJumpVelocity(Vector3 velocity, int jumpType)
{
	auto yHelp = velocity.y;
	velocity.y = 0;
	switch (jumpType)
	{
	case JumpType::JT_LONG:
	case JumpType::JT_WALL_JUMP_LONG:
		if (velocity.length() > PLAYER_LONG_JUMP_SPEED_MAX)
		{
			velocity.normalise();
			velocity = velocity * PLAYER_LONG_JUMP_SPEED_MAX;
		}
		velocity.y = yHelp;
		break;
	#if ENABLE_BACKWARDS_JUMP
	case JumpType::JT_BACKWARDS:
		if (velocity.length() > PLAYER_BACKWARDS_JUMP_SPEED_MAX)
		{
			velocity.normalise();
			velocity = velocity * PLAYER_BACKWARDS_JUMP_SPEED_MAX;
		}
		velocity.y = yHelp;
		break;
	#endif
	case JumpType::JT_NORMAL_MOVING:
	case JumpType::JT_NORMAL_STATIC:
	case JumpType::JT_WALL_JUMP_NORMAL:
	case JumpType::JT_NONE:
		if (velocity.length() > PLAYER_NORMAL_JUMP_SPEED_MAX)
		{
			velocity.normalise();
			velocity = velocity * PLAYER_NORMAL_JUMP_SPEED_MAX;
		}
		velocity.y = yHelp;
		break;
	}

	return velocity;
}

Platform* Player::getStandingPlatform()
{
	btAlignedObjectArray<btVector3> toReturn;
	btDiscreteDynamicsWorld* world = Physics::getSingleton()->getWorld();

	int numberManifolds = world->getDispatcher()->getNumManifolds();
	for (int i = 0; i < numberManifolds; i++)
	{
		btPersistentManifold* contactManifold = world->getDispatcher()->getManifoldByIndexInternal(i);
		btCollisionObject* obA = const_cast<btCollisionObject*>(contactManifold->getBody0());
		btCollisionObject* obB = const_cast<btCollisionObject*>(contactManifold->getBody1());
		if (obA->platformPointer) return static_cast<Platform*>(obA->platformPointer);
		if (obB->platformPointer) return static_cast<Platform*>(obB->platformPointer);
	}
	return 0;
}

/*btAlignedObjectArray<btVector3> Player::getObjectTouchingNormal()
{
	Physics::getSingleton()->getWorld()->setInternalTickCallback(
	mRigidBody->getOverlappingPairCache()->getOverlappingPairArray()[0].m_pProxy0->
	btAlignedObjectArray<btVector3> toReturn;
	btDiscreteDynamicsWorld* world = Physics::getSingleton()->getWorld();
	int numberManifolds = world->getDispatcher()->getNumManifolds();
	for (int i = 0; i < numberManifolds; i++)
	{
		btPersistentManifold* contactManifold = world->getDispatcher()->getManifoldByIndexInternal(i);
		btCollisionObject* obA = const_cast<btCollisionObject*>(contactManifold->getBody0());
		btCollisionObject* obB = const_cast<btCollisionObject*>(contactManifold->getBody1());
		if (obA == mRigidBody || obB == mRigidBody)
		{
			int numContacts = contactManifold->getNumContacts();
			for (int j = 0; j < numContacts; j++)
			{
				btManifoldPoint& pt = contactManifold->getContactPoint(j);
				if (pt.getDistance() < CONTACT_MARGIN)
				{
					toReturn.push_back(pt.m_normalWorldOnB);
				}
			}
		}
	}

	if (Keys::singleton->isKeyDown(Keys::singleton->K_TOGGLE_DEBUG_MODE) && !Keys::singleton->isKeyDownLastUpdate(Keys::singleton->K_TOGGLE_DEBUG_MODE))
	{
		mDebugDrawer->setDebugMode(!mDebugDrawer->getDebugMode());
	}

	return toReturn;
}*/

bool Player::isTouchingBelow()
{
	return mKinematicController->onGround();// && mKinematicController->customOnGround());
	/*btAlignedObjectArray<btVector3> toReturn = getObjectTouchingNormal();
	for (int i = 0; i < toReturn.size(); i++)
	{
		if (toReturn[i].y() == 1)
		{
			return true;
		}
	}
	return false;
	/*static int counter = 0;
	if (Math::Abs(mPlayerBody->getLinearVelocity().y()) < VELOCITY_Y_MARGIN)
	{
		counter++;
		if (counter >= 15)
		{
			return true;
		}
	}
	else
	{
		counter = 0;
	}
	return false;
	/*btVector3 from = mPlayerBody->getCenterOfMassPosition(), to = btVector3(0, -1, 0);
	from.setY(from.y() - mBodyDimensions.y() + 0.1);
	//mDebugDrawer->drawLine(from, to, btVector3(0, 0, 0));
	ClosestNotMe rayCallback(mPlayerBody);
	Physics::getSingleton()->getWorld()->rayTest(from, to, rayCallback);
	if (rayCallback.hasHit() && rayCallback.m_closestHitFraction < 0.10)
	{
		return true;
	}
	else
	{
		return false;
	}*/
}

#if ENABLE_WALL_GRAB
btVector3 Player::getTopPositionForWallGrab()
{
	Vector3 position = getLocation();
	position.y += PLAYER_BODY_Y / 2 + PLAYER_WALL_GRAB_OFFSET_Y;
	return TO_BULLET(position);
}
#endif

Vector3 Player::getOrientationForAudioLib()
{
	return Quaternion(mCameraYaw->_getDerivedOrientation().getYaw(), Vector3::UNIT_Y) * Vector3::UNIT_Z;;
}

#if ENABLE_WALL_GRAB
btVector3 Player::getOrientationForWallGrab()
{
	Vector3 newVector = mBodyNode->getOrientation() * Vector3::UNIT_Z;
	return TO_BULLET(newVector);
	btScalar orientation = btScalar(mBodyNode->getOrientation().getYaw().valueDegrees());

	btVector3 toReturn = btVector3(0, 0, 0);
	if (orientation < 45 && orientation >= -45)
	{
		toReturn = btVector3(0, 0, 1);
	}
	else if (orientation >= 45 && orientation <= 90 + 45)
	{
		toReturn = btVector3(1, 0, 0);
	}
	else if (Math::Abs(orientation) >= 90 + 45)
	{
		toReturn = btVector3(0, 0, -1);
	}
	else if (orientation < -45 && orientation >= -(90 + 45))
	{
		toReturn = btVector3(-1, 0, 0);
	}
	

	return toReturn;
}
#endif

void Player::_onPlayerToggle()
{
	SceneNode* parentNode = static_cast<SceneNode*>(mCamera->getParentNode());
	if (parentNode) parentNode->detachObject(mCamera);
	mCameraHolder->attachObject(mCamera);
}

void Player::_editorSetOrigin(Vector3 origin)
{
	btTransform transf;
	transf.setIdentity();
	transf.setOrigin(TO_BULLET(origin));
	mRigidBody->setWorldTransform(transf);
}
//
//ColourValue Player::Trail::possibleColors[5] = {
//	ColourValue(0.0, 1.0, 1.0, 1.0),
//	ColourValue(0.502, 0, 0.502, 1.0),
//	ColourValue(0.749, 1, 0, 1.0),
//	ColourValue(1, 0.212, 0.212, 1.0),
//	ColourValue(1, 0.965, 0.024, 1.0)
//};

ColourValue Player::Trail::possibleColors[5] = {
	ColourValue(0.0, 1.0, 1.0, 1.0),
	ColourValue(0.0, 1.0, 1.0, 1.0),
	ColourValue(0.0, 1.0, 1.0, 1.0),
	ColourValue(0.0, 1.0, 1.0, 1.0),
	ColourValue(0.0, 1.0, 1.0, 1.0)
};