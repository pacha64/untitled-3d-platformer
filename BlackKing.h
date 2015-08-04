#pragma once
#include "monster.h"

class BlackKing;

class BlackKingMotionState
	: public btMotionState
{
public:
	BlackKingMotionState(BlackKing*);
	~BlackKingMotionState(void);
	virtual void getWorldTransform(btTransform& worldTrans) const;
	virtual void setWorldTransform(const btTransform& worldTrans);
};

class BlackKing :
	public Monster
{
public:

	BlackKing(void);
	~BlackKing(void);
	virtual MonsterType getMonsterType()
	{
		return MT_BLACK_KING;
	}
	virtual void update(Player*, const Ogre::FrameEvent& evt);
	bool isBodyWithThunder();
	inline Vector3 getPosition() { return mNode->_getDerivedPosition(); }// }
private:
	enum BlackKingAnimations
	{
		BKA_STANDING,
		BKA_RUNNING,
		BKA_HORIZONTAL_ATTACK,
		BKA_VERTICAL_ATTACK,
		BKA_DOWN_THUNDER,
		BKA_UP_THUNDER,
		BKA_TURNING_LEFT,
		BKA_TURNING_RIGHT,
		BKA_SIZE
	};
#if ENABLE_ANIMATION_BLENDING
	bool mFadingIn[BKA_SIZE];
	bool mFadingOut[BKA_SIZE];
	void fadeAnimations(Real);
#endif
	int mAnimationID;
	void updateAnimations(Real deltaTime);
	void setupAnimations();
	void setAnimation(BlackKing::BlackKingAnimations animation, bool reset = false);
	AnimationState* mAnimations[BKA_SIZE];
	btRigidBody* mBody;
	btCylinderShape* mShape;
	BlackKingMotionState* mMotionState;
	btGhostObject* mDownSwordObject;
	Real mTimerHelper;
	/// 0 vertical, 1 horizontal
	int mAttackType;
	Real
		mTimeThunderHelper;
	bool mIsAttacking;
	Entity* mEntity;
	SceneNode* mNode;
};