#pragma once
#include "WorldGenerator.h"
#include "MapFormat.h"


enum MapType
{
	MP_INFINITE_PATH,
	MP_OTHER
};

class MapCollection
{
public:
	struct MapDirectory
	{
		std::vector<MapDirectory> childDirectories;
		std::vector<String> childMaps;
	};
	~MapCollection(void);
	static MapCollection* getSingleton();
	void load(String directory);
	MapDirectory rootDirectory;
	String _helperMap()
	{
		return "resources/maps/editing.map";
		//String filename = rootDirectory.childDirectories[1].childDirectories[0].childMaps[0].filename;
		return rootDirectory.childMaps[0];
	}
private:
	bool validDir(MapDirectory&);
	void loadDirectory(String directory, MapDirectory&);
	MapCollection(void);
	static MapCollection* singleton;
	//std::vector<WorldStructure*> maps;
};

