#pragma once
#include "stdafx.h"
#include "BtOgreGP.h"
#include "BtOgrePG.h"
#include "balance.h"
#include "WorldGenerator.h"
#include "Keys.h"
#include "PlayerController.h"
#include "Physics.h"
#include "TextRenderer.h"
#include "SoundManager.h"
#include "DebugDrawer.h"


class Player
{
public:
	enum PlayerAnimation
	{
		PA_STANDING,
		PA_FALLING_START,
		PA_RUNNING,
		PA_HEAP_JUMP,
		PA_HIGH_JUMP,
		PA_NORMAL_JUMP,
		PA_WALL_JUMP,
		PA_GRABBING_WALL,
		PA_GO_UP_WALL,
		PA_LANDING_AFTER_JUMP,
		PA_DYING,
		PA_CELEBRATE,
		//PA_ACTION_SPIN,
		PA_NUM_ANIMS,
	};
	
	enum JumpType
	{
		JT_WALL_JUMP_NORMAL,
		JT_WALL_JUMP_LONG,
		JT_NORMAL_STATIC,
		JT_NORMAL_MOVING,
		JT_LONG,
#if ENABLE_BACKWARDS_JUMP
		JT_BACKWARDS,
#endif
		JT_NONE
	};
	Player(WorldGenerator*, Camera*, SceneManager*, RenderWindow*);
	~Player(void);
	void resetCamera();
	void update(const FrameEvent&);//, bool isPaused);
	void updatePlatformBelow(const FrameEvent&);
	bool isTouchingBelow();
	void onGoalReaching();
	Vector3 getOrientationForAudioLib();
	static Vector3 trimJumpVelocity(Vector3, int jumpType);


//	bool isJumping();
	Vector3 getAabbMax()
	{
		Vector3 vectorHelper = TO_OGRE(mRigidBody->getWorldTransform().getOrigin());//TO_OGRE(mRigidBody->getWorldTransform().getOrigin());
		return vectorHelper + Vector3(PLAYER_BODY_X, PLAYER_BODY_Y, PLAYER_BODY_Z);
	}
	Vector3 getAabbMin()
	{
		Vector3 vectorHelper = TO_OGRE(mRigidBody->getWorldTransform().getOrigin());//TO_OGRE(mRigidBody->getWorldTransform().getOrigin());
		return vectorHelper - Vector3(PLAYER_BODY_X, PLAYER_BODY_Y, PLAYER_BODY_Z);
	}
	Vector3 getLocation()
	{
		return TO_OGRE(mRigidBody->getWorldTransform().getOrigin());
	}
	Real getTimeUntilRespawn()
	{
		Real toReturn = 0;
		toReturn = PLAYER_RESPAWN_TIME - mSecondsSinceDie;
		if (toReturn < 0)
		{
			return 0;
		}
		else
		{
			return toReturn;
		}
	}
	PlayerController* getController()
	{
		return mKinematicController;
	}
	btRigidBody* getRigidBody()
	{
		return mRigidBody;
	}
	bool isDead()
	{
		return mGameOver;

	}
	//void disableBottomShadow();
	//void enableBottomShadow(btVector3& position);
	void respawnCharacter();
	void killCharacter();
	SceneNode* getBodyNode()
	{
		return mBodyNode;
	}
	String getPlayerName()
	{
		return "pacha";
		//return mPlayerName;
	}
	void setRespawnPoint(Vector3 point)
	{
		mCurrentRespawnPoint = point;
	}
	bool shouldKillPlayer;
	void onMovablePlatformMovement(MovablePlatform*, const FrameEvent& evt);

	void _onPlayerToggle();
	void _editorSetOrigin(Vector3);
	Platform* getStandingPlatform();
protected:
	bool shouldBlockMovement()
	{
		for (int i = 0; i < mKinematicController->getTouchingPlatform().size(); i++)
		{
			if (mKinematicController->getTouchingPlatform()[i].first->isKillBox() ||
				mKinematicController->getTouchingPlatform()[i].first->isTopKillBox() &&
				normalIsStandable(TO_BULLET(mKinematicController->getTouchingPlatform()[i].second)))
			{
				return true;
			}
		}
#if ENABLE_WALL_GRAB
		return mGrabbingWall || mReachedGoal || mGameOver;
#else
		return mReachedGoal || mGameOver;
#endif
	}
	struct CrashEffect
	{
		enum CrashEffectType
		{
			WALL_JUMP,
			LONG_JUMP,
#if ENABLE_BACKWARDS_JUMP
			HIGH_JUMP,
#endif
			NORMAL_STATIC_JUMP,
			NORMAL_MOVING_JUMP,
			AFTER_LANDING,
			AFTER_LANDING_SMALL
		};
		CrashEffect(Vector3 position, Quaternion orientation, CrashEffectType type);
		/*	: shouldDelete(false),
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
			case CrashEffect::AFTER_LANDING:
				secondsEffect = SECONDS_AFTER_FALLING_EFFECT;
				break;
			}
			
			node = Root::getSingleton().getSceneManager(SCENE_MANAGER_NAME)->getRootSceneNode()->createChildSceneNode();
			entity = Root::getSingleton().getSceneManager(SCENE_MANAGER_NAME)->createEntity("jump-crash.mesh");
			material = static_cast<MaterialPtr>(MaterialManager::getSingleton().getByName("Character/JumpEffect"));
			material = material->clone(requestNewRandomMaterialName());
			
			entity->setMaterialName("Character/JumpEffect");
			
			node->attachObject(entity);
			node->translate(position);
			node->setOrientation(orientation);
		}*/
		~CrashEffect();
		void update(const FrameEvent& evt);
		bool shouldDelete;
		CrashEffectType type;
	private:
		SceneNode* node;
		Entity* entity;
		Real time, secondsEffect;
		MaterialPtr material;
	};
	struct Trail
	{
		Trail();
		void update(const FrameEvent& evt);
		void startTrail();
		void reset(Vector3 position, Quaternion animation, AnimationState* animationState);
		int colorIndex;
		bool resetFlag;
		SceneNode* node;
		Entity* entity;
		Real timeSinceReset;
	private:
		static ColourValue possibleColors[5];
	};
	Vector3 mCurrentRespawnPoint;
	enum ZoomLevel
	{
		ZM_1,
		ZM_2,
		ZM_3,
		ZM_4,
		ZM_SIZE
	};
	ZoomLevel mCurrentZoom;
	MovablePlatform* mMovablePlatformBelow;
#if ENABLE_WALL_GRAB
	Platform* mGrabbingPointer;
#endif
	SoundManager* mSoundManager;
#if ENABLE_WALL_GRAB
	void updateClimbUpAnimation(Real);
	bool mGrabbingWall;
#endif
	bool
#if ENABLE_WALL_GRAB
		mStartedClimbingUpLedge,
#endif
		mStillLandingAfterLengthyJump,
		mGameOver;
	Real
#if ENABLE_WALL_GRAB
		mTimeSinceClimbingUpLedge,
		mTimeForClimbingUpLedge,
		mTimeSinceLastWallGrab,
#endif
		mTimeAfterLengthyJump,
		mTimeSinceLengthyJump,
		mRunningAnimationLength;
	int mRunningStep;
#if ENABLE_WALL_GRAB
	btVector3 getTopPositionForWallGrab();
	btVector3 getOrientationForWallGrab();
#endif
	String mPlayerName;
	//bool mCameraAlwaysFocus;
	JumpType mLastJumpType;
	BillboardSet* mDeathBillboardSet;
	Billboard* mDeathBillboard;
	void onCameraChange(ZoomLevel);
	int mAnimationID;
#if ENABLE_ANIMATION_BLENDING
	void fadeAnimations(Real);
#endif
	void setupAnimations();
	void updateAnimations(Real);
	void setAnimation(PlayerAnimation, bool = false);
//	bool mIsGameOver;
	//bool mFadingIn[NUM_ANIMS];
	//bool mFadingOut[NUM_ANIMS];
	AnimationState* mAnimations[PA_NUM_ANIMS];
#if ENABLE_ANIMATION_BLENDING
	bool mFadingIn[PA_NUM_ANIMS];
	bool mFadingOut[PA_NUM_ANIMS];
#endif
	//btAlignedObjectArray<btVector3>* mNormals;
	Real
		mLastVerticalVelocity,
		mTimeOnAir,
		mTimeOnGround,
		mSecondsSinceDie,
		mTimeSinceWallJump,
		mTimeLastJump;
	//WorldGenerator* mGenerator;
	bool mPressJumpLastUpdate[3];
	bool mReachedGoal;
//	btAlignedObjectArray<btVector3> getObjectTouchingNormal();
	Vector3 mLastVectorVelocity;
	//SceneNode* mCameraHelper;
	//SceneNode* mDeathNode;
	Camera* mCamera;
	SceneNode* mMainNode;
	SceneNode* mBodyNode;
	SceneNode* mCameraPivot;
	SceneNode* mCameraYaw;
	SceneNode* mCameraPitch;
	SceneNode* mCameraHolder;
	SceneNode* mParticleHolder;
	Radian mLastStandingYaw;
	std::vector<CrashEffect*> mCrashEffects;
//	btSoftBody* mCapeSofyBody;
	Entity* mCapeEntity;
	//SceneNode* mBottomShadowNode;
	Entity* mPlayerEntity;//, *mShadowEntity;
	int mCurrentTrail;
	Real mTimeSinceLastTrail;
	Trail* mPlayerTrails[PLAYER_MAX_COPY_TRAIL];
	SceneManager* mSceneManager;
	WorldGenerator* mGenerator;
	//btRigidBody* mPlayerBody;
//	Vector3 mStartPosition;
	PlayerController* mKinematicController;
	btVector3 mInitialHorizontalVelocity;
	btRigidBody* mRigidBody;
	//bool mIsJumping;
	btVector3 mBodyDimensions;
	/*bool mCameraCenterMovementFlag;
	Radian mCameraCenterYaw;*/
};

class ClosestNotMe : public btCollisionWorld::ClosestRayResultCallback
{
protected:
	btRigidBody* mMe;
public:
	ClosestNotMe (btRigidBody* me) : btCollisionWorld::ClosestRayResultCallback(btVector3(0.0, 0.0, 0.0), btVector3(0.0, 0.0, 0.0))
	{
		mMe = me;
	}

	virtual btScalar addSingleResult(btCollisionWorld::LocalRayResult& rayResult,bool normalInWorldSpace)
	{
		if (rayResult.m_collisionObject == mMe)
			return 1.0;

		return btCollisionWorld::ClosestRayResultCallback::addSingleResult (rayResult, normalInWorldSpace);
	}
};