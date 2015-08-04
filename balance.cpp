#include "balance.h"
#include "MapFormat.h"
#include <ctime>

String requestNewStaticGeometryName()
{
	static int counter = 0;
	return StringConverter::toString(++counter) + "-static-geometry";
}

String requestNewParticleName()
{
	static int counter = 0;
	return StringConverter::toString(++counter) + "-particle-name";
}

String requestNewPlatformBorderName()
{
	static int counter = 0;
	return StringConverter::toString(++counter) + "-platform-border";
}

String requestNewRandomMaterialName()
{
	static int counter = 0;
	return StringConverter::toString(++counter) + "-material-pointer";
}

String requestNewEntityName()
{
	static int counter = 0;
	return StringConverter::toString(++counter) + "-entity";
}

String timeToString(int seconds)
{
	String toReturn = "";
	int helper = 0;
	if (seconds >= 60 * 60)
	{
		helper = (seconds / (60 * 60)) % 60;
		toReturn += StringConverter::toString(helper) + ":";
		helper = (seconds / 60) % 60;
		toReturn += StringConverter::toString(helper) + ":";
		helper = seconds % 60;
		if (helper < 10)
		{
			toReturn += "0" + StringConverter::toString(helper);
		}
		else
		{
			toReturn += StringConverter::toString(helper);
		}
	}
	else
	{
		helper = (seconds / 60) % 60;
		toReturn += StringConverter::toString(helper) + ":";
		helper = seconds % 60;
		if (helper < 10)
		{
			toReturn += "0" + StringConverter::toString(helper);
		}
		else
		{
			toReturn += StringConverter::toString(helper);
		}
	}
	return toReturn;
}

String generateWindowTitle(int fps)
{
	return String("Platforming demo - ~") + StringConverter::toString(fps) + " FPS (capped at 60 FPS)";
}

/*bool normalIsStandableSlope(btVector3& normal)
{
	Degree slope = Vector3::NEGATIVE_UNIT_Y.angleBetween(TO_OGRE(normal));
	if (slope >= Degree(180 - MAX_SLOPE - 1))
	{
		return true;
	}
	else
	{
		return false;
	}
}*/

bool normalIsStandable(const btVector3& normal)
{
	if (normal.length() == 0)
	{
		return false;
	}
	auto dot = Vector3::UNIT_Y.dotProduct(TO_OGRE(normal));
	return Math::ACos(dot / normal.length()).valueAngleUnits() <= PLAYER_MAX_SLOPE;
}

bool normalValidWallJump(const btVector3& normal)
{
	btVector3 newVector = normal;
	newVector.setY(0);
	newVector.normalize();
	Degree slope = Vector3::UNIT_Y.angleBetween(TO_OGRE(normal));
	if (slope >= Degree(180 - PLAYER_MAX_SLOPE_TOP_WALL_JUMP + 1 /* margin */))
	{
		return false;
	}
	
	return true;
}

Real fixAngleNumber(Real angle)
{
	if (angle < 0)
	{
		angle = (180 - Math::Abs(angle) + 180);
	}

	return angle;
}

Vector3 reflectVector3(Vector3 reflector, Vector3 toReflect)
{
	return 2 * (reflector.dotProduct((toReflect))) * (toReflect) - reflector;
}

MapFormat* getRandomStartMenuMap()
{
	return new MapFormat("resources/maps/editing.map");
}

MapFormat* getTutorialMap(int number)
{
	return new MapFormat("resources/maps/editing.map");
}