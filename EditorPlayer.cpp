#include "EditorPlayer.h"
#include "Globals.h"
#include "DebugDrawer.h"
#include "GameGUI.h"

#define KEYSTROKE(x) (keyboard->isKeyDown(x) && !mKeystateOld[x])
#define SHIFT_DOWN (keyboard->isKeyDown(OIS::KC_LSHIFT) || keyboard->isKeyDown(OIS::KC_RSHIFT))
#define CONTROL_DOWN (keyboard->isKeyDown(OIS::KC_LCONTROL) || keyboard->isKeyDown(OIS::KC_RCONTROL))
#define ALT_DOWN (keyboard->isKeyDown(OIS::KC_LMENU) || keyboard->isKeyDown(OIS::KC_RMENU))
#define CLICK(x) (currentMouseState.buttonDown(x) && !mOldMouseState.buttonDown(x))

#define SET_X_ABSOLUTE "Set X"
#define SET_Y_ABSOLUTE "Set Y"
#define SET_Z_ABSOLUTE "Set Z"
#define SET_X_RELATIVE "Add to X"
#define SET_Y_RELATIVE "Add to Y"
#define SET_Z_RELATIVE "Add to Z"
#define SET_ROTATION_ABSOLUTE "Set rotation"
#define SET_ROTATION_RELATIVE "Add to rotation"
#define SET_ROTATION_SPEED_RELATIVE "Add to rotation speed"
#define SET_ROTATION_SPEED_ABSOLUTE "Set rotation speed"
#define SET_LINEAR_SPEED_RELATIVE "Add to linear speed"
#define SET_LINEAR_SPEED_ABSOLUTE "Set linear speed"

#define SET_MARGIN "Set margin"

void EditorPlayer::Matrix3D::refreshPosition()
{
	for (int y = 0; y < maxY; y++)
	{
		for (int z = 0; z < maxZ; z++)
		{
			for (int x = 0; x < maxX; x++)
			{
				Vector3 helper = Vector3(
					(margin) * x - (margin * (maxX - 1.0)) / 2,
					(margin) * y - (margin * (maxY - 1.0)) / 2,
					(margin) * z - (margin * (maxZ - 1.0)) / 2
				);
				(*this)(x, y, z)->_setPosition(origin + helper);
			}
		}
	}
}

void EditorPlayer::Matrix3D::refreshNumber()
{
	//if (vector.size() > maxX * maxY * maxZ)
	//{
	//	for (int i = maxX * maxY * maxZ; i < vector.size(); i++)
	//	{
	//		if (vector[i]) editor->deletePlatform(vector[i]);
	//	}

	//	vector.resize(maxX * maxY * maxZ);
	//}
	//else if (vector.size() < maxX * maxY * maxZ)
	//{
	//	for (int i = vector.size(); i < maxX * maxY * maxZ; i++)
	//	{
	//		vector.push_back(editor->newPlatform(
	//			origin,
	//			rotation,
	//			type
	//		));
	//	}
	//}
}

bool EditorPlayer::Matrix3D::canAddMorePlatforms(int axis)
{
	int x = maxX, y = maxY, z = maxZ;
	switch (axis)
	{
	case 0:
		x++;
		break;
	case 1:
		y++;
		break;
	case 2:
		z++;
		break;
	}
	int difference = x * y * z - maxX * maxY* maxZ;
	if (editor->getAmountOfPlatforms() + difference <= MAX_PLATFORMS_NUM) return true; else return false;
}

void EditorPlayer::Path::appendPlatform()
{
	previousRotation = currentRotation;
//	platforms.push_back(EditorPlayer::Matrix3D::editor->newPlatform(platforms[platforms.size() - 1]->getBox()->getNode()->_getDerivedPosition() + (Quaternion(previousRotation, Vector3::UNIT_Y) * Vector3(0, currentHeight, margin)), previousRotation, type));
	currentHeight = height;
	currentMargin = margin;
}

void EditorPlayer::Path::setRotation(Degree rotation)
{
	currentRotation = rotation;
	if (platforms.size() > 1)
	{
		platforms[platforms.size() - 1]->_setPosition(platforms[platforms.size() - 2]->getBox()->getNode()->_getDerivedPosition() + (Quaternion(currentRotation, Vector3::UNIT_Y) * Vector3(0, currentHeight, margin)));
	}

	platforms[platforms.size() - 1]->_setRotate(currentRotation);
}

void EditorPlayer::Path::setMargin(int margin)
{
	int difference = (margin - currentMargin);
	currentMargin = margin;
	platforms[platforms.size() - 1]->_translate(Quaternion(currentRotation, Vector3::UNIT_Y) * Vector3(0, 0, difference));
}

void EditorPlayer::Path::setHeight(int height)
{
	int difference = (height - currentHeight);
	currentHeight = height;
	platforms[platforms.size() - 1]->_translate(Vector3(0, difference, 0));
}

void EditorPlayer::Path::setDefaultMargin(int margin)
{
	this->margin = margin;
	setMargin(margin);
}

void EditorPlayer::Path::setDefaultHeight(int height)
{
	this->height = height;
	setHeight(height);
}

EditorPlayer::EditorPlayer(Player* playerInstance, GameGUI* gameGUI, Camera* camera, SceneManager* sceneManager, WorldEditor* editor)
	: mEditor(editor),
	mLockX(false),
	mLockY(false),
	mLockZ(false),
	mModifyingSpeed(false),
	mMovementAbsolute(false),
	mModifyingOrigin(false),
	mCamera(camera),
	mLockNumber(0),
	mRotate(false),
	mAddingMatrix(false),
	mAddingPath(false),
	mMatrix(0),
	mPath(0),
	mGameGUI(gameGUI),
	mPlayerInstance(playerInstance)
{
	Matrix3D::editor = editor;
	mMainNode = sceneManager->getRootSceneNode()->createChildSceneNode();
	mCameraPivot = mMainNode->createChildSceneNode();
	mCameraYaw = mCameraPivot->createChildSceneNode();
	mCameraPitch = mCameraYaw ->createChildSceneNode();
	mCameraHolder = mCameraPitch->createChildSceneNode();
	
	//mCameraHolder->attachObject(mCamera);
	onPlayerToggle();

	changeCameraFreeRoaming();

	memset(mKeystateOld, 0, sizeof(char) * 256);
}

EditorPlayer::~EditorPlayer(void)
{
}

void EditorPlayer::update(const FrameEvent& evt, OIS::Keyboard* keyboard, OIS::Mouse* mouse)
{
	const Real translatePerKeystroke = .5;
	mGameGUI->clearAllEditorGUI();

	char keystateNew[256];
	OIS::MouseState& currentMouseState = const_cast<OIS::MouseState&>(mouse->getMouseState());
	Keys* keysSingleton = Keys::singleton;
	keyboard->copyKeyStates(keystateNew);

	if (mCameraRoaming == 0)
	{
		mCameraPitch->pitch(Degree((_::options.invertY ? -1 : 1) * (CAMERA_SPEED_MIN + _::options.mouseSensitivity * CAMERA_SPEED_MAX_INCREASE))*evt.timeSinceLastFrame*currentMouseState.Y.rel);
		mCameraYaw->yaw(Degree((_::options.invertX ? -1 : 1) * (CAMERA_SPEED_MIN + _::options.mouseSensitivity * CAMERA_SPEED_MAX_INCREASE))*evt.timeSinceLastFrame*currentMouseState.X.rel);
		if (mCameraPitch->getOrientation().getPitch().valueAngleUnits() < -90)
		{
			mCameraPitch->pitch(-Degree((_::options.invertY ? -1 : 1) * (CAMERA_SPEED_MIN + _::options.mouseSensitivity * CAMERA_SPEED_MAX_INCREASE))*evt.timeSinceLastFrame*currentMouseState.Y.rel);
		}
		if (mCameraPitch->getOrientation().getPitch().valueAngleUnits() > 90)
		{
			mCameraPitch->pitch(-Degree((_::options.invertY ? -1 : 1) * (CAMERA_SPEED_MIN + _::options.mouseSensitivity * CAMERA_SPEED_MAX_INCREASE))*evt.timeSinceLastFrame*currentMouseState.Y.rel);
		}

		Vector3 translateVector = Vector3::ZERO;
		if (keyboard->isKeyDown(OIS::KC_W))
		{
			translateVector += Vector3(0, 0, -CAMERA_CONTROLLER_SPEED * evt.timeSinceLastFrame);
		}
		if (keyboard->isKeyDown(OIS::KC_S))
		{
			translateVector += Vector3(0, 0, CAMERA_CONTROLLER_SPEED * evt.timeSinceLastFrame);
		}
		if (keyboard->isKeyDown(OIS::KC_D))
		{
			translateVector += Vector3(CAMERA_CONTROLLER_SPEED * evt.timeSinceLastFrame, 0, 0);
		}
		if (keyboard->isKeyDown(OIS::KC_A))
		{
			translateVector += Vector3(-CAMERA_CONTROLLER_SPEED * evt.timeSinceLastFrame, 0, 0);
		}


		if (keyboard->isKeyDown(OIS::KC_E))
		{
			translateVector += Vector3(0, CAMERA_CONTROLLER_SPEED * evt.timeSinceLastFrame, 0);
		}
		if (keyboard->isKeyDown(OIS::KC_Q))
		{
			translateVector += Vector3(0, -CAMERA_CONTROLLER_SPEED * evt.timeSinceLastFrame, 0);
		}

		if (translateVector.length() != 0)
		{
			auto increaseHelper = 1;
			if (SHIFT_DOWN)
			{
				increaseHelper *= 10;
			}
			translateVector = translateVector.normalisedCopy() * CAMERA_CONTROLLER_SPEED * increaseHelper * evt.timeSinceLastFrame;
			translateVector = Quaternion(mCameraHolder->_getDerivedOrientation().getYaw(), Vector3::UNIT_Y) * translateVector;
			mMainNode->translate(translateVector);
		}

	}
	else if (mCameraRoaming == 1)
	{
		Vector3 posCurrent = mCameraHolder->getPosition();
		if (keysSingleton->Wheel != 0)
		{
			posCurrent.z -= CAMERA_ZOOM_DISTANCE * (keysSingleton->Wheel > 0 ? 1 : -1);

			mCameraHolder->setPosition(posCurrent);
		}

		mCameraPitch->pitch(Degree((_::options.invertY ? -1 : 1) * (CAMERA_SPEED_MIN + _::options.mouseSensitivity * CAMERA_SPEED_MAX_INCREASE))*evt.timeSinceLastFrame*keysSingleton->MouseY);
		mCameraYaw->yaw(Degree((_::options.invertX ? -1 : 1) * (CAMERA_SPEED_MIN + _::options.mouseSensitivity * CAMERA_SPEED_MAX_INCREASE))*evt.timeSinceLastFrame*keysSingleton->MouseX);
		if (mCameraPitch->getOrientation().getPitch().valueAngleUnits() < -90)
		{
			mCameraPitch->pitch(-Degree((_::options.invertY ? -1 : 1) * (CAMERA_SPEED_MIN + _::options.mouseSensitivity * CAMERA_SPEED_MAX_INCREASE))*evt.timeSinceLastFrame*keysSingleton->MouseY);
		}
		if (mCameraPitch->getOrientation().getPitch().valueAngleUnits() > 90)
		{
			mCameraPitch->pitch(-Degree((_::options.invertY ? -1 : 1) * (CAMERA_SPEED_MIN + _::options.mouseSensitivity * CAMERA_SPEED_MAX_INCREASE))*evt.timeSinceLastFrame*keysSingleton->MouseY);
		}

		/*if (mCurrentModifyingPlatform.size() > 0)
		{
			mMainNode->setPosition(mCurrentModifyingPlatform->getBox()->getNode()->_getDerivedPosition());
		}*/
	} 
	
	Platform* platformHelper = 0;

	if (mAddingMatrix)
	{
		if (mAddingStep == 0)
		{
			mGameGUI->setEditorBottom1(SET_X_ABSOLUTE);
			updateKeyboardNumber(keystateNew, mKeystateOld, mLockNumber);

			if (KEYSTROKE(OIS::KC_RETURN))
			{
				mAddingStep++;
				mAddingVectorHelper.x = mLockNumber;
				mLockNumber = 0;
			}
		}
		else if (mAddingStep == 1)
		{
			mGameGUI->setEditorBottom1(SET_Y_ABSOLUTE);
			updateKeyboardNumber(keystateNew, mKeystateOld, mLockNumber);

			if (KEYSTROKE(OIS::KC_RETURN))
			{
				mAddingStep++;
				mAddingVectorHelper.y = mLockNumber;
				mLockNumber = 0;
			}
		}
		else if (mAddingStep == 2)
		{
			mGameGUI->setEditorBottom1(SET_Z_ABSOLUTE);
			updateKeyboardNumber(keystateNew, mKeystateOld, mLockNumber);

			if (KEYSTROKE(OIS::KC_RETURN))// && !mKeystateOld[OIS::KC_RETURN])
			{
				mAddingStep++;
				mAddingVectorHelper.z = mLockNumber;
				mLockNumber = 0;
			}
		}
		else if (mAddingStep == 3)
		{
			mGameGUI->setEditorBottom1(SET_MARGIN);
			updateKeyboardNumber(keystateNew, mKeystateOld, mLockNumber);

			if (KEYSTROKE(OIS::KC_RETURN))// && !mKeystateOld[OIS::KC_RETURN])
			{
				mAddingStep++;
			}
		}
		else
		{
			if (!mMatrix)
			{
				mMatrix = new Matrix3D(mAddingVectorHelper, mLockNumber);
				mLockNumber = 0;
				changeCameraOrbit(mAddingVectorHelper, CAMERA_ZOOM_DEFAULT);
				mAddingVectorHelper = Vector3::ZERO;
				mLockX = false;
				mLockY = false;
				mLockZ = false;
				mRotate = false;
				mModifyingOrigin = false;
				mMovementAbsolute = false;
			}

			if (mAddingVectorHelper.length() != 0)
			{
				mMatrix->setOrigin(mAddingVectorHelper);
				changeCameraOrbit(mAddingVectorHelper, mCameraHolder->getPosition().z);
				mAddingVectorHelper = Vector3::ZERO;
			}

			if (KEYSTROKE(OIS::KC_X))// && !mKeystateOld[OIS::KC_X])
			{
				mLockX = true;
				mLockY = false;
				mLockZ = false;
				mRotate = false;
				mModifyingOrigin = false;
				mMovementAbsolute = false;
				mLockNumber = 0;
			}

#if ENABLE_EDITOR_MATRIX_HEIGHT
			if (KEYSTROKE(OIS::KC_Y))// && !mKeystateOld[OIS::KC_Y])
			{
				mLockX = false;
				mLockY = true;
				mLockZ = false;
				mRotate = false;
				mModifyingOrigin = false;
				mMovementAbsolute = false;
				mLockNumber = 0;
			}
#endif
					
			if (KEYSTROKE(OIS::KC_Z))// && !mKeystateOld[OIS::KC_Z])
			{
				mLockX = false;
				mLockY = false;
				mLockZ = true;
				mRotate = false;
				mModifyingOrigin = false;
				mMovementAbsolute = false;
				mLockNumber = 0;
			}
					
			if (KEYSTROKE(OIS::KC_R))// && !mKeystateOld[OIS::KC_R])
			{
				mLockX = false;
				mLockY = false;
				mLockZ = false;
				mRotate = true;
				mModifyingOrigin = false;
				mMovementAbsolute = false;
				mLockNumber = mMatrix->getRotation().valueDegrees();
			}
					
			if (KEYSTROKE(OIS::KC_O))// && !mKeystateOld[OIS::KC_O])
			{
				mLockX = false;
				mLockY = false;
				mLockZ = false;
				mRotate = false;
				mModifyingOrigin = true;
				mMovementAbsolute = false;
				mLockNumber = 0;
				mAddingStep = 0;
			}
					
			if (KEYSTROKE(OIS::KC_M))// && !mKeystateOld[OIS::KC_M])
			{
				mLockX = false;
				mLockY = false;
				mLockZ = false;
				mRotate = false;
				mModifyingOrigin = false;
				mMovementAbsolute = true;
				mLockNumber = 0;
			}

			if (KEYSTROKE(OIS::KC_TAB) && !ALT_DOWN)// && !mKeystateOld[OIS::KC_TAB])
			{
				// TODO change size
				//short type = mMatrix->type;
				//if (keyboard->isKeyDown(OIS::KC_LSHIFT) || keyboard->isKeyDown(OIS::KC_RSHIFT))
				//{
				//	type--;
				//}
				//else
				//{
				//	type++;
				//}
				//if (type == WorldBox::BT_SIZE)
				//{
				//	type = 0;
				//}
				//if (type == -1)
				//{
				//	type = WorldBox::BT_SIZE - 1;
				//}
				//mMatrix->setType((WorldBox::BoxType)type);
			}

			if (mLockX || mLockY || mLockZ)
			{
				if (KEYSTROKE(OIS::KC_LEFT))// && !mKeystateOld[OIS::KC_LEFT])
				{
					if (mLockX)
					{
						mMatrix->removeX();
					}
					else if (mLockY)
					{
						mMatrix->removeY();
					}
					else if (mLockZ)
					{
						mMatrix->removeZ();
					}
				}
				else if (KEYSTROKE(OIS::KC_RIGHT))// && !mKeystateOld[OIS::KC_RIGHT])
				{
					if (mLockX)
					{
						if (mMatrix->canAddMorePlatforms(0)) mMatrix->addX();
					}
					else if (mLockY)
					{
						if (mMatrix->canAddMorePlatforms(1)) mMatrix->addY();
					}
					else if (mLockZ)
					{
						if (mMatrix->canAddMorePlatforms(2)) mMatrix->addZ();
					}
				}

				if (mLockX)
				{
					mGameGUI->setEditorBottom1("Press left or right to add to X");
				}
				else if (mLockY)
				{
					mGameGUI->setEditorBottom1("Press left or right to add to Y");
				}
				else if (mLockZ)
				{
					mGameGUI->setEditorBottom1("Press left or right to add to Z");
				}

				if (KEYSTROKE(OIS::KC_RETURN))// && !mKeystateOld[OIS::KC_RETURN])
				{
					mLockX = 0;
					mLockZ = 0;
					mLockY = 0;
				}
			}
			else if (mRotate)
			{
				updateKeyboardNumber(keystateNew, mKeystateOld, mLockNumber);
				mMatrix->setRotation(Degree(mLockNumber));

				if (KEYSTROKE(OIS::KC_RETURN))// && !mKeystateOld[OIS::KC_RETURN])
				{
					mRotate = false;
				}
			}
			else if (mMovementAbsolute)
			{
				updateKeyboardNumber(keystateNew, mKeystateOld, mLockNumber);
				mMatrix->setMargin((mLockNumber));

				if (KEYSTROKE(OIS::KC_RETURN))// && !mKeystateOld[OIS::KC_RETURN])
				{
					mMovementAbsolute = false;
				}
			}
			else if (mModifyingOrigin)
			{
				mAddingStep = 0;
				mLockNumber = 0;
			}
			else
			{
				if (CLICK(OIS::MouseButtonID::MB_Right))
				{
					mMatrix->deleteAllPlatforms();
					mAddingMatrix = false;
					mCurrentModifyingPlatform.clear();
				}
				else if ((CLICK(OIS::MouseButtonID::MB_Left)))
				{
					mAddingMatrix = false;
					mCurrentModifyingPlatform.clear();
				}
			}
		}
	}
	else if (mAddingPath)
	{
		if (mAddingStep == 0)
		{
			mGameGUI->setEditorBottom1(SET_X_ABSOLUTE);
			updateKeyboardNumber(keystateNew, mKeystateOld, mLockNumber);

			if (KEYSTROKE(OIS::KC_RETURN))// && !mKeystateOld[OIS::KC_RETURN])
			{
				mAddingStep++;
				mAddingVectorHelper.x = mLockNumber;
				mLockNumber = 0;
			}
		}
		else if (mAddingStep == 1)
		{
			mGameGUI->setEditorBottom1(SET_Y_ABSOLUTE);
			updateKeyboardNumber(keystateNew, mKeystateOld, mLockNumber);

			if (KEYSTROKE(OIS::KC_RETURN))// && !mKeystateOld[OIS::KC_RETURN])
			{
				mAddingStep++;
				mAddingVectorHelper.y = mLockNumber;
				mLockNumber = 0;
			}
		}
		else if (mAddingStep == 2)
		{
			mGameGUI->setEditorBottom1(SET_Z_ABSOLUTE);
			updateKeyboardNumber(keystateNew, mKeystateOld, mLockNumber);

			if (keyboard->isKeyDown(OIS::KC_RETURN) && !mKeystateOld[OIS::KC_RETURN])
			{
				mAddingStep++;
				mAddingVectorHelper.z = mLockNumber;
				mLockNumber = 0;
			}
		}
		else if (mAddingStep == 3)
		{
			mGameGUI->setEditorBottom1(SET_MARGIN);
			updateKeyboardNumber(keystateNew, mKeystateOld, mLockNumber);

			if (KEYSTROKE(OIS::KC_RETURN))// && !mKeystateOld[OIS::KC_RETURN])
			{
				mAddingStep++;
				mAddingVectorHelper2.x = mLockNumber;
				mLockNumber = 0;
			}
		}
		/*else if (mAddingStep == 4)
		{
			updateKeyboardNumber(keystateNew, mKeystateOld, mLockNumber);

			if (KEYSTROKE(OIS::KC_RETURN))// && !mKeystateOld[OIS::KC_RETURN])
			{
				mAddingStep++;
				mAddingVectorHelper2.y = mLockNumber;
				mLockNumber = 0;
			}
		}*/
		else
		{
			if (!mPath)
			{
				/*mPath = new Path(
					mAddingVectorHelper,
					0,
					mAddingVectorHelper2.x,
					WorldBox::BoxType::BT_BOX_PLATFORM_SMALL
				);*/
				changeCameraOrbit(mAddingVectorHelper, CAMERA_ZOOM_DEFAULT);
			}
			
			platformHelper = mPath->lastPlatform();

			mGameGUI->setEditorBottom1("Press left/right to rotate, up/down to modify height");
			
			if (KEYSTROKE(OIS::KC_LEFT))// && !mKeystateOld[OIS::KC_LEFT])
			{
				mPath->reduceRotation();
			}
			else if (KEYSTROKE(OIS::KC_RIGHT))// && !mKeystateOld[OIS::KC_RIGHT])
			{
				mPath->addRotation();
			}

			if (KEYSTROKE(OIS::KC_UP))// && !mKeystateOld[OIS::KC_LEFT])
			{
				mPath->addHeight();
			}
			else if (KEYSTROKE(OIS::KC_DOWN))// && !mKeystateOld[OIS::KC_RIGHT])
			{
				mPath->reduceHeight();
			}
			
			if (KEYSTROKE(OIS::KC_TAB) && !ALT_DOWN)// && !mKeystateOld[OIS::KC_TAB])
			{
				// TODO change size
				//short type = mPath->type;
				//if (keyboard->isKeyDown(OIS::KC_LSHIFT) || keyboard->isKeyDown(OIS::KC_RSHIFT))
				//{
				//	type--;
				//}
				//else
				//{
				//	type++;
				//}
				//if (type == WorldBox::BT_SIZE)
				//{
				//	type = 0;
				//}
				//if (type == -1)
				//{
				//	type = WorldBox::BT_SIZE - 1;
				//}
				//mPath->setType((WorldBox::BoxType)type);
			}

			if (KEYSTROKE(OIS::KC_N))// && !mKeystateOld[OIS::KC_N])
			{
				if (mEditor->canAddMorePlatforms())
				{
					mPath->appendPlatform();
				}
			}
			else if (KEYSTROKE(OIS::KC_P))// && !mKeystateOld[OIS::KC_P])
			{
				mPath->deleteOne();
				platformHelper = mPath->lastPlatform();
			}

			mMainNode->setPosition(mPath->lastPlatform()->getBox()->getNode()->_getDerivedPosition());

			if (CLICK(OIS::MouseButtonID::MB_Left))
			{
				mAddingPath = false;
				mCurrentModifyingPlatform.clear();
			}
			if (CLICK(OIS::MouseButtonID::MB_Right))
			{
				mPath->deleteAllPlatforms();
				mAddingPath = false;
				mCurrentModifyingPlatform.clear();
			}
		}
	}
	else if (mCurrentModifyingPlatform.size() == 0)
	{
		if (mCameraRoaming != 0)
		{
			changeCameraFreeRoaming();
		}

		Vector3 translate = mCamera->getDerivedOrientation() * Vector3(0, 0, 3000);
		
		platformHelper = Physics::getSingleton()->raycastPlatform(TO_BULLET(mCamera->getDerivedPosition()), TO_BULLET(mCamera->getDerivedPosition() - translate));

		if (CLICK(OIS::MouseButtonID::MB_Right))
		{
			if (SHIFT_DOWN)
			{
				if (platformHelper)
				{
					mCurrentModifyingPlatformHelperBuffer.erase(remove(mCurrentModifyingPlatformHelperBuffer.begin(), mCurrentModifyingPlatformHelperBuffer.end(), platformHelper), mCurrentModifyingPlatformHelperBuffer.end());
				}
			}
			else
			{
				mCurrentModifyingPlatformHelperBuffer.clear();
			}
		}

		if (platformHelper)
		{
			AxisAlignedBox box = platformHelper->getBox()->getNode()->_getWorldAABB();
			box.setExtents(box.getMinimum() - Vector3(2, 2, 2), box.getMaximum() + Vector3(2, 2, 2));
			
			DebugDrawer::getSingleton().drawCuboid(box.getAllCorners(), Ogre::ColourValue::White);
			
			if (currentMouseState.buttonDown(OIS::MouseButtonID::MB_Left))
			{
				if (SHIFT_DOWN)
				{
					if (std::find(mCurrentModifyingPlatformHelperBuffer.begin(), mCurrentModifyingPlatformHelperBuffer.end(), platformHelper)==mCurrentModifyingPlatformHelperBuffer.end())
					{
						mCurrentModifyingPlatformHelperBuffer.push_back(platformHelper);
					}
				}
				else if CLICK(OIS::MouseButtonID::MB_Left)
				{
					for (int i = 0; i < mCurrentModifyingPlatformHelperBuffer.size(); i++)
					{
						if (mCurrentModifyingPlatformHelperBuffer[i] == platformHelper) continue;
						else mCurrentModifyingPlatform.push_back(mCurrentModifyingPlatformHelperBuffer[i]);
					}

					mCurrentModifyingPlatform.push_back(platformHelper);
					onStartEditing();
					changeCameraOrbit(true);
				}
			}
		}
		
		for (int i = 0; i < mCurrentModifyingPlatformHelperBuffer.size(); i++)
		{
			mCurrentModifyingPlatformHelperBuffer[i]->getBox()->getNode()->_updateBounds();
			AxisAlignedBox box = mCurrentModifyingPlatformHelperBuffer[i]->getBox()->getNode()->_getWorldAABB();
			box.setExtents(box.getMinimum() - Vector3(2, 2, 2), box.getMaximum() + Vector3(2, 2, 2));
			if (platformHelper == mCurrentModifyingPlatformHelperBuffer[i])
			{
				DebugDrawer::getSingleton().drawCuboid(box.getAllCorners(), Ogre::ColourValue::Green); // yellow
			}
			else
			{
				DebugDrawer::getSingleton().drawCuboid(box.getAllCorners(), Ogre::ColourValue::Red);
			}
		}

		if (KEYSTROKE(OIS::KC_I))// && !mKeystateOld[OIS::KC_I])
		{
			if (mEditor->canAddMorePlatforms())
			{
				mCurrentModifyingPlatformHelperBuffer.clear();
				mCurrentModifyingPlatform.clear();

				Vector3 pos = mCamera->getDerivedPosition() - mCamera->getDerivedOrientation() * Vector3(0, 0, 30);
				
				for (int i = 0; i < 3; i++)
				{
					double helperDouble = 0;
					if (modf(pos[i], &helperDouble) > 0.5)
					{
						pos[i] = helperDouble + .5;
					}
					else
					{
						pos[i] = helperDouble;
					}
				}

				mCurrentModifyingPlatform.push_back(mEditor->newPlatform(
					pos,
					Degree(0)
				));

				onStartEditing();
			}
		}
		else if (KEYSTROKE(OIS::KC_P))
		{
			mPlayerInstance->_editorSetOrigin(mMainNode->_getDerivedPosition());
		}
		else if (KEYSTROKE(OIS::KC_M))// && !mKeystateOld[OIS::KC_M])
		{
			//mAddingMatrix = true;
			//mAddingStep = 0;
			//if (mMatrix) delete mMatrix;
			//mMatrix = 0;
		}
		else if (KEYSTROKE(OIS::KC_P))// && !mKeystateOld[OIS::KC_P])
		{
			//mAddingPath = true;
			//mAddingStep = 0;
			//if (mPath) delete mPath;
			//mPath = 0;
		}
	}
	// editing platforms
	else
	//if (mCurrentModifyingPlatform)
	{
		changeCameraOrbit();

		if (mModifyingSpeed || mLockX || mLockY || mLockZ || mRotate)
		{
			if (mLockX)
			{
				if (mMovementAbsolute)
				{
					mGameGUI->setEditorBottom1(SET_X_ABSOLUTE);
				}
				else
				{
					mGameGUI->setEditorBottom1(SET_X_RELATIVE);
				}
			}
			else if (mLockY)
			{
				if (mMovementAbsolute)
				{
					mGameGUI->setEditorBottom1(SET_Y_ABSOLUTE);
				}
				else
				{
					mGameGUI->setEditorBottom1(SET_Y_RELATIVE);
				}
			}
			else if (mLockZ)
			{
				if (mMovementAbsolute)
				{
					mGameGUI->setEditorBottom1(SET_Z_ABSOLUTE);
				}
				else
				{
					mGameGUI->setEditorBottom1(SET_Z_RELATIVE);
				}
			}
			else if (mRotate)
			{
				if (mModifyingOrigin)
				{
					if (mMovementAbsolute)
					{
						mGameGUI->setEditorBottom1(SET_ROTATION_SPEED_ABSOLUTE);
					}
					else
					{
						mGameGUI->setEditorBottom1(SET_ROTATION_SPEED_RELATIVE);
					}
				}
				else
				{
					if (mMovementAbsolute)
					{
						mGameGUI->setEditorBottom1(SET_ROTATION_ABSOLUTE);
					}
					else
					{
						mGameGUI->setEditorBottom1(SET_ROTATION_RELATIVE);
					}
				}
			}
			else
			{
				if (mMovementAbsolute)
				{
					mGameGUI->setEditorBottom1(SET_LINEAR_SPEED_ABSOLUTE);
				}
				else
				{
					mGameGUI->setEditorBottom1(SET_LINEAR_SPEED_RELATIVE);
				}
			}

			int oldLockNumber = mLockNumber;

			updateKeyboardNumber(keystateNew, mKeystateOld, mLockNumber);

			if (mModifyingSpeed)
			{
				for (int i = 0; i < mCurrentModifyingPlatform.size(); i++)
				{
					if (mRotate)
					{
						static_cast<MovablePlatform*>(mCurrentModifyingPlatform[i])->setRotateArgs(Degree(mLockNumber));
					}
					else
					{
						static_cast<MovablePlatform*>(mCurrentModifyingPlatform[i])->setLinearMovementArgs(TO_BULLET(static_cast<MovablePlatform*>(mCurrentModifyingPlatform[i])->getLinearArgument()), mLockNumber);
					}
				}

			}
			else if (!mRotate)
			{
				for (int i = 0; i < mCurrentModifyingPlatform.size(); i++)
				{
					Vector3 toIncrease = mModifyingPlatformLockStartPos[i];

					if (mLockX)
					{
						if (mMovementAbsolute)
						{
							toIncrease.x = mLockNumber;
						}
						else
						{
							toIncrease.x += mLockNumber;
						}
					}
					if (mLockY)
					{
						if (mMovementAbsolute)
						{
							toIncrease.y = mLockNumber;
						}
						else
						{
							toIncrease.y += mLockNumber;
						}
					}
					if (mLockZ)
					{
						if (mMovementAbsolute)
						{
							toIncrease.z = mLockNumber;
						}
						else
						{
							toIncrease.z += mLockNumber;
						}
					}
					
					if (mModifyingOrigin)
					{
						mAddingVectorHelper = toIncrease;
					}
					else
					{
						mCurrentModifyingPlatform[i]->_setPosition(toIncrease);
					}
				}

				if (oldLockNumber != mLockNumber)
				{
					Vector3 translate = Vector3::ZERO;
					int marginTranslate = mLockNumber - oldLockNumber;
					if (mLockX)
					{
						translate.x = marginTranslate;
					}
					if (mLockY)
					{
						translate.y = marginTranslate;
					}
					if (mLockZ)
					{
						translate.z = marginTranslate;
					}

					mMainNode->translate(translate);
				}
			}
			else
			{
				if (mMovementAbsolute)
				{
					for (int i = 0; i < mCurrentModifyingPlatform.size(); i++)
					{
						mCurrentModifyingPlatform[i]->_setRotate(Degree(mLockNumber));
					}
				}
				else
				{
					for (int i = 0; i < mCurrentModifyingPlatform.size(); i++)
					{
						mCurrentModifyingPlatform[i]->_setRotate(mModifyingPlatformLockStartRotation[i] + Degree(mLockNumber));
					}
				}
			}

			if ((CLICK(OIS::MouseButtonID::MB_Right)))
			{
				if (mRotate)
				{
					for (int i = 0; i < mCurrentModifyingPlatform.size(); i++)
					{
						mCurrentModifyingPlatform[i]->_setRotate(mModifyingPlatformLockStartRotation[i]);
					}
				}
				else
				{
					for (int i = 0; i < mCurrentModifyingPlatform.size(); i++)
					{
						mCurrentModifyingPlatform[i]->_setPosition(mModifyingPlatformLockStartPos[i]);
					}
				}
				mLockX = false;
				mLockY = false;
				mLockZ = false;
				mRotate = false;
				mModifyingSpeed = false;
				mMovementAbsolute = false;
			}
			else if (CLICK(OIS::MouseButtonID::MB_Left) || (KEYSTROKE(OIS::KC_RETURN)))
			{
				mLockX = false;
				mLockY = false;
				mLockZ = false;
				mRotate = false;
				mModifyingSpeed = false;
				mMovementAbsolute = false;
			}
		}
		else if (mModifyingOrigin)
		{
			Vector3 translateVector = Vector3::ZERO;

			if (KEYSTROKE(OIS::KC_W))
			{
				translateVector += Vector3(0, 0, -1);
			}
			if (KEYSTROKE(OIS::KC_S))
			{
				translateVector += Vector3(0, 0, 1);
			}
			if (KEYSTROKE(OIS::KC_D))
			{
				translateVector += Vector3(1, 0, 0);
			}
			if (KEYSTROKE(OIS::KC_A))
			{
				translateVector += Vector3(-1, 0, 0);
			}

			if (translateVector.length() > 0)
			{
				translateVector = mCameraYaw->getOrientation() * translateVector;
				if (abs(translateVector.z) > abs(translateVector.x))
				{
					if (translateVector.z > 0)
					{
						translateVector = Vector3::UNIT_Z;
					}
					else
					{
						translateVector = Vector3::NEGATIVE_UNIT_Z;
					}
				}
				else
				{
					if (translateVector.x > 0)
					{
						translateVector = Vector3::UNIT_X;
					}
					else
					{
						translateVector = Vector3::NEGATIVE_UNIT_X;
					}
				}
			}

			translateVector *= translatePerKeystroke;
			if (KEYSTROKE(OIS::KC_E))
			{
				translateVector += Vector3(0, translatePerKeystroke, 0);
			}
			if (KEYSTROKE(OIS::KC_Q))
			{
				translateVector += Vector3(0, -translatePerKeystroke, 0);
			}

			if (translateVector.length() > 0)
			{
				if (SHIFT_DOWN)
				{
					translateVector *= 10;
				}

				for (int i = 0; i < mCurrentModifyingPlatform.size(); i++)
				{
					static_cast<MovablePlatform*>(mCurrentModifyingPlatform[i])->setLinearMovementArgs(TO_BULLET((static_cast<MovablePlatform*>(mCurrentModifyingPlatform[i])->getLinearArgument()) + translateVector), static_cast<MovablePlatform*>(mCurrentModifyingPlatform[i])->getLinearSpeed());
				}
			}

			if (KEYSTROKE(OIS::KC_RETURN))
			{
				mModifyingOrigin = false;
			}
			else if (KEYSTROKE(OIS::KC_DELETE))
			{
				static_cast<MovablePlatform*>(mCurrentModifyingPlatform[0])->setLinearMovementArgs(TO_BULLET(mModifyingPlatformStartPos[0]), static_cast<MovablePlatform*>(mCurrentModifyingPlatform[0])->getLinearSpeed());
			}
		}
		else
		{
			Vector3 translateVector = Vector3::ZERO;

			if (KEYSTROKE(OIS::KC_W))
			{
				translateVector += Vector3(0, 0, -1);
			}
			if (KEYSTROKE(OIS::KC_S))
			{
				translateVector += Vector3(0, 0, 1);
			}
			if (KEYSTROKE(OIS::KC_D))
			{
				translateVector += Vector3(1, 0, 0);
			}
			if (KEYSTROKE(OIS::KC_A))
			{
				translateVector += Vector3(-1, 0, 0);
			}
			
			if (translateVector.length() > 0)
			{
				translateVector = mCameraYaw->getOrientation() * translateVector;
				if (abs(translateVector.z) > abs(translateVector.x))
				{
					if (translateVector.z > 0)
					{
						translateVector = Vector3::UNIT_Z;
					}
					else
					{
						translateVector = Vector3::NEGATIVE_UNIT_Z;
					}
				}
				else
				{
					if (translateVector.x > 0)
					{
						translateVector = Vector3::UNIT_X;
					}
					else
					{
						translateVector = Vector3::NEGATIVE_UNIT_X;
					}
				}
			}

			translateVector *= translatePerKeystroke;
			if (KEYSTROKE(OIS::KC_E))
			{
				translateVector += Vector3(0, translatePerKeystroke, 0);
			}
			if (KEYSTROKE(OIS::KC_Q))
			{
				translateVector += Vector3(0, -translatePerKeystroke, 0);
			}

			if (translateVector.length() != 0)
			{
				if (SHIFT_DOWN)
				{
					translateVector *= 10;
				}
				else if (CONTROL_DOWN)
				{
					translateVector *= 250;
				}

				mMainNode->translate(translateVector);
				//if (!(keyboard->isKeyDown(OIS::KC_LSHIFT) || keyboard->isKeyDown(OIS::KC_RSHIFT)))
				//{
				//	mMainNode->translate(translateVector);
				//}
				//else
				//{
				//	Vector3 newVector = mCameraYaw->getOrientation() * Vector3::UNIT_Z;
				//	if (abs(newVector.z) > abs(newVector.x))
				//	{
				//		if (newVector.z > 0)
				//		{
				//			newVector = Vector3::UNIT_Z;
				//		}
				//		else
				//		{
				//			newVector = Vector3::NEGATIVE_UNIT_Z;
				//		}
				//	}
				//	else
				//	{
				//		if (newVector.x > 0)
				//		{
				//			newVector = Vector3::UNIT_X;
				//		}
				//		else
				//		{
				//			newVector = Vector3::NEGATIVE_UNIT_X;
				//		}
				//	}
				//}

				for (int i = 0; i < mCurrentModifyingPlatform.size(); i++)
				{
					mCurrentModifyingPlatform[i]->_translate(translateVector);
				}
			}

			if (
				(KEYSTROKE(OIS::KC_Z)) ||
				(KEYSTROKE(OIS::KC_X)) ||
				(KEYSTROKE(OIS::KC_Y)) ||
				(KEYSTROKE(OIS::KC_R)))
			{
				if ((SHIFT_DOWN) && mCurrentModifyingPlatform.size() == 1)
				{
					mMovementAbsolute = true;
				}
				else
				{
					mMovementAbsolute = false;
				}

				if (KEYSTROKE(OIS::KC_X))// && !mKeystateOld[OIS::KC_X])
				{
					mLockX = true;
					if (mMovementAbsolute)
					{
						mLockNumber = mCurrentModifyingPlatform[0]->getBox()->getNode()->_getDerivedPosition().x;
					}
					else
					{
						mLockNumber = 0;
					}
				}
				else if (KEYSTROKE(OIS::KC_Y))// && !mKeystateOld[OIS::KC_Y])
				{
					mLockY = true;
					if (mMovementAbsolute)
					{
						mLockNumber = mCurrentModifyingPlatform[0]->getBox()->getNode()->_getDerivedPosition().y;
					}
					else
					{
						mLockNumber = 0;
					}
				}
				else if (KEYSTROKE(OIS::KC_Z))// && !mKeystateOld[OIS::KC_Z])
				{
					mLockZ = true;
					if (mMovementAbsolute)
					{
						mLockNumber = mCurrentModifyingPlatform[0]->getBox()->getNode()->_getDerivedPosition().z;
					}
					else
					{
						mLockNumber = 0;
					}
				}
				else if (KEYSTROKE(OIS::KC_R))// && !mKeystateOld[OIS::KC_R])
				{
					mRotate = true;
					if (mMovementAbsolute)
					{
						mLockNumber = fixAngleNumber(mCurrentModifyingPlatform[0]->getBox()->getNode()->_getDerivedOrientation().getYaw().valueDegrees());
					}
					else
					{
						mLockNumber = 0;
					}
				}

				if (KEYSTROKE(OIS::KC_R))// && !mKeystateOld[OIS::KC_R])
				{
					mModifyingPlatformLockStartRotation.clear();
					for (int i = 0; i < mCurrentModifyingPlatform.size(); i++)
					{
						mModifyingPlatformLockStartRotation.push_back(mCurrentModifyingPlatform[i]->getBox()->getNode()->getOrientation().getYaw());
					}
				}
				else
				{
					mModifyingPlatformLockStartPos.clear();
					for (int i = 0; i < mCurrentModifyingPlatform.size(); i++)
					{
						mModifyingPlatformLockStartPos.push_back(TO_OGRE(mCurrentModifyingPlatform[i]->getBox()->getBody()->getWorldTransform().getOrigin()));
					}
				}
			}
			else if (KEYSTROKE(OIS::KC_T))
			{
				for (int i = 0; i < mCurrentModifyingPlatform.size(); i++)
				{
					if (!mCurrentModifyingPlatform[i]->getActivator())
					{
						mCurrentModifyingPlatform[i]->addActivator(WorldActivator::ActivatorType::AT_POINTS);
						mEditor->addActivator(mCurrentModifyingPlatform[i]->getActivator());
					}
					else
					{
						mEditor->removeActivator(mCurrentModifyingPlatform[i]->getActivator());
						mCurrentModifyingPlatform[i]->_removeActivator();
					}
				}
			}
			else if (KEYSTROKE(OIS::KC_C))
			{
				if (mCurrentModifyingPlatform.size() == 1 && mCurrentModifyingPlatform[0]->getActivator() != 0 && mCurrentModifyingPlatform[0]->getActivator()->getActivatorType() == WorldActivator::AT_RESPAWN)
				{
					mEditor->setInitialRespawn(mCurrentModifyingPlatform[0]);
				}
			}
			else if (!SHIFT_DOWN && KEYSTROKE(OIS::KC_TAB) && !ALT_DOWN && CONTROL_DOWN)// && !mKeystateOld[OIS::KC_TAB])
			{
				for (int i = 0; i < mCurrentModifyingPlatform.size(); i++)
				{
					if (CONTROL_DOWN)
					{
						if (mCurrentModifyingPlatform[i]->getActivator())
						{
							mEditor->changeActivatorType(mCurrentModifyingPlatform[i]->getActivator());
							mCurrentModifyingPlatform[i]->getActivator()->_changeType(
								mCurrentModifyingPlatform[i]->getActivator()->getActivatorType() == WorldActivator::AT_RESPAWN ?
								WorldActivator::AT_POINTS :
								WorldActivator::AT_RESPAWN
							);
						}
					}
					else
					{
						//short type = mCurrentModifyingPlatform[i]->getBox()->getBoxType();
						//if (keyboard->isKeyDown(OIS::KC_LSHIFT) || keyboard->isKeyDown(OIS::KC_RSHIFT))
						//{
						//	type--;
						//}
						//else
						//{
						//	type++;
						//}
						//if (type == WorldBox::BT_SIZE)
						//{
						//	type = 0;
						//}
						//if (type == -1)
						//{
						//	type = WorldBox::BT_SIZE - 1;
						//}
						//mCurrentModifyingPlatform[i]->_changeType((WorldBox::BoxType)type);
					}
				}
			}
			else if (KEYSTROKE(OIS::KC_LEFT))
			{
				for (int i = 0; i < mCurrentModifyingPlatform.size(); i++)
				{
					mCurrentModifyingPlatform[i]->_setRotate(mCurrentModifyingPlatform[i]->getBox()->getNode()->getOrientation().getYaw() - Degree(15));
				}
			}
			else if (KEYSTROKE(OIS::KC_RIGHT))
			{
				for (int i = 0; i < mCurrentModifyingPlatform.size(); i++)
				{
					mCurrentModifyingPlatform[i]->_setRotate(mCurrentModifyingPlatform[i]->getBox()->getNode()->getOrientation().getYaw() + Degree(15));
				}
			}
			else if (KEYSTROKE(OIS::KC_L))
			{
				mModifyingOrigin = true;
				mAddingVectorHelper = mCurrentModifyingPlatform[0]->getBox()->getNode()->_getDerivedPosition();

				for (int i = 0; i < mCurrentModifyingPlatform.size(); i++)
				{
					if (mCurrentModifyingPlatform[i]->Type() != PlatformType::PT_MOVABLE)
					{
						mCurrentModifyingPlatform[i] = mEditor->upcastPlatform(mCurrentModifyingPlatform[i]);
						
					}

					if (static_cast<MovablePlatform*>(mCurrentModifyingPlatform[i])->getLinearSpeed() == 0)
					{
						static_cast<MovablePlatform*>(mCurrentModifyingPlatform[i])->setLinearSpeed(EDITOR_DEFAULT_LINEAR_SPEED);
					}
					static_cast<MovablePlatform*>(mCurrentModifyingPlatform[i])->addFlag(MovablePlatform::MP_LINEAR);
				}
			}
			else if (KEYSTROKE(OIS::KC_K))
			{
				mModifyingSpeed = true;
				mAddingVectorHelper = Vector3::ZERO;//mCurrentModifyingPlatform[0]->getBox()->getNode()->_getDerivedPosition();
				for (int i = 0; i < mCurrentModifyingPlatform.size(); i++)
				{
					if (mCurrentModifyingPlatform[i]->Type() != PlatformType::PT_MOVABLE)
					{
						mCurrentModifyingPlatform[i] = mEditor->upcastPlatform(mCurrentModifyingPlatform[i]);
						
					}
					static_cast<MovablePlatform*>(mCurrentModifyingPlatform[i])->addFlag(MovablePlatform::MP_LINEAR);
					mAddingVectorHelper.x = static_cast<MovablePlatform*>(mCurrentModifyingPlatform[i])->getLinearSpeed();
				}
			}
			else if (KEYSTROKE(OIS::KC_J))
			{
				mModifyingSpeed = true;
				mRotate = true;
				mAddingVectorHelper = Vector3::ZERO;
				for (int i = 0; i < mCurrentModifyingPlatform.size(); i++)
				{
					if (mCurrentModifyingPlatform[i]->Type() != PlatformType::PT_MOVABLE)
					{
						mCurrentModifyingPlatform[i] = mEditor->upcastPlatform(mCurrentModifyingPlatform[i]);
						
					}

					this->mModifyingPlatformLockStartRotation.push_back(static_cast<MovablePlatform*>(mCurrentModifyingPlatform[i])->getRotateSpeed());
					static_cast<MovablePlatform*>(mCurrentModifyingPlatform[i])->addFlag(MovablePlatform::MP_ROTATING);
				}
			}
			else if (KEYSTROKE(OIS::KC_H))
			{
				for (int i = 0; i < mCurrentModifyingPlatform.size(); i++)
				{
					if (mCurrentModifyingPlatform[i]->Type() != PlatformType::PT_MOVABLE)
					{
						mCurrentModifyingPlatform[i] = mEditor->upcastPlatform(mCurrentModifyingPlatform[i]);
						
					}

					static_cast<MovablePlatform*>(mCurrentModifyingPlatform[i])->setLinearMovementType(!static_cast<MovablePlatform*>(mCurrentModifyingPlatform[i])->getIsLinearMovementType());
				}
			}
			else if (KEYSTROKE(OIS::KC_G))
			{
				for (int i = 0; i < mCurrentModifyingPlatform.size(); i++)
				{
					if (mCurrentModifyingPlatform[i]->Type() != PlatformType::PT_MOVABLE)
					{
						mCurrentModifyingPlatform[i] = mEditor->upcastPlatform(mCurrentModifyingPlatform[i]);
						
					}

					static_cast<MovablePlatform*>(mCurrentModifyingPlatform[i])->setRotationRight(!static_cast<MovablePlatform*>(mCurrentModifyingPlatform[i])->getIsRotationRight());
				}
			}
			/*else if (KEYSTROKE(OIS::KC_B))
			{
				for (int i = 0; i < mCurrentModifyingPlatform.size(); i++)
				{
					mCurrentModifyingPlatform[i]->_toggleBreakableFlag();
				}
			}
			else if (KEYSTROKE(OIS::KC_V))
			{
				for (int i = 0; i < mCurrentModifyingPlatform.size(); i++)
				{
					mCurrentModifyingPlatform[i]->_toggleKillFlag();
				}
			}*/
			else if (KEYSTROKE(OIS::KC_TAB))
			{
				for (int i = 0; i < mCurrentModifyingPlatform.size(); i++)
				{
					short type = mCurrentModifyingPlatform[i]->getBox()->getMaterial();
					if (SHIFT_DOWN)
					{
						type--;
					}
					else
					{
						type++;
					}

					if (type == WorldBox::CM_SIZE)
					{
						type = 0;
					}
					if (type == -1)
					{
						type = WorldBox::CM_SIZE - 1;
					}

					mCurrentModifyingPlatform[i]->_changePlatformType((WorldBox::CustomMaterial)type);
				}
			}
			else if (KEYSTROKE(OIS::KC_DELETE))
			{
				for (int i = 0; i < mCurrentModifyingPlatform.size(); i++)
				{
					mEditor->deletePlatform(mCurrentModifyingPlatform[i]);
				}
				mCurrentModifyingPlatform.clear();
			}

			if (mCurrentModifyingPlatform.size() > 0 && CLICK(OIS::MouseButtonID::MB_Right))
			{
				for (int i = 0; i < mCurrentModifyingPlatform.size(); i++)
				{
					mCurrentModifyingPlatform[i]->_setPosition(mModifyingPlatformStartPos[i]);
					mCurrentModifyingPlatform[i]->_setRotate(mModifyingPlatformLockStartRotation[i]);
				}
				onFinishEditing();
				changeCameraFreeRoaming();
			}
			else if (mCurrentModifyingPlatform.size() > 0 && CLICK(OIS::MouseButtonID::MB_Left))
			{
				onFinishEditing();
				changeCameraFreeRoaming();
			}
		}

		
		for (int i = 0; i < mCurrentModifyingPlatform.size(); i++)
		{
			mCurrentModifyingPlatform[i]->getBox()->getNode()->_updateBounds();
			AxisAlignedBox box = mCurrentModifyingPlatform[i]->getBox()->getNode()->_getWorldAABB();
			box.setExtents(box.getMinimum() - Vector3(.2, .2, .2), box.getMaximum() + Vector3(.2, .2, .2));
			DebugDrawer::getSingleton().drawCuboid(box.getAllCorners(), Ogre::ColourValue::Red);
		}
	}
	
	for (int i = 0; i < mEditor->getMovablePlatforms()->size(); i++)
	{
		MovablePlatform* movablePlat = (*mEditor->getMovablePlatforms())[i];

		if (!movablePlat->_validMovable()) continue;

		if (movablePlat->isFlagActive(MovablePlatform::MP_LINEAR))
		{
			DebugDrawer::getSingleton().drawLine(
				movablePlat->getBox()->getNode()->_getDerivedPosition(),
				(movablePlat->getLinearArgument() + movablePlat->getBox()->getNode()->_getDerivedPosition()),
				ColourValue(1.0, 1.0, 0.0)
			);

			if (!movablePlat->getIsLinearMovementType())
			{
				DebugDrawer::getSingleton().drawCircle(
					movablePlat->getLinearArgument() + movablePlat->getBox()->getNode()->_getDerivedPosition(),
					(movablePlat->getLinearArgument()).length(),
					256,
					ColourValue(HEX_TO_COLOR_VALUE(0x87,0xCE,0xEB))
				);
			}
		}
		
		if (
			platformHelper != movablePlat &&
			!(std::find(mCurrentModifyingPlatform.begin(), mCurrentModifyingPlatform.end(), movablePlat) != mCurrentModifyingPlatform.end()) &&
			!(std::find(mCurrentModifyingPlatformHelperBuffer.begin(), mCurrentModifyingPlatformHelperBuffer.end(), movablePlat) != mCurrentModifyingPlatformHelperBuffer.end()))
		{
			//mCurrentModifyingPlatform[i]->getBox()->getNode()->_updateBounds();
			AxisAlignedBox box = movablePlat->getBox()->getNode()->_getWorldAABB();
			box.setExtents(box.getMinimum() - Vector3(.2, .2, .2), box.getMaximum() + Vector3(.2, .2, .2));
			DebugDrawer::getSingleton().drawCuboid(box.getAllCorners(), Ogre::ColourValue(1.0, 1.0, 0.0));
		}
	}

	if (KEYSTROKE(OIS::KC_S) && CONTROL_DOWN)
	{
		mEditor->reloadMapFormat();
		MapFormat* format = mEditor->getFormat();
		format->saveToFile("resources\\maps\\editing.map");
	}
	mGameGUI->setEditorTop1(mCameraHolder->_getDerivedPosition());
	bool shouldErase = false;
	if (mCurrentModifyingPlatformHelperBuffer.size() == 0 && platformHelper)
	{
		shouldErase = true;
		mCurrentModifyingPlatformHelperBuffer.push_back(platformHelper);
	}
	mGameGUI->updateEditorSides(
		mEditor->getAmountOfPlatforms(),
		String(mEditor->getFormat()->name),
		mCurrentModifyingPlatform.size() == 0 ? mCurrentModifyingPlatformHelperBuffer : mCurrentModifyingPlatform
	);
	if (shouldErase) mCurrentModifyingPlatformHelperBuffer.clear();

	mOldMouseState = currentMouseState;
	memcpy(mKeystateOld, keystateNew, sizeof(char) * 256);

	//DebugDrawer::getSingleton().drawLine(Vector3::ZERO, Vector3(0, 0, 1), ColourValue::White);

	//platformHelper->getBox()->getEntity()->
}

void EditorPlayer::changeCameraFreeRoaming()
{
	Vector3 position = mCamera->getDerivedPosition();
	mMainNode->setPosition(position);
	mCameraHolder->setPosition(0, 0, 0);
	mCameraRoaming = 0;
}

void EditorPlayer::changeCameraOrbit(bool resetZoom)
{
	int distance = 0;
	Vector3 center = Vector3::ZERO;

	for (int i = 0; i < mCurrentModifyingPlatform.size(); i++)
	{
		center += mCurrentModifyingPlatform[i]->getBox()->getNode()->_getDerivedPosition();
		distance += (mCurrentModifyingPlatform[i]->getBox()->getNode()->_getDerivedPosition() - mCamera->getDerivedPosition()).length();
	}

	if (resetZoom)
	{
		mCameraHolder->setPosition(Vector3(0, 0, distance / mCurrentModifyingPlatform.size()));
	}

	if (mCurrentModifyingPlatform.size() == 1 && mCurrentModifyingPlatform[0]->Type() == PlatformType::PT_MOVABLE && mModifyingOrigin)
	{
		mMainNode->setPosition(mCurrentModifyingPlatform[0]->getBox()->getNode()->_getDerivedPosition() + static_cast<MovablePlatform*>(mCurrentModifyingPlatform[0])->getLinearArgument());
	}
	else
	{
		mMainNode->setPosition(center / mCurrentModifyingPlatform.size());
	}
	
	mCameraRoaming = 1;
}

void EditorPlayer::changeCameraOrbit(Vector3 position, int length)
{
	mCameraHolder->setPosition(Vector3(0, 0, length));
	mMainNode->setPosition(position);
	
	mCameraRoaming = 1;
}

void EditorPlayer::onStartEditing()
{
	mModifyingPlatformLockStartPos.clear();
	mModifyingPlatformLockStartRotation.clear();
	mModifyingPlatformStartPos.clear();
	mAddingVectorHelper = Vector3::ZERO;
	mAddingVectorHelper2 = Vector3::ZERO;
	mRotate = false;
	mLockX = false;
	mLockY = false;
	mLockZ = false;
	mModifyingOrigin = false;
	mAddingMatrix = false;
	mAddingPath = false;
	mMovementAbsolute = false;
	mLockNumber = 0;
	mAddingVectorHelper = Vector3::ZERO;
	mAddingVectorHelper2 = Vector3::ZERO;
	mCurrentModifyingPlatformHelperBuffer.clear();
	for (int i = 0; i < mCurrentModifyingPlatform.size(); i++)
	{
		mModifyingPlatformStartPos.push_back(mCurrentModifyingPlatform[i]->getBox()->getNode()->_getDerivedPosition());
	}
	for (int i = 0; i < mCurrentModifyingPlatform.size(); i++)
	{
		mModifyingPlatformLockStartRotation.push_back(mCurrentModifyingPlatform[i]->getBox()->getNode()->_getDerivedOrientation().getYaw());
	}

	if (mCurrentModifyingPlatform.size() == 1)
	{
		//changeCameraOrbit(mCurrentModifyingPlatform[0]->getBox()->getNode()->getPosition(), (mCurrentModifyingPlatform[0]->getBox()->getNode()->getPosition() - mCamera->getDerivedPosition()).length());
	}

	mGameGUI->setEditingCrosshairVisible(false);
}

void EditorPlayer::onFinishEditing()
{
	mCurrentModifyingPlatformHelperBuffer.clear();
	mCurrentModifyingPlatform.clear();
	mModifyingPlatformLockStartPos.clear();
	mModifyingPlatformLockStartRotation.clear();
	mModifyingPlatformStartPos.clear();
	mRotate = false;
	mLockX = false;
	mLockY = false;
	mLockZ = false;
	mAddingMatrix = false;
	mAddingPath = false;
	mMovementAbsolute = false;
	mModifyingOrigin = false;
	mLockNumber = 0;
	mAddingVectorHelper = Vector3::ZERO;
	mAddingVectorHelper2 = Vector3::ZERO;
	mGameGUI->setEditingCrosshairVisible(true);
}

void EditorPlayer::updateKeyboardNumber(char* keystateNew, char* keystateOld, int& number)
{
	int toIncrease = 0;
	bool keypress = false;
	for (int i = OIS::KC_1; i <= OIS::KC_0; i++)
	{
		if (!keystateNew[i] && keystateOld[i])
		{
			keypress = true;
			if (i != OIS::KC_0)
			{
				toIncrease = i - OIS::KC_1 + 1;
			}
			break;
		}
	}

	if (!keypress)
	{
		for (int i = OIS::KC_NUMPAD1; i <= OIS::KC_NUMPAD0; i++)
		{
			if (!keystateNew[i] && keystateOld[i])
			{
				if (i != OIS::KC_NUMPAD0)
				{
					toIncrease = i - OIS::KC_NUMPAD1 + 1;
				}
				break;
			}
		}
	}

	if (keypress)
	{
		number = number * 10 + (number < 0 ? -toIncrease : toIncrease);
	}

	if (!keystateNew[OIS::KC_MINUS] && keystateOld[OIS::KC_MINUS])
	{
		number *= -1;
	}

	if (!keystateNew[OIS::KC_BACK] && keystateOld[OIS::KC_BACK])
	{
		number = (int)Math::Floor(number / 10.0);
	}

	mGameGUI->setEditorBottom2(StringConverter::toString(number));
}

void EditorPlayer::onPlayerToggle()
{
	SceneNode* parentNode = static_cast<SceneNode*>(mCamera->getParentNode());
	if (parentNode) parentNode->detachObject(mCamera);
	mCameraHolder->attachObject(mCamera);
}

WorldEditor* EditorPlayer::Matrix3D::editor = 0;

// keys used
// selecting platforms:
// X Y Z R 
// on editing:
//
// adding matrix:
//
// adding path:
// 