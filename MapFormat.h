#pragma once
#include "stdafx.h"
#include "balance.h"
#include "WorldBox.h"

struct MapFormat
{
	struct Block
	{
		Block()
		{
			isPoint = false;
			isRespawn = false;
			origin = Vector3::ZERO;
			linearLocation = Vector3::ZERO;
			rotation = Degree(0);
			linearSpeed = 0;
			rotationSpeed = 0;
			extraArgsBit = 0;
			specialBlockId = -1;
			isRotationMovement = false;
			isRightRotationMovement = true;
//			size = Vector3(PLATFORM_SIZE_UNIT, PLATFORM_SIZE_UNIT, PLATFORM_SIZE_UNIT);
		}

//		Vector3 size;
		WorldBox::CustomMaterial materialIndex;
		Vector3 origin;
		Degree rotation;
		int linearSpeed;
		/// 1st bit: kill box, 2nd bit: breakable platform
		int extraArgsBit;
		int specialBlockId;
		Vector3 linearLocation;
		int rotationSpeed;
		bool
			isPoint,
			isRespawn,
			isRotationMovement,
			isRightRotationMovement;
	};

	MapFormat(void);
	MapFormat(String filename);
	~MapFormat(void);

	char name[MAP_NAME_MAX_LENGTH];
	char description[MAP_DESCRIPTION_MAX_LENGTH];
	int firstRespawnOffset;
	Vector3 size;
	std::vector<Block*> blocks;

	String filename;

	void saveToFile(String filename);
	void loadFromFile(String filename);
	static bool validMap(String filename);
};