#include "MapCollection.h"
#include "dirent.h"

MapCollection::MapCollection(void)
{
}

MapCollection::~MapCollection(void)
{
}

MapCollection* MapCollection::getSingleton()
{
	if (singleton == 0)
	{
		singleton = new MapCollection();
	}
	return singleton;
}

void MapCollection::load(String directory)
{
	loadDirectory(directory, rootDirectory);
	/*DIR* dir = opendir(directory.c_str());
	if (dir != 0)
	{
		dirent* dire = 0;
		while ((dire = readdir(dir)) != NULL)
		{
			opendir(dire->d_reclen
			if (dire->d_type == DT_REG)
			{
				int a;
				a = 22;
			}
			else if (dire->d_type == DT_DIR)
			{
				int b;
				b = 22;
			}
		}
		
	}*/
}

bool MapCollection::validDir(MapCollection::MapDirectory& dir)
{
	if (dir.childMaps.size() > 0)
	{
		return true;
	}

	if (dir.childDirectories.size() != 0)
	{
		for (int i = 0; i < dir.childDirectories.size(); i++)
		{
			bool valid = validDir(dir.childDirectories[i]);
			if (valid)
			{
				return true;
			}
		}
	}

	return false;
}

void MapCollection::loadDirectory(String directory, MapDirectory& mapStructure)
{
	DIR* dir = opendir(directory.c_str());
	if (dir != 0)
	{
		dirent* dirent = 0;
		while ((dirent = readdir(dir)) != NULL)
		{
			String name = String(dirent->d_name);
			if (dirent->d_type == DT_DIR && name != "." && name != "..")
			{
				MapDirectory newDirectory;
				loadDirectory(directory + "/" + name, newDirectory);
				
				if (validDir(newDirectory))
				{
					mapStructure.childDirectories.push_back(newDirectory);
				}
			}
			else if (dirent->d_type == DT_REG)
			{
				String extension = String(strrchr(dirent->d_name, '.'));
				if (dirent->d_name && extension == ".map" && (MapFormat::validMap(directory + "/" + String(dirent->d_name))))// &&  WorldGenerator::validJson(
				{
					mapStructure.childMaps.push_back(directory + "/" + dirent->d_name);
				}
			}
		}
	}
}

MapCollection* MapCollection::singleton = 0;