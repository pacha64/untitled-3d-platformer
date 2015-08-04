#pragma once
#include "stdafx.h"



class MapInformation
{
public:
	enum Direction : int
	{
		NORTH = 0,
		NORTH_WEST,
		WEST,
		SOUTH_WEST,
		SOUTH,
		SOUTH_EAST,
		EAST,
		NORTH_EAST
	};
	enum MapSize
	{
		MP_SMALL,
		MP_MEDIUM,
		MP_BIG
	};
	MapInformation();
	~MapInformation();
	static btVector3 mapSize(MapSize size);
	static Radian direction2radian(Direction direction)
	{
		return Radian(Degree(45) * direction);
	}
};
