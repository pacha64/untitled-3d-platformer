#pragma once
#include "stdafx.h"
#include "MapInformation.h"
#include "balance.h"
#include "MovablePlatform.h"
#include "md5.h"
#include "DecalUtility.h"
#include "OgreDecal.h"
#include "WorldActivator.h"
#include "MapFormat.h"
//#include "BlackKing.h"
#include "Monster.h"

class MonsterState;
class Player;
class GameState;

#define PLATFORM_HEIGHT 7
#define PLATFORM_WIDTH 25
#define HEAP_PLANE_WIDTH 3
#define DISTANCE_BETWEEN_PLATFORMS 175



struct WorldStructure
{
	enum NodeType
	{
		NT_BLOCK,
		NT_PATH,
		NT_MATRIX
	};

	struct Meta
	{
		Meta()
		{
			name = "";
			description = "";
			creator = "";
			filename = "";
		}
		String name;
		String description;
		String creator;
		String filename;
	};
	
	struct Map
	{
		struct MovableArgs
		{
			MovableArgs()
			{
				isLinear = false;
				isRotate = false;
				isScale = false;
				linearArgs = Vector3::ZERO;
				scaleArgs = Vector3::ZERO;
				scalarLimit = 1;
				linearSpeed = 0;
				rotateSpeed = Degree(0);
			}
			bool isLinear, isRotate, isScale;
			Vector3 linearArgs, scaleArgs;
			Real scalarLimit, linearSpeed;
			Degree rotateSpeed;
		};

		struct Block
		{
			Block()
			{
				movableArgs = 0;
				isStart = false;
				isEnd = false;
				isRespawn = false;
				isPoint = false;
				child = 0;
			}
			json_t* child;
			Vector3 origin;
			//Vector3 size;
			Degree rotation;
			MovableArgs* movableArgs;
			bool isStart, isEnd, isRespawn, isPoint;
		};

		std::vector<Block*> blocks;
		//std::vector<Path*> paths;
	};

	Meta meta;
	Map map;
};

enum WorldBlockType
{
	BT_STATIC,
	BT_MOVABLE_PLATFORM
};

class WorldGenerator
{
public:
	enum WorldGeneratorType
	{
		WGT_MINI_MAPS,
		WGT_INFINITE_PATH,
		WGT_GENERIC,
		WGT_WORLD_EDITOR,
		WGT_START_MENU,
		WGT_TUTORIAL
	};

	enum MapZones
	{
//		MZ_MAPOUT,
//		MZ_BLACK_CASTLE,
//		MZ_CHURCH,
		MZ_ICE_CORE,
		MZ_CAVE,
		MZ_BOTTOM_LAIR,
//		MZ_TUTORIAL_PATH,
		MZ_SIZE
	};


	WorldGenerator(SceneManager*);
	virtual ~WorldGenerator(void);
	virtual WorldGeneratorType getGeneratorType()
	{
		return WorldGeneratorType::WGT_GENERIC;
	}
	static bool validJson(json_t* root);
	static void destroyWorldStructure(WorldStructure*);
	virtual Vector3 worldRespawn2vector(WorldActivator*);
	virtual Vector3 getInitialRespawn();
	virtual Vector3 getRandomRespawn();
	virtual void activateRespawn(WorldActivator*);
	virtual WorldActivator* getPointAt(Vector3&, Vector3&);
	virtual WorldActivator* getRespawnAt(Vector3&, Vector3&);
	virtual bool isInsideWorld(Vector3 aabbMax, Vector3 aabbMin);
	MapZones getCurrentMapZone(Vector3 position);
	void playerPlatformShaderArgs(Vector4 crashArgs, Vector3 playerPosition);
	void disableAllCheckpoints()
	{
		for (int i = 0; i < mPoints.size(); i++)
		{
			mPoints[i]->setActivation(false);
		}
	}
	String getSeed()
	{
		return mSeed;
	}
	void setSeed(String string)
	{
		mSeed = string;
	}
	MapFormat* getFormat()
	{
		return mCurrentFormat;
	}
	//bool load(String&);
	//Real getFarthestDistanceToSoul(Vector3 position);
	virtual bool isOnTopOfGoal(Player*);
	//virtual bool updateTouchingSoul(Vector3&, Vector3&);
	uint getSeedNumeric();
	void setGameState(GameState* state) { mGameState = state; }
	void setMapStructure(MapFormat* world);
	virtual void destroy();
	virtual void generate();
	//void gpuParamtersHelper();
	virtual void update(const FrameEvent&, std::vector<Player*>*);
	std::vector<MovablePlatform*>* getMovablePlatforms()
	{
		return &mMovablePlatforms;
	}
	std::vector<Platform*>* getPlatforms()
	{
		return &mPlatforms;
	}
	void newCharacterRespawn(Player*);
	void newCharacterDied(Player*, Vector3 = Vector3::UNIT_Y);
	virtual Real calculateTimeLeft() { return 0.0; }
	//int counter;
	int getTotalScore()
	{
		return mPoints.size();
	}
	virtual Platform* generateMapBlock(MapFormat::Block* block, bool useStatic = true);
	static WorldGenerator* getCurrentInstance()
	{
		return mCurrentInstance;
	}
	String mapzone2string(MapZones mz)
	{
		return mMapZonesNames[mz];
	}
protected:
	virtual void platformShaderArgs();
	SceneNode* mMapNodes[MapZones::MZ_SIZE];
	Entity* mMapEntities[MapZones::MZ_SIZE];
	btCollisionObject* mMapCollisionObject[MapZones::MZ_SIZE];
	std::pair<String, String> mMeshMaterialName[MapZones::MZ_SIZE];
	std::pair<Vector3, Vector3> mMapZonesAabb[MapZones::MZ_SIZE];
	String mMapZonesNames[MapZones::MZ_SIZE];
	static WorldGenerator* mCurrentInstance;
	Monster* mMonsters[Monster::MonsterType::MT_SIZE];
	double mTimeSinceStart;
	void resetAllMovablePlatforms();
	WorldActivator* mCurrentPlayerActivator;
	//int mTotalScore;
	std::vector<WorldTouchableObject*> mTouchableGoals;
	//MonsterState* mMonsterState;
	Monster* mMapMonster;
	MapFormat* mCurrentFormat;
	StaticGeometry* mStaticGeometry;
	//StaticGeometry* mStaticGeometryCircle;
	void recreateSkyBox();
	WorldStructure* mMapWorld;
	SceneNode* mSkyBox;
	SceneNode* mDeathNode;
	String mSeed;
	//Vector3 mStartVector;
	//void fixMapLimits();
	btVector3 mWorldTopRight;
	btVector3 mWorldBottomLeft;
	bool mPlatformGeneratorStateMatrix[13][13];
	bool mPlatformGeneratorToGenerate[13][13];
	bool mPlatformGeneratorStateMiddle[12][12];
	void fillGeneratorArray();
	void generateMiddlePlatform(btVector3&, Radian);
	void generatePlatformAt(btVector3&, int, int, int = 5 * 2 - 1, int = 5 * 2 - 1);
	//void generateFourSide(btVector3& origin);
	const btVector3 blockDimension;
	//GameDifficulty mDifficulty;
	int numberOfBlocks()
	{
		return 5;
	}
	//void createBlock(btVector3&, WorldBlockConnection, Radian);
	std::vector<Platform*> mPlatforms;
	//std::vector<WorldSoul*> mSouls;
	std::vector<WorldActivator*> mPoints;
	std::vector<WorldActivator*> mRespawns;
	WorldActivator* mFirstRespawn;
	std::vector<MovablePlatform*> mMovablePlatforms;
	//std::vector<Platform*>* generatePath(WorldStructure::Map::PathHelper*, Vector3 = Vector3::ZERO, bool = false);
	SceneManager* mSceneManager;
	GameState* mGameState;
};
