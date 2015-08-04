#include "MapFormat.h"
#include <jansson.h>

MapFormat::MapFormat(void)
{
	memset(name, 0, sizeof(name));
	memset(description, 0, sizeof(description));
	firstRespawnOffset = -1;
	size = Vector3(100000, 100000, 100000);
}

MapFormat::MapFormat(String filename)
{
	memset(name, 0, sizeof(name));
	memset(description, 0, sizeof(description));
	firstRespawnOffset = -1;
	size = Vector3(100000, 100000, 100000);
	loadFromFile(filename);
}

MapFormat::~MapFormat(void)
{
}

void MapFormat::saveToFile(String filename)
{
	FILE* file = fopen(filename.c_str(), "w");
	if (file)
	{
		json_t* rootObject = json_object();
		json_t* nameJson = json_string(name);
		json_t* descriptionJson = json_string(description);
		json_t* firstRespawnJson = json_integer(firstRespawnOffset);
//		json_t* sizeXjson = json_integer(Math::ICeil(size.x));
//		json_t* sizeXjson = json_integer(Math::ICeil(size.y));
//		json_t* sizeXjson = json_integer(Math::ICeil(size.z));
		int blockSize = blocks.size();
		json_t* blocksJson = json_array();

		json_object_set(rootObject, "name", nameJson);
		json_object_set(rootObject, "description", descriptionJson);
		json_object_set(rootObject, "first-respawn", firstRespawnJson);
		
//		
//		fwrite(name, sizeof(char), sizeof(name), file);
//		fwrite(description, sizeof(char), sizeof(description), file);
////		fwrite(&respawnTime, sizeof(respawnTime), 1, file);
//		fwrite(&firstRespawnOffset, sizeof(firstRespawnOffset), 1, file);
//		fwrite(&size, sizeof(size), 1, file);
//		int a;
//		fwrite(&a, sizeof(a), 1, file);
//
//		int blockSize = blocks.size();
//		fwrite(&(blockSize), sizeof(blockSize), 1, file);
		for (int i = 0; i < blocks.size(); i++)
		{
			Block* block = blocks[i];
			json_t* indBlock = json_object();
			
			json_object_set(indBlock, "extra-args", json_integer(block->extraArgsBit));
			json_object_set(indBlock, "origin-x", json_integer(Math::ICeil(block->origin.x)));
			json_object_set(indBlock, "origin-y", json_integer(Math::ICeil(block->origin.y)));
			json_object_set(indBlock, "origin-z", json_integer(Math::ICeil(block->origin.z)));

			json_object_set(indBlock, "material-index", json_integer(block->materialIndex));
			
			if (block->isPoint)
			{
				json_object_set(indBlock, "is-point", json_boolean(block->isPoint));
			}
			else if (block->isRespawn)
			{
				json_object_set(indBlock, "is-respawn", json_boolean(block->isRespawn));
			}

			if (Math::IFloor(block->rotation.valueDegrees()) != 0)
			{
				json_object_set(indBlock, "rotation", json_integer(Math::IFloor(block->rotation.valueDegrees())));
			}

			if ((block->linearSpeed && Math::IFloor(block->linearLocation.length()) > 0)|| block->rotationSpeed)
			{
				json_t* movableArgs = json_object();
				if (block->linearSpeed > 0 && Math::IFloor(block->linearLocation.length()) > 0)
				{
					json_object_set(movableArgs, "linear-speed", json_integer(block->linearSpeed));
					json_object_set(movableArgs, "linear-location-x", json_integer(Math::ICeil(block->linearLocation.x)));
					json_object_set(movableArgs, "linear-location-y", json_integer(Math::ICeil(block->linearLocation.y)));
					json_object_set(movableArgs, "linear-location-z", json_integer(Math::ICeil(block->linearLocation.z)));
					if (block->isRotationMovement)
					{
						json_object_set(movableArgs, "rotation-movement", json_integer(1));
						json_object_set(movableArgs, "is-rotation-right", json_integer(block->isRightRotationMovement ? 1 : 0));
					}
				}
				if (block->rotationSpeed)
				{
					json_object_set(movableArgs, "rotation-speed", json_integer(Math::ICeil(block->rotationSpeed)));
				}

				json_object_set(indBlock, "movable-args", movableArgs);
			}

			if (block->specialBlockId != -1)
			{
				json_object_set(indBlock, "special-block", json_integer(block->specialBlockId));
			}

			json_array_append(blocksJson, indBlock);
			
		}
		
		json_object_set(rootObject, "blocks", blocksJson);

		json_dumpf(rootObject, file, JSON_INDENT(4));
		
		json_delete(rootObject);
		fclose(file);
	}
}

void MapFormat::loadFromFile(String filename)
{
	this->filename = filename;
	FILE* file = fopen(filename.c_str(), "r");
	if (file)
	{
		json_error_t* error = new json_error_t();
		json_t* root = json_loadf(file, 0, error);
		delete error;
		
		int blockSize = blocks.size();
		json_t* blocksJson = json_array();

//		const char* charBuffer = json_string_value(json_object_get(root, "name"));
//		strcpy_s(name, strlen(charBuffer), charBuffer);
//		charBuffer = json_string_value(json_object_get(root, "description"));
//		strcpy_s(description, strlen(charBuffer), charBuffer);
		firstRespawnOffset = json_integer_value(json_object_get(root, "description"));
		
		json_t* blockArray = json_object_get(root, "blocks");
		int blockCount = json_array_size(blockArray);

		for (int i = 0; i < blockCount; i++)
		{
			Block* block = new Block();
			json_t* indBlock = json_array_get(blockArray, i);
			
			block->extraArgsBit = json_integer_value(json_object_get(indBlock, "extra-args"));//, json_integer(block->extraArgsBit));

			block->origin.x = json_integer_value(json_object_get(indBlock, "origin-x"));
			block->origin.y = json_integer_value(json_object_get(indBlock, "origin-y"));
			block->origin.z = json_integer_value(json_object_get(indBlock, "origin-z"));

			block->materialIndex = (WorldBox::CustomMaterial)json_integer_value(json_object_get(indBlock, "material-index"));

			if ((json_object_get(indBlock, "is-point")))
			{
				block->isPoint = true;
			}
			else if ((json_object_get(indBlock, "is-respawn")))
			{
				block->isRespawn = true;
			}

			if (json_integer_value(json_object_get(indBlock, "rotation")))
			{
				block->rotation = Ogre::Degree(json_integer_value(json_object_get(indBlock, "rotation")));
			}

			if (json_t* movableArgs = json_object_get(indBlock, "movable-args"))
			{
				if (json_object_get(movableArgs, "linear-speed"))//> 0 && Math::IFloor(block->linearLocation.length()) > 0)
				{
					block->linearSpeed = json_integer_value(json_object_get(movableArgs, "linear-speed"));
					block->linearLocation.x = json_integer_value(json_object_get(movableArgs, "linear-location-x"));
					block->linearLocation.y = json_integer_value(json_object_get(movableArgs, "linear-location-y"));
					block->linearLocation.z = json_integer_value(json_object_get(movableArgs, "linear-location-z"));
					if (json_object_get(movableArgs, "rotation-movement") && json_integer_value(json_object_get(movableArgs, "rotation-movement")))
					{
						block->isRightRotationMovement = true;
						block->isRotationMovement = json_integer_value(json_object_get(movableArgs, "is-rotation-right"));
					}
				}

				if (json_object_get(movableArgs, "rotation-speed"))
				{
					block->rotationSpeed = json_integer_value(json_object_get(movableArgs, "rotation-speed"));
				}
			}
			
			if (json_object_get(indBlock, "special-block"))
			{
				block->specialBlockId = json_integer_value(json_object_get(indBlock, "special-block"));
			}

			blocks.push_back(block);
			
		}

		json_delete(root);
		fclose(file);
	}
}

bool MapFormat::validMap(String filename)
{
	return true;
}