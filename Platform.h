#pragma once
#include "WorldBox.h"
#include "WorldActivator.h"

enum PlatformType
{
	PT_COMMON,
	PT_MOVABLE
};

class Platform
{
public:
	enum ShaderColors
	{
		SC_COMMON = 0,
		SC_BREAKABLE,
		SC_KILLBOX,
		SC_KILLBOX_TOP,
		SC_GOAL,
		SC_MOVABLE,
		SC_MOVABLE_KILLBOX_TOP,
		SC_ICE
	};
	Platform(WorldBox*);
	~Platform(void);
	WorldBox* getBox()
	{
		return mBox;
	}
	virtual PlatformType Type()
	{
		return PlatformType::PT_COMMON;
	}
	void _prepareForUpcast()
	{
		mBox = 0;
		mActivator = 0;
	}
	WorldActivator* getActivator()
	{
		return mActivator;
	}
	bool isStatic()
	{
		return ENABLE_STATIC_GEOMETRY && !(Type() == PT_MOVABLE || mIsBreakable);
	}

	int getShaderColorType();

	// ice
	bool isIce() { return mIsIce; }
	void setIce(bool isIce) { mIsIce = isIce; }

	// breakable
	void startBreak() { mIsBreaking = true; };
	bool startedBreaking() { return mIsBreaking;}
	bool isBreakable() { return mIsBreakable; }
	bool isBroken() { return mIsBroken; }
	void setBreakable(bool braekable) { mIsBreakable = braekable; }
	btScalar getTimeSinceStartedBreaking() { return mTimeSinceStartedBreaking; }

	// kill box
	//void setKillBox(bool isKillBox) { mIsKillBox = isKillBox; }
	bool isKillBox() { return mBox->materialIsKillbox(); }
	bool isTopKillBox() { return mBox->materialIsTopKillbox(); }

	virtual void update(const FrameEvent& evt);
	void updateBreakble(const FrameEvent& evt);
	void rebuildBreak();
	void addActivator(WorldActivator::ActivatorType);
	void _changeActivatorType(WorldActivator::ActivatorType);
	void _removeActivator();
//	void _addSize(Vector3);
	void _changePlatformType(WorldBox::CustomMaterial cm);
	void _translate(Vector3);
	virtual void _setPosition(Vector3);
	void _setRotate(Degree rotate);
	void _setKillFlag(bool killFlag);
	void _setBreakableFlag(bool);
	Quaternion orientationGrabbingPointer(btVector3& start, btVector3& to);
protected:
	WorldBox* mBox;
	WorldActivator* mActivator;

	// ice
	bool mIsIce;
	// break-related
	bool mIsBreaking, mIsBreakable, mIsBroken;
	// kill box
	bool mIsKillBox, mTopKillBox;
	btScalar mTimeSinceStartedBreaking;
	SceneNode* mBreakingParticleEffectNode;
};

