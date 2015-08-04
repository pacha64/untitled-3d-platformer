#pragma once

#include "stdafx.h"
#include "balance.h"
#include "TextRenderer.h"

class BaseMovementState : public btMotionState
{
public:
	BaseMovementState(SceneNode* node);
	~BaseMovementState(void);
	virtual void getWorldTransform(btTransform& worldTrans) const;
	virtual void setWorldTransform(const btTransform& worldTrans);
protected:
	SceneNode* mNode;
	//btTransform mTransform;
};

