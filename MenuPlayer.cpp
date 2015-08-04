#include "MenuPlayer.h"
#include "Keys.h"
#include "WorldGenerator.h"

MenuPlayer::MenuPlayer(WorldGenerator* generator, Camera* camera, SceneManager* sceneManager, RenderWindow* renderWindow)
	: mCamera(camera),
	mRenderWindow(renderWindow),
	mSceneManager(sceneManager),
	mRightDirection(false),
	mGenerator(generator)
{
	mCameraPivot = mSceneManager->getRootSceneNode()->createChildSceneNode();;//
	mCameraYaw = mCameraPivot->createChildSceneNode();
	mCameraPitch = mCameraYaw ->createChildSceneNode();
	mCameraHolder = mCameraPitch->createChildSceneNode();
	mCameraHolder->translate(Vector3(0, 0, CAMERA_START_MENU_DISTANCE));
	mCameraHolder->attachObject(mCamera);

	mCameraPitch->pitch((Degree(-40)));
}


MenuPlayer::~MenuPlayer(void)
{
}

void MenuPlayer::update(const FrameEvent& evt)
{
	mGenerator->playerPlatformShaderArgs(Vector4::ZERO, Vector3::ZERO);
	mCameraYaw->yaw(evt.timeSinceLastFrame * Degree(10));
	return; // TODO implement everything below this
	const int mouseMaxSpeed = 1;
	auto xMouse = Keys::singleton->MouseX;
	if (Math::Abs(xMouse) > 20) xMouse = 20 * xMouse < 0 ? -1 : 1;
	if (Math::Abs(xMouse) > 10) mRightDirection = xMouse > 0 ? true : false;
	mCameraYaw->yaw(evt.timeSinceLastFrame * Degree(Math::Abs(xMouse) > 10 ? xMouse : 10 * mRightDirection ? 1 : -1));
}