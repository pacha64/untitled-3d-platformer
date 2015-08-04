#include "BlackKing.h"
#include "Player.h"

BlackKingMotionState::BlackKingMotionState(BlackKing* blackKing)
{
}

BlackKingMotionState::~BlackKingMotionState(void)
{
}

void BlackKingMotionState::getWorldTransform(btTransform& worldTrans) const
{
	//worldTrans = mTransform;
}
void BlackKingMotionState::setWorldTransform(const btTransform& worldTrans)
{
	//mTransform = worldTrans;
	//btQuaternion rot = worldTrans.getRotation();
	//Quaternion quat(rot.w(), rot.x(), rot.y(), rot.z());
	//mNode->setOrientation(quat);
	//btVector3 pos = worldTrans.getOrigin();
	//mNode->setPosition(pos.x(), pos.y(), pos.z());
}

BlackKing::BlackKing(void)
	: Monster(),
	mBody(0),
	mShape(0),
	mEntity(0),
	mNode(0),
	mDownSwordObject(0),
	mTimerHelper(0.0),
	mIsAttacking(false),
	mAttackType(0),
	mAnimationID(BKA_SIZE + 1),
	mTimeThunderHelper(0.0)
{
	btTransform transf;
	transf.setIdentity();
	transf.setOrigin(BLACK_KING_ORIGIN);

	mShape = new btCylinderShape(BLACK_KING_HALF_SIZE);
	mBody = new btRigidBody(0.0f, mMotionState = new BlackKingMotionState(this), mShape);
	mBody->setWorldTransform(transf);
	mBody->setLinearFactor(btVector3(0.0f, 0.0f, 0.0f));
	mBody->setAngularFactor(btVector3(0.0f, 0.0f, 0.0f));
	mBody->monsterPointer = this;
	
	Physics::getSingleton()->getWorld()->addRigidBody(mBody);
	
	btBoxShape* downSwordShape = new btBoxShape(BLACK_KING_DOWN_SWORD_SIZE);
	mDownSwordObject = new btGhostObject();
	mDownSwordObject->setCollisionShape(downSwordShape);
	mDownSwordObject->setWorldTransform(btTransform::getIdentity());
	
	Physics::getSingleton()->getWorld()->addCollisionObject(mDownSwordObject, 0);
	
	mNode = mSceneManager->getRootSceneNode()->createChildSceneNode(TO_OGRE(BLACK_KING_ORIGIN));
	mEntity = mSceneManager->createEntity("black-king.mesh");
	mEntity->setMaterialName("Monster/BlackKing");
	mNode->attachObject(mEntity);
	
	setupAnimations();
}


BlackKing::~BlackKing(void)
{
}

void BlackKing::update(Player* player, const Ogre::FrameEvent& evt)
{
	Vector3
		position = getPosition(),
		playerPos = player->getBodyNode()->_getDerivedPosition(),
		orientation = mNode->getOrientation() * Vector3::UNIT_Z,
		playerOrientation = playerPos - position;
	
	position.y = 0;
	playerPos.y = 0;
	orientation.y = 0;
	playerOrientation.y = 0;

	orientation.normalise();
	playerOrientation.normalise();

	Real angle = Radian(Math::ACos(orientation.dotProduct(playerOrientation))).valueAngleUnits();
	
	if (angle > 0 && !mIsAttacking && !player->isDead())
	{
		if (Vector3::UNIT_Y.dotProduct(orientation.crossProduct(playerOrientation)) < 0)
		{
			angle = -angle;
		}
		Real toRotate = evt.timeSinceLastFrame * BLACK_KING_ROTATE_SPEED;
		if (angle < 0) toRotate = -toRotate;
		if (Math::Abs(angle) < Math::Abs(toRotate))
		{
			toRotate = angle;
		}
		mNode->yaw(Degree(toRotate));
	}

	// TODO remove this
	const Real
		timeAttackVertical = mAnimations[BKA_VERTICAL_ATTACK]->getLength(),
		timeAttackVerticalStartDmg = 0.25,
		timeAttackHorizontal = mAnimations[BKA_HORIZONTAL_ATTACK]->getLength(),
		timeAttackHorizontalStartDmg = 0.1,
		timeThunderUp = mAnimations[BKA_UP_THUNDER]->getLength();

	if ((playerPos - position).length() < BLACK_KING_MIN_DISTANCE_THUNDER_UP)
	{
		mTimeThunderHelper += evt.timeSinceLastFrame;
	}
	else
	{
		mTimeThunderHelper = 0;
	}
	
	if (!mIsAttacking && (playerPos - position).length() < BLACK_KING_MIN_DISTANCE_THUNDER_UP && mTimeThunderHelper >= BLACK_KING_MIN_TIME_THUNDER_UP)
	{
		mIsAttacking = true;
		mAttackType = 2;
		mTimerHelper = 0.0;
		setAnimation(BKA_UP_THUNDER, true);
		mTimeThunderHelper = 0;
	}

	if (Math::Abs(angle) < BLACK_KING_ANGLE_MIN && !mIsAttacking)
	{
		if (!player->isDead())
		{
			if ((playerPos - position).length() > BLACK_KING_MIN_DISTANCE)
			{
				Vector3 toAdd = Vector3::UNIT_Z * BLACK_KING_SPEED * evt.timeSinceLastFrame;
				mNode->translate(toAdd, Node::TS_LOCAL);
				setAnimation(BKA_RUNNING);
			}
			else if (!mIsAttacking && Math::Abs(angle) < 3)
			{
				// start attacking
				mIsAttacking = true;
				mTimerHelper = 0.0;
				if (BLACK_KIN_PERCENTAGE_ATTACK_VERTICAL >= Math::RangeRandom(1, 100))
				{
					mAttackType = 0;
					setAnimation(BKA_VERTICAL_ATTACK, true);
				}
				else
				{
					mAttackType = 1;
					setAnimation(BKA_HORIZONTAL_ATTACK, true);
				}
				mTimeThunderHelper = 0;
			}
			
		}
		else
		{
			setAnimation(BKA_STANDING);
		}
	}
	else if (!mIsAttacking)
	{
		if (!player->isDead())
		{
			if (angle < 0)
			{
				setAnimation(BKA_TURNING_LEFT);
			}
			else
			{
				setAnimation(BKA_TURNING_RIGHT);
			}
		}
		else
		{
			setAnimation(BKA_STANDING);
		}
	}

	bool shouldCheckKill = false;
	Real swordAttackY = 0;
	Degree swordAttackRotation = Degree(0);

	if (mIsAttacking)
	{
		mTimerHelper += evt.timeSinceLastFrame;
		Real time = 0, timeStartDmg = 0;
		switch (mAttackType)
		{
		case 0:
			time = timeAttackVertical;
			timeStartDmg = timeAttackVerticalStartDmg;
			break;
		case 1:
			time = timeAttackHorizontal;
			timeStartDmg = timeAttackHorizontalStartDmg;
			break;
		case 2:
			time = timeThunderUp;
			timeStartDmg = timeAttackHorizontalStartDmg; // TODO thunder time
			break;
		}
		if (mTimerHelper >= (time))
		{
			mIsAttacking = false;
		}
		else
		{
			if (mTimerHelper >= timeStartDmg)
			{
				shouldCheckKill = true;
			}
		}
	}

	if (shouldCheckKill)
	{
		for (int i = 0; i < mDownSwordObject->getOverlappingPairs().size(); i++)
		{
			if (mDownSwordObject->getOverlappingPairs()[i] == player->getRigidBody()) player->shouldKillPlayer = true;
		}
	}

	btTransform transf = mBody->getWorldTransform();
	transf.setOrigin(TO_BULLET(mNode->_getDerivedPosition()));// + btVector3(0, BLACK_KING_ORIGIN.y(), 0));
	transf.setRotation(TO_BULLET(mNode->_getDerivedOrientation()));
	mBody->setWorldTransform(transf);
	
	Bone* armBone = mEntity->getSkeleton()->getBone("high.arm.right");
	
	transf.setRotation(TO_BULLET(
		mNode->_getDerivedOrientation() * armBone->_getDerivedOrientation()	
	));
	
	transf.setOrigin(TO_BULLET(
		mNode->_getDerivedPosition() +
		mNode->_getDerivedOrientation() * armBone->_getDerivedPosition() +
		mNode->_getDerivedOrientation() * armBone->_getDerivedOrientation() * Vector3(0, BLACK_KING_DOWN_SWORD_SIZE.y() * 2, 0)
	));

	mDownSwordObject->setWorldTransform(transf);

	updateAnimations(evt.timeSinceLastFrame);
	
}

bool BlackKing::isBodyWithThunder()
{
	return (BlackKingAnimations)mAnimationID == BKA_UP_THUNDER;
}

void BlackKing::setupAnimations()
{
	mEntity->getSkeleton()->setBlendMode(SkeletonAnimationBlendMode::ANIMBLEND_AVERAGE);

	String animNames[] = {
		"Standing",
		"Running",
		"HorizontalAttack",
		"VerticalAttack",
		"DownThunder",
		"UpThunder",
		"TurningLeft",
		"TurningLeft",
	};
	
#if ENABLE_ANIMATION_BLENDING
	memset(mFadingIn, false, sizeof(mFadingIn));
	memset(mFadingOut, false, sizeof(mFadingOut));
#endif

	for (int i = 0; i < BKA_SIZE; i++)
	{
		mAnimations[i] = mEntity->getAnimationState(animNames[i]);
		mAnimations[i]->setLoop(true);
		mAnimations[i]->setEnabled(false);
		
#if ENABLE_ANIMATION_BLENDING
		mFadingIn[i] = false;
		mFadingOut[i] = false;
#endif
	}
	
		//BKA_STANDING,
		//BKA_RUNNING,
		//BKA_HORIZONTAL_ATTACK,
		//BKA_VERTICAL_ATTACK,
		//BKA_DOWN_THUNDER,
		//BKA_UP_THUNDER,
		//BKA_TURNING_LEFT,
		//BKA_TURNING_RIGHT,
		//BKA_SIZE
	mAnimations[BKA_HORIZONTAL_ATTACK]->setLoop(false);
	mAnimations[BKA_VERTICAL_ATTACK]->setLoop(false);
	mAnimations[BKA_DOWN_THUNDER]->setLoop(false);
	mAnimations[BKA_UP_THUNDER]->setLoop(false);

	setAnimation(BKA_STANDING, true);
}

#if ENABLE_ANIMATION_BLENDING
void BlackKing::fadeAnimations(Real deltaTime)
{
	for (int i = 0; i < BKA_SIZE; i++)
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

void BlackKing::updateAnimations(Real deltaTime)
{
	if (mAnimationID >= 0 && mAnimationID < BKA_SIZE)
	{
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

void BlackKing::setAnimation(BlackKing::BlackKingAnimations animation, bool reset)
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
	if (mAnimationID >= 0 && mAnimationID < BKA_SIZE && animation != mAnimationID)
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