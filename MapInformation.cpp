#include "MapInformation.h"


MapInformation::MapInformation(void)
{
}

btVector3 MapInformation::mapSize(MapSize size)
{
	switch(size)
	{
	case MP_SMALL:
		return btVector3(300, 300, 300);
		break;
	case MP_MEDIUM:
		return btVector3(600, 600, 600);
		break;
	case MP_BIG:
		return btVector3(900, 900, 900);
		break;
	}
}

MapInformation::~MapInformation(void)
{
}
