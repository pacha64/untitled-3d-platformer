#include "BaseMovementState.h"
#include "BtOgreGP.h"

BaseMovementState::BaseMovementState(SceneNode* node)
{
	mNode = node;
//	mTransform = btTransform(BtOgre::Convert::toBullet(node->_getDerivedOrientation()), BtOgre::Convert::toBullet(node->_getDerivedPosition()));
}

BaseMovementState::~BaseMovementState(void)
{
}

void BaseMovementState::getWorldTransform(btTransform& worldTrans) const
{
	//worldTrans = mTransform;
}
void BaseMovementState::setWorldTransform(const btTransform& worldTrans)
{
	//mTransform = worldTrans;
    btQuaternion rot = worldTrans.getRotation();
	Quaternion quat(float(rot.w()), float(rot.x()), float(rot.y()), float(rot.z()));
    mNode->setOrientation(quat);
    btVector3 pos = worldTrans.getOrigin();
    mNode->setPosition(pos.x(), pos.y(), pos.z());
}
