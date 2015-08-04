#pragma once
#include "stdafx.h"

class WorldGenerator;

class MenuPlayer
{
public:
	MenuPlayer(WorldGenerator* generator, Camera*, SceneManager*, RenderWindow*);
	~MenuPlayer(void);
	void update(const FrameEvent&);//, bool isPaused);
protected:
	WorldGenerator* mGenerator;
	bool mRightDirection;
	Camera* mCamera;
	SceneManager* mSceneManager;
	RenderWindow* mRenderWindow;
	SceneNode* mCameraPivot;
	SceneNode* mCameraYaw;
	SceneNode* mCameraPitch;
	SceneNode* mCameraHolder;
};

