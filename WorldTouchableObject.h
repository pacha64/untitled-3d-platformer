#pragma once
#include "stdafx.h"



class WorldTouchableObject
{
public:
	WorldTouchableObject(Vector3 position, Vector3 size);
	virtual ~WorldTouchableObject(void);
	virtual bool isTouchingObject(Vector3& max, Vector3& min);
	void updatePosition(Vector3 position);
protected:
	Vector3 mAabbMin, mAabbMax, position, size;
};

