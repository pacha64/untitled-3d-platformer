#pragma once
#include "stdafx.h"
#include "worldtouchableobject.h"
#include "balance.h"

class Platform;

class WorldActivator :
	public WorldTouchableObject
{
public:
	enum ActivatorType
	{
		AT_RESPAWN,
		AT_POINTS
	};
	WorldActivator(ActivatorType, Platform* platform);
	~WorldActivator(void);
	void setActivation(bool activate);
	void update(const Ogre::FrameEvent& evt);
	void setInitialPosition();
	Vector3 getBase();
	void setVisible(bool);
	bool isActivated()
	{
		return mIsActivated;
	}
	ActivatorType getActivatorType()
	{
		return mActivatorType;
	}
	void _changeType(ActivatorType type)
	{
		mActivatorType = type;
		setMaterial();
	}
	void _translateY(Real y)
	{
		mNode->translate(Vector3(0, y / PLATFORM_SIZE_UNIT, 0));
	}
	SceneNode* _getNode() { return mNode; }
	void _resetTime();
	void _newPlatform(Platform* plat) { mPlatformParent = plat; }
	static String activatortype2string(WorldActivator* activator)
	{
		switch(activator->getActivatorType())
		{
		case ActivatorType::AT_RESPAWN:
			if (activator->isStartRespawnHelper)
			{
				return "Initial respawn";
			}
			else
			{
				return "Respawn";
			}
		case ActivatorType::AT_POINTS:
			return "Point";
		}

		return "None";
	}
	Platform* getParent()
	{
		return mPlatformParent;
	}
	bool isStartRespawnHelper;
protected:

	void setMaterial()
	{
		switch (mActivatorType)
		{
		case AT_POINTS:
			mEntityActive->setMaterialName("Checkpoint/Activated");
			mEntityNotActive->setMaterialName("Checkpoint/NotActivated");
			break;
		case AT_RESPAWN:
			mEntityActive->setMaterialName("Respawn/Activated");
			mEntityNotActive->setMaterialName("Respawn/NotActivated");
			break;
		}
	}

	Platform* mPlatformParent;
	ActivatorType mActivatorType;
	bool mIsActivated;
	SceneNode* mNode;
	Entity* mEntityActive;
	Entity* mEntityNotActive;
};

