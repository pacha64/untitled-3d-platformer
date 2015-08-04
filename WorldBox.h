#pragma once
#include "stdafx.h"
#include "BaseMovementState.h"



class WorldBox
{
public:
	enum CustomMaterial
	{
		CM_TUTORIAL_PATH_1,
		CM_TUTORIAL_PATH_1_BREAKABLE,
		CM_TUTORIAL_PATH_2_KILLBOX,
		CM_TUTORIAL_PATH_3,
		CM_TUTORIAL_PATH_4_TOP_KILLBOX,
		CM_TUTORIAL_PATH_4_KILLBOX,
		CM_TUTORIAL_PATH_5,
		CM_TUTORIAL_PATH_6,
		CM_OUT_MAP_1,
		CM_BOTTOM_LAIR_1,
		CM_CHAPEL_1,
		CM_HORIZONTAL_VERTICAL_SPECIAL_1,
		CM_SIZE
	};

	WorldBox(StaticGeometry*, btTransform&, CustomMaterial material, bool isStatic);//, btQuaternion& rotation = btQuaternion(0, 1, 0, 0));
	~WorldBox(void);

	static void initialize(SceneManager*, btDiscreteDynamicsWorld*);
	static String materialId2string(WorldBox::CustomMaterial material);
	static Vector3 typeSize(CustomMaterial);
	static bool materialIsBreakable(CustomMaterial);
	static bool materialIsKillbox(CustomMaterial);
	static bool materialIsTopKillbox(CustomMaterial);
	bool materialIsBreakable()
	{
		return materialIsBreakable(mMaterialId);
	}
	bool materialIsKillbox()
	{
		return materialIsKillbox(mMaterialId);
	}
	bool materialIsTopKillbox()
	{
		return materialIsTopKillbox(mMaterialId);
	}

#if !FIX_INSERT_PLATFORM_EDITOR
	bool insertFix;
#endif

	void removePlatformEffects();
	void setPlatformEffects(CustomMaterial);

	double getYmax()
	{
		return mBody->getWorldTransform().getOrigin().y() + size().y / 2;
	}

	void setEnabled(bool);
	bool getEnabled() { return mIsEnabled; }
	Vector3 size() { return typeSize(mMaterialId); }
	/// returns 0 if it is static
	SceneNode* getNode()
	{
		return mNode;
	}
	Entity* getEntity()
	{
		return mEntity;
	}
	btCollisionObject* getBody()
	{
		return mBody;
	}
	btCollisionShape* getShape()
	{
		return mShape;
	}
	CustomMaterial getMaterial()
	{
		return mMaterialId;
	}
	/*btVector3 getBoundingBox()
	{
		return mBoundingBox;
	}*/
protected:
	bool mIsEnabled;
	CustomMaterial mMaterialId;
//	Vector3 mSize;
	//btVector3 mBoundingBox;
	static btCollisionShape* mAllShapes[CustomMaterial::CM_SIZE];
	static String mCustomMaterialStrings[CustomMaterial::CM_SIZE];
	static Entity* mStaticBoxEntity[CustomMaterial::CM_SIZE];//[BoxType::BT_SIZE];
	static SceneManager* mManager;
	static btDiscreteDynamicsWorld* mWorld;
//	static String mMaterials[CustomMaterial::CM_SIZE];
	btCollisionObject* mBody;
	btCollisionShape* mShape;
	SceneNode* mNode;
	Entity* mEntity;
};
