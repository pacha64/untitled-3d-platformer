#pragma once

#include "stdafx.h"

#include "Player.h"
#include "WorldEditor.h"
#include "balance.h"
#include "GameState.h"
#include "MapCollection.h"
#include "GameGUI.h"
#include "WorldActivator.h"
#include "MonsterState.h"
#include "SoundManager.h"
#include "EditorPlayer.h"
#include "Timer.h"
#include "MenuPlayer.h"
#include "NewGameArgs.h"

class Playground : public Ogre::WindowEventListener, public Ogre::FrameListener, public Ogre::RenderQueueListener, public OIS::KeyListener
{
public:
	Playground(void);
	~Playground(void);
   void go(void);
	void takeScreenshot();
	static void destroySceneNode(SceneNode* sceneNode);

protected:
    // Ogre::WindowEventListener
    virtual void windowResized(Ogre::RenderWindow* rw);
    virtual void windowClosed(Ogre::RenderWindow* rw);
 
	// keyboard
	virtual bool keyPressed(const OIS::KeyEvent &arg);
	virtual bool keyReleased(const OIS::KeyEvent &arg);

    // Ogre::FrameListener
  //  virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);
	virtual bool frameStarted (const FrameEvent &evt);
    virtual bool frameEnded(const FrameEvent& evt)
    {
		if (mCurrentGameLocation == GameStateLocation::GSL_INGAME)
		{
			DebugDrawer::getSingleton().clear();
		}
		return true;
	}

	void destroyCurrentScene();
	void initializeCurrentScene();
	void toggleFullScreen();
private:
	void startGame(NewGameArgs& args);
	enum InputSystemType
	{
		IST_INGAME,
		IST_GUI,
		IST_INGAME_PAUSE
	};
	CTimer mTimer;
	Real mTimeSinceForceRespawn;
	void recreateInputSystem(InputSystemType);
	InputSystemType mCurrentInputSystemType;
	enum GameStateLocation
	{
		GSL_MENU,
		GSL_INGAME
	};
	HWND mHwnd;
	GameStateLocation mCurrentGameLocation;
	//GameGUI* mGameGui;
	GameGUI* mGameGui;
	GameState* mGameState;
	WorldGenerator::MapZones mCurrentMapZone;
	Real mTimeOnArea;
	WorldGenerator* mGenerator;
	SoundManager* mSoundGenerator;
	Camera* mCamera;
	bool mQuit;
	bool mPause;
	//void createPlatformBorder(Vector3& size);
	void configHelper();
    Ogre::Root* mRoot;
    Ogre::String mResourcesCfg;
    Ogre::String mPluginsCfg;
    Ogre::RenderWindow* mWindow;
    Ogre::SceneManager* mSceneMgr;
	Player* mPlayer;
	EditorPlayer* mEditorPlayer;
	bool mUsingEditorPlayer;
	Real mTimeLastFrame;
	MenuPlayer* mMenuPlayer;

    // OIS Input devices
    OIS::InputManager* mInputManager;
    OIS::Mouse* mMouse;
    OIS::Keyboard* mKeyboard;
	OIS::JoyStick* mJoystick;
};

