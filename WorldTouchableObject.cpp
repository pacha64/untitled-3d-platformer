#include "WorldTouchableObject.h"


WorldTouchableObject::WorldTouchableObject(Vector3 position, Vector3 size)
	: position(position),
	size(size)
{
	mAabbMin = position;
	mAabbMax = position;
	mAabbMin -= size / 2;
	mAabbMax += size / 2;
}


WorldTouchableObject::~WorldTouchableObject(void)
{
}

void WorldTouchableObject::updatePosition(Vector3 position)
{
	this->position = position;
	mAabbMin = position;
	mAabbMax = position;
	mAabbMin -= size / 2;
	mAabbMax += size / 2;
}

bool WorldTouchableObject::isTouchingObject(Vector3& aabbMax, Vector3& aabbMin)
{
  //return (a.max_x() >= b.min_x() and a.min_x() <= b.max_x())
  //   and (a.max_y() >= b.min_y() and a.min_y() <= b.max_y())
  //   and (a.max_z() >= b.min_z() and a.min_z() <= b.max_z())
	if ((mAabbMax.x >= aabbMin.x && mAabbMin.y <= aabbMax.y) &&
		(mAabbMax.z >= aabbMin.z && mAabbMin.x <= aabbMax.x) &&
		(mAabbMax.y >= aabbMin.y && mAabbMin.z <= aabbMax.z))
	{
		return true;
	}
	else
	{
		return false;
	}
}