#include "MovablePlatformMotionState.h"
#include "BtOgreGP.h"

MovablePlatformMotionState::MovablePlatformMotionState(SceneNode* node)
	:BaseMovementState(node)
{
}


MovablePlatformMotionState::~MovablePlatformMotionState(void)
{
}

void MovablePlatformMotionState::getWorldTransform(btTransform& worldTrans) const
{
	/*worldTrans.setIdentity();
	worldTrans.setOrigin(BtOgre::Convert::toBullet(mNode->getPosition()));
	worldTrans.setRotation(BtOgre::Convert::toBullet(mNode->getOrientation()));*/
}
void MovablePlatformMotionState::setWorldTransform(const btTransform& worldTrans)
{

    btQuaternion rot = worldTrans.getRotation();
	Quaternion quat(rot.w(), rot.x(), rot.y(), rot.z());
    mNode->setOrientation(quat);
    btVector3 pos = worldTrans.getOrigin();
    mNode->setPosition(pos.x(), pos.y(), pos.z());
}