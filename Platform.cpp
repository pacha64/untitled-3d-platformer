#include "Platform.h"
#include "Physics.h"

Platform::Platform(WorldBox* box)
	: mBox(box),
	mActivator(0),
	mIsBreaking(false),
	mIsBreakable(false),
	mIsBroken(false),
	mIsKillBox(false),
	mTopKillBox(false),
	mIsIce(false),
	mTimeSinceStartedBreaking(0.0),
	mBreakingParticleEffectNode(0)
{
	mBox->getBody()->platformPointer = this;
}


Platform::~Platform(void)
{
	if (mActivator)
	{
		delete mActivator;
	}
	if (mBox)
	{
		delete mBox;
	}
}

Quaternion Platform::orientationGrabbingPointer(btVector3& start, btVector3& to)
{
	btScalar distanceHelper = PLAYER_BODY_X + PLAYER_WALL_GRAB_OFFSET_Z;
	distanceHelper *= 1;
	btCollisionWorld::ClosestRayResultCallback result((start), start + to * distanceHelper);
	Physics::getSingleton()->getWorld()->rayTest((start), start + to * distanceHelper, result);
	if (result.hasHit())
	{
		return Quaternion(Math::ATan2(result.m_hitNormalWorld.x(), result.m_hitNormalWorld.z()) - Degree(180), Vector3::UNIT_Y);
		//btScalar distance = result.m_hitPointWorld.distance(start);
		//Platform* platformPtr = ((Platform*)result.m_collisionObject->platformPointer);
		//if (platformPtr == 0) return 0;
		//double yMaxPlatform = platformPtr->getBox()->getYmax();
		//if (Math::Abs(start.y() - yMaxPlatform) <= PLAYER_WALL_GRAB_OFFSET_Y)
		//{
		//	return platformPtr;
		//}
		//result.m_collisionObject
	}
	else
	{
		return Quaternion(Degree(0), Vector3::UNIT_Y);
	}
}

int Platform::getShaderColorType()
{
	if (isBreakable())
	{
		return ShaderColors::SC_BREAKABLE;
	}
	if (isKillBox())
	{
		return SC_KILLBOX;
	}
	if (isTopKillBox())
	{
		if (Type() == PlatformType::PT_MOVABLE)
		{
			return SC_MOVABLE_KILLBOX_TOP;
		}
		else
		{
			return SC_KILLBOX_TOP;
		}
	}
	if (Type() == PlatformType::PT_MOVABLE)
	{
		return SC_MOVABLE;
	}

	return SC_COMMON;
}

void Platform::update(const FrameEvent& evt)
{
	if (mBox->getEntity())
	{
		auto size = mBox->size();
		mBox->getEntity()->getSubEntity(0)->setCustomParameter(0, Vector4(size.x, size.y, size.z, 0));
	}
	updateBreakble(evt);
}

void Platform::updateBreakble(const FrameEvent& evt)
{
	if (mIsBreakable)
	{
		if (mIsBreaking)
		{
			mTimeSinceStartedBreaking += evt.timeSinceLastFrame;
			if (!mIsBroken)
			{
				if (mTimeSinceStartedBreaking >= BREAKABLE_PLATFORM_TIME_TO_BREAK)
				{
					mIsBroken = true;
					mBox->setEnabled(false);
					mBreakingParticleEffectNode = Root::getSingleton().getSceneManager(SCENE_MANAGER_NAME)->getRootSceneNode()->createChildSceneNode((mBox->getNode()->_getDerivedPosition()));
					ParticleSystem* particleSystem = Root::getSingleton().getSceneManager(SCENE_MANAGER_NAME)->createParticleSystem(requestNewParticleName(), "Platform/Shattering");
					//dynamic_cast<BoxEmitter*>(particleSystem->getEmitter(0))->setSize(Vector3(10000, 10000, 10000));
					//dynamic_cast<BoxEmitter*>(particleSystem->getEmitter(0))->_notifyOwner(particleSystem);

					mBreakingParticleEffectNode->attachObject(particleSystem);
					
					dynamic_cast<BoxEmitter*>(particleSystem->getEmitter(0))->setEmissionRate(
						(float)(mBox->size().x * mBox->size().y * mBox->size().z) *
						dynamic_cast<BoxEmitter*>(particleSystem->getEmitter(0))->getEmissionRate() /
						(PLATFORM_SIZE_UNIT * PLATFORM_SIZE_UNIT * PLATFORM_SIZE_UNIT));
					mBreakingParticleEffectNode->scale(mBox->size());
					mBreakingParticleEffectNode->setOrientation(mBox->getNode()->getOrientation());
				}
			}
			else
			{
				if (mTimeSinceStartedBreaking >= BREAKABLE_PLATFORM_TIME_TO_BREAK + BREAKABLE_PLATFORM_TIME_TO_RECREATE)
				{
					rebuildBreak();
				}
			}
		}
	}
}

void Platform::rebuildBreak()
{
	mTimeSinceStartedBreaking = 0.0;
	mIsBreaking = false;
	mIsBroken = false;
	mBox->setEnabled(true);
}

void Platform::addActivator(WorldActivator::ActivatorType type)
{
	if (mActivator)
	{
		_changeActivatorType(type);
	}
	else
	{
		mActivator = new WorldActivator(type, this);
	}
}

void Platform::_setPosition(Vector3 position)
{
	if (getBox()->getNode())
	{
		btTransform newTransf = mBox->getBody()->getWorldTransform();
		newTransf.setOrigin(TO_BULLET(position));
		mBox->getBody()->setWorldTransform(newTransf);

		mBox->getNode()->setOrientation(TO_OGRE(mBox->getBody()->getWorldTransform().getRotation()));
		mBox->getNode()->setPosition(TO_OGRE(mBox->getBody()->getWorldTransform().getOrigin()));
	}
}

void Platform::_setRotate(Degree rotate)
{
	if (getBox()->getNode())
	{
		btTransform newTransf = mBox->getBody()->getWorldTransform();
		newTransf.setRotation(btQuaternion(btVector3(0, 1, 0), rotate.valueRadians()));// TO_BULLET(position));
		mBox->getBody()->setWorldTransform(newTransf);

		mBox->getNode()->setOrientation(TO_OGRE(mBox->getBody()->getWorldTransform().getRotation()));
		mBox->getNode()->setPosition(TO_OGRE(mBox->getBody()->getWorldTransform().getOrigin()));
	}
}

void Platform::_setBreakableFlag(bool breakable)
{
	if (breakable)
	{
		setBreakable(true);
	}
	else
	{
		setBreakable(false);
	}
}

void Platform::_setKillFlag(bool killFlag)
{
	if (killFlag)
	{
//		setKillBox(true);
	}
	else
	{
//		setKillBox(false);
	}
}

void Platform::_translate(Vector3 translate)
{
	_setPosition(TO_OGRE(mBox->getBody()->getWorldTransform().getOrigin()) + translate);
}

//void Platform::_addSize(Vector3 size)
//{
//	for (int i = 0; i < 3; i++)
//	{
//		if (size[i] < 0) size[i] = -1; else if (size[i] > 0) size[i] = 1; else size[i] = 0;
//	}
//
//	Vector3 newSize = mBox->size() + size * INCREASE_SIZE_UNIT;
//	if (mActivator)
//	{
//		Real difference = mBox->size().y - newSize.y;
//		
//		difference /= 2;
//		getBox()->getNode()->removeChild(mActivator->_getNode());
//		mActivator->_translateY(difference);
//	}
//
//	btTransform transf = mBox->getBody()->getWorldTransform();
//	WorldBox::CustomMaterial mat = mBox->getMaterial();
//	delete mBox;
//	mBox = new WorldBox(0, transf, mat, false);
//	mBox->getBody()->platformPointer = this;
//	if (mActivator)
//	{
//		mBox->getNode()->addChild(mActivator->_getNode());
//	}
//}
void Platform::_changePlatformType(WorldBox::CustomMaterial cm)
{
	if (mActivator)
	{
		Real difference = WorldBox::typeSize(cm).y - WorldBox::typeSize(getBox()->getMaterial()).y;
		
		difference /= 2;
		getBox()->getNode()->removeChild(mActivator->_getNode());
		mActivator->_translateY(difference);
	}

	btTransform transf = mBox->getBody()->getWorldTransform();
	delete mBox;
	mBox = new WorldBox(0, transf, cm, false);
	mBox->getBody()->platformPointer = this;

	if (WorldBox::materialIsBreakable(cm))
	{
		_setBreakableFlag(true);
	}
	else
	{
		_setBreakableFlag(false);
	}
	
	if (WorldBox::materialIsKillbox(cm))
	{
		_setKillFlag(true);
	}
	else
	{
		_setKillFlag(false);
	}

	if (mActivator)
	{
		mBox->getNode()->addChild(mActivator->_getNode());
	}
}

void Platform::_changeActivatorType(WorldActivator::ActivatorType type)
{
	if (!mActivator)
	{
		addActivator(type);
	}
	else
	{
		mActivator->_changeType(type);
	}
}

void Platform::_removeActivator()
{
	if (mActivator)
	{
		delete mActivator;
		mActivator = 0;
	}
}