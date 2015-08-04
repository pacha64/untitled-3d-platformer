#include "Playground.h"
#include "Physics.h"
#include "WorldGenerator.h"
#include <stdlib.h>
#include <stdio.h>

#include <OgreException.h>
#include <OgreConfigFile.h>
#include <OgreCamera.h>
#include <OgreViewport.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include <OgreEntity.h>
#include <OgreWindowEventUtilities.h>
#include <OgreRenderSystem.h>
#include "TextRenderer.h"
#include "Keys.h"
#include "resource.h"
#include "InfiniteMiniWorldGenerator.h"
#include "InfinitePathWorldGenerator.h"
#include "TutorialWorldGenerator.h"
#include "NewGameArgs.h"


Playground::Playground(void)
	: mRoot(0),
	mMenuPlayer(0),
	mTimeSinceForceRespawn(0),
	mResourcesCfg(StringUtil::BLANK),
    mPluginsCfg(StringUtil::BLANK),
    mWindow(0),
    mSceneMgr(0),
	mSoundGenerator(0),
	mGenerator(0),
	mPlayer(0),
	mEditorPlayer(0),
	mPause(false),
	mQuit(false),
	mGameGui(0),
	mGameState(0),
	mCurrentGameLocation(GameStateLocation::GSL_MENU),
	mJoystick(0),
	mKeyboard(0),
	mMouse(0),
	mInputManager(0),
	mCurrentInputSystemType(InputSystemType::IST_GUI),
	mHwnd(0),
	mUsingEditorPlayer(false),
	mCurrentMapZone(WorldGenerator::MZ_ICE_CORE),
	mTimeOnArea(0.0)
{
}

Playground::~Playground(void)
{
    //Remove ourself as a Window listener
	if (mGameGui != 0)
	{
		delete mGameGui;
	}
    Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, this);
    windowClosed(mWindow);
    delete mRoot;
}

void Playground::go(void)
{
	mResourcesCfg = "game-resources.cfg";
	mPluginsCfg = "plugins.cfg";
 
	// construct Ogre::Root
	mRoot = new Ogre::Root(mPluginsCfg);

 
	// setup resources
	// Load resource paths from config file
	Ogre::ConfigFile cf;
	cf.load(mResourcesCfg);
 
	// Go through all sections & settings in the file
	Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();
 
	Ogre::String secName, typeName, archName;
	while (seci.hasMoreElements())
	{
		secName = seci.peekNextKey();
		Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
		Ogre::ConfigFile::SettingsMultiMap::iterator i;
		for (i = settings->begin(); i != settings->end(); ++i)
		{
			typeName = i->first;
			archName = i->second;
			Ogre::ResourceGroupManager::getSingleton().addResourceLocation(archName, typeName, secName);
		}
	}

    // configure
    // Show the configuration dialog and initialise the system
	configHelper();
 
    mWindow = mRoot->initialise(false, "3D platformer");
	 
	//EnumDisplayMonitors(NULL, NULL, sCreateMonitorsInfoEnumProc, (LPARAM)&mMonitorInfoList);
	NameValuePairList args;
	args["vsync"] = "true";
	args["title"] = generateWindowTitle(0).c_str();
	args["border"] = "resize";
//	args["externalWindowHandle"] = StringConverter::toString((size_t)mHwnd);
	args["FSAA"] = "8";  // Level 4 anti aliasing 
	auto options = mRoot->getRenderSystem()->getConfigOptions();
	_::options.loadConfig(CONFIG_FILENAME);
	
	DEVMODE dm = { 0 };
	dm.dmSize = sizeof(dm);
	int maxWidth = DEFAULT_WINDOW_WIDTH, maxHeight = DEFAULT_WINDOW_HEIGHT;
	for( int iModeNum = 0; EnumDisplaySettings( NULL, iModeNum, &dm ) != 0; iModeNum++ )
	{
		if (dm.dmPelsHeight > maxHeight) maxHeight = dm.dmPelsHeight;
		if (dm.dmPelsWidth > maxWidth) maxWidth = dm.dmPelsWidth;
	}
	
	// 4K resolution: 3840, 2160
	mWindow = mRoot->createRenderWindow("main-window", maxWidth, maxHeight, false, &args);
//	mWindow = mRoot->createRenderWindow("main-window", 1680, 1050, true, &args);
	
	//Lets load a custom icon into the window class
	mWindow->getCustomAttribute("WINDOW", &mHwnd);
	if(mHwnd != 0)
	{
	   LONG iconID = (LONG)LoadIcon(GetModuleHandle(0), MAKEINTRESOURCE(IDI_ICON1));
	   SetClassLong(mHwnd, GCL_HICON, iconID);
	}
	mSceneMgr = mRoot->createSceneManager("DefaultSceneManager", "default");
	mSceneMgr->setAmbientLight(Ogre::ColourValue(1.0,1.0,1.0));
	mSceneMgr->setShadowTechnique(ShadowTechnique::SHADOWTYPE_NONE);
	
    // Set default mipmap level (NB some APIs ignore this)
    Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);
	
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
    // initialise all resource groups
	Ogre::MeshManager::getSingleton().setBoundsPaddingFactor(0);
    // Create the SceneManager, in this case a generic one


	//mSceneMgr->setShadowTextureSize(2048);
	//mSceneMgr->setShadowTextureFSAA(8);


	mCamera = mSceneMgr->createCamera("player-camera");

	//mCameraCenterYaw = mBodyNode->_getDerivedOrientation().getYaw();
    
	mCamera->setPosition(Vector3(0, 0, 0));
	mCamera->lookAt(Vector3::ZERO);
	mCamera->setNearClipDistance(1);

	Ogre::Viewport* vp = mWindow->addViewport(mCamera);
    vp->setBackgroundColour(Ogre::ColourValue(0.882,0.267,0.988));
    mCamera->setAspectRatio(Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));

	mSceneMgr->getRootSceneNode()->attachObject(mCamera);
	//mCameraHolder->attachObject(mCamera);
	
	vp->update();

	new TextRenderer();
	TextRenderer::getSingleton().addTextBox("textbox-1", "", 10, 10, 100, 20, Ogre::ColourValue::Red);
	TextRenderer::getSingleton().addTextBox("textbox-2", "", 10, 45, 100, 20, Ogre::ColourValue::Red);
	TextRenderer::getSingleton().addTextBox("textbox-3", "", 10, 80, 100, 20, Ogre::ColourValue::Red);
	TextRenderer::getSingleton().addTextBox("textbox-4", "", 10, 80 + 35, 100, 20, Ogre::ColourValue::Red);
	TextRenderer::getSingleton().addTextBox("textbox-5", "", 10, 80+ 35*2, 100, 20, Ogre::ColourValue::Red);
	TextRenderer::getSingleton().addTextBox("textbox-6", "", 10, 80+ 35*3, 100, 20, Ogre::ColourValue::Red);
	TextRenderer::getSingleton().addTextBox("textbox-7", "", 10, 80+ 35*4, 100, 20, Ogre::ColourValue::Red);
	TextRenderer::getSingleton().addTextBox("textbox-8", "", 10, 80+ 35*5, 100, 20, Ogre::ColourValue::Red);
	TextRenderer::getSingleton().addTextBox("textbox-9", "", 10, 80+ 35*6, 100, 20, Ogre::ColourValue::Red);
	TextRenderer::getSingleton().addTextBox("textbox-10", "", 10, 80+ 35*7, 100, 20, Ogre::ColourValue::Red);
	TextRenderer::getSingleton().addTextBox("textbox-11", "", 10, 80+ 35*8, 100, 20, Ogre::ColourValue::Red);
	TextRenderer::getSingleton().addTextBox("textbox-12", "", 10, 80+ 35*9, 100, 20, Ogre::ColourValue::Red);
	TextRenderer::getSingleton().addTextBox("textbox-13", "", 10, 80+ 35*10, 100, 20, Ogre::ColourValue::Red);
    // Set ambient light

	Monster::initialize(mSceneMgr);

	MapCollection* mapCollection = MapCollection::getSingleton();
	Ogre::String filename = "resources/maps";
	mapCollection->load(filename);

	//Real distance = mGenerator->getFarthestDistanceToSoul(mPlayer->getLocation());
	//mGameState.timeLeft = Math::Ceil(distance * TIME_PER_DISTANCE_UNIT);
	
	mSoundGenerator = SoundManager::singleton;
	mSoundGenerator->init();

	mGameGui = new GameGUI(
		mSoundGenerator,
		mWindow->getWidth(),
		mWindow->getHeight(),
		mSceneMgr
	);
	// gui is fucked up without this, don't know why.
	mGameGui->onWindowResize(_::options.window.width, _::options.window.height);

	mGameGui->changeCurrentGui(GameGUI::GT_MENU_START);

	recreateInputSystem(InputSystemType::IST_GUI);

//	mMouse->setEventCallback(Keys::singleton);

	//mJoystick = static_cast<OIS::JoyStick*>(mInputManager->createInputObject( OIS::OISJoyStick, false ));
    //Set initial mouse clipping size
    windowResized(mWindow);
 
    //Register as a Window listener
    Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);

    mRoot->addFrameListener(this);
	
	//mWindow->setFullscreen(true, 1680, 1050);
	mRoot->getRenderSystem()->_initRenderTargets();

    // Clear event times
    mRoot->clearEventTimes();

	// init physics, reduces slowdown in first game.
	Physics::getSingleton()->recreateWorld();

	//Ogre::CompositorPtr comp3 = Ogre::CompositorManager::getSingleton().create(
 //       "Motion Blur", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME
 //   );
 //   {
 //       Ogre::CompositionTechnique *t = comp3->createTechnique();
 //       {
 //           Ogre::CompositionTechnique::TextureDefinition *def = t->createTextureDefinition("scene");
 //           def->width = 0;
 //           def->height = 0;
 //           def->formatList.push_back(Ogre::PF_R8G8B8);
 //       }
 //       {
 //           Ogre::CompositionTechnique::TextureDefinition *def = t->createTextureDefinition("sum");
 //           def->width = 0;
 //           def->height = 0;
 //           def->formatList.push_back(Ogre::PF_R8G8B8);
 //       }
 //       {
 //           Ogre::CompositionTechnique::TextureDefinition *def = t->createTextureDefinition("temp");
 //           def->width = 0;
 //           def->height = 0;
 //           def->formatList.push_back(Ogre::PF_R8G8B8);
 //       }
 //       /// Render scene
 //       {
 //           Ogre::CompositionTargetPass *tp = t->createTargetPass();
 //           tp->setInputMode(Ogre::CompositionTargetPass::IM_PREVIOUS);
 //           tp->setOutputName("scene");
 //       }
 //       /// Initialisation pass for sum texture
 //       {
 //           Ogre::CompositionTargetPass *tp = t->createTargetPass();
 //           tp->setInputMode(Ogre::CompositionTargetPass::IM_PREVIOUS);
 //           tp->setOutputName("sum");
 //           tp->setOnlyInitial(true);
 //       }
 //       /// Do the motion blur
 //       {
 //           Ogre::CompositionTargetPass *tp = t->createTargetPass();
 //           tp->setInputMode(Ogre::CompositionTargetPass::IM_NONE);
 //           tp->setOutputName("temp");
 //           { Ogre::CompositionPass *pass = tp->createPass();
 //               pass->setType(Ogre::CompositionPass::PT_RENDERQUAD);
 //               pass->setMaterialName("Ogre/Compositor/Combine");
 //               pass->setInput(0, "scene");
 //               pass->setInput(1, "sum");
 //           }
 //       }
 //       /// Copy back sum texture
 //       {
 //           Ogre::CompositionTargetPass *tp = t->createTargetPass();
 //           tp->setInputMode(Ogre::CompositionTargetPass::IM_NONE);
 //           tp->setOutputName("sum");
 //           { Ogre::CompositionPass *pass = tp->createPass();
 //               pass->setType(Ogre::CompositionPass::PT_RENDERQUAD);
 //               pass->setMaterialName("Ogre/Compositor/Copyback");
 //               pass->setInput(0, "temp");
 //           }
 //       }
 //       /// Display result
 //       {
 //           Ogre::CompositionTargetPass *tp = t->getOutputTargetPass();
 //           tp->setInputMode(Ogre::CompositionTargetPass::IM_NONE);
 //           { Ogre::CompositionPass *pass = tp->createPass();
 //               pass->setType(Ogre::CompositionPass::PT_RENDERQUAD);
 //               pass->setMaterialName("Ogre/Compositor/MotionBlur");
 //               pass->setInput(0, "sum");
 //           }
 //       }
 //   }

//	CompositorManager::getSingleton().addCompositor(vp, "MotionBlur");
	
	//CompositorManager::getSingleton().addCompositor(vp, "SSAA");
	//CompositorManager::getSingleton().setCompositorEnabled(vp, "SSAA", true);

	//CompositorManager::getSingleton().addCompositor(vp, "Bloom");
	//CompositorManager::getSingleton().setCompositorEnabled(vp, "Bloom", true);

//	CompositorManager::getSingleton().addCompositor(vp, "Motion Blur");
//	CompositorManager::getSingleton().setCompositorEnabled(vp, "Motion Blur", true);
	
	NewGameArgs newGameArgs;
	newGameArgs.type = WorldGenerator::WGT_START_MENU;
	newGameArgs.optionalMapFormat = getRandomStartMenuMap();
	startGame(newGameArgs);
	
	MSG msg;
	ZeroMemory(&msg,sizeof(MSG));

	while (!mQuit)
	{
//		Sleep(1000 / FRAMES_PER_SECOND);
		//Pump messages in all registered RenderWindow windows
		WindowEventUtilities::messagePump();
		if (this->mCurrentGameLocation == GameStateLocation::GSL_INGAME)
		{
			if (!mRoot->renderOneFrame())// / _::options.slowMotion))
			{
				break;
			}
		}
		else
		{
			if (!mRoot->renderOneFrame())
			{
				break;
			}
		}

		static bool firstResize = false;
		if (!firstResize)
		{
			firstResize = true;
			mWindow->resize(_::options.window.width, _::options.window.height);
			mWindow->reposition(_::options.window.x, _::options.window.y);
#if !_DEBUG
			toggleFullScreen();
#endif
		}


		/*static int counter = 0;
		counter++;
		if (counter % FRAMES_PER_SECOND / 12 == 0)
		{*/
		
		/*if (Keys::singleton->keystroke(Keys::K_EXTRA))
		{
			static bool a = false;
			a = !a;
			CompositorManager::getSingleton().setCompositorEnabled(vp, "Bloom", a);
		}*/

		static unsigned int oldWidth = 0, oldHeight = 0;
		unsigned int width = mWindow->getWidth();
		unsigned int height = mWindow->getHeight();
		bool helper = false;
		if (width < MIN_WINDOW_WIDTH)
		{
			width = MIN_WINDOW_WIDTH;
			helper = true;
		}
		if (height < MIN_WINDOW_HEIGHT)
		{
			height = MIN_WINDOW_HEIGHT;
			helper = true;
		}
		if (helper)
		{
			mWindow->resize(width, height);
		}
		if (/*(oldWidth != 0 && oldHeight != 0) &&*/ (oldWidth != width || oldHeight != height))
		{
			
			mGameGui->onWindowResize(width, height);
			oldWidth = width;
			oldHeight = height;
		}
		Real aspectRatio = mCamera->getAspectRatio();
		if (aspectRatio != (Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight())))
		{
			mCamera->setAspectRatio((Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight())));
		}
			
		if (Keys::singleton->isKeyDown(Keys::singleton->K_TOGGLE_DEBUG_MODE) && !Keys::singleton->isKeyDownLastUpdate(Keys::singleton->K_TOGGLE_DEBUG_MODE))
		{

		}

		//}
		//mSceneMgr->setShadowCameraSetup(ShadowCameraSetupPtr(new FocusedShadowCameraSetup()));
	}
}

void Playground::recreateInputSystem(InputSystemType inputSystem)
{
    if( mInputManager )
    {
		if (mMouse != 0)
		{
			mInputManager->destroyInputObject( mMouse );
			mMouse = 0;
		}
		if (mJoystick != 0)
		{
			mInputManager->destroyInputObject( mJoystick );
			mJoystick = 0;
		}
		if (mKeyboard != 0)
		{
			mInputManager->destroyInputObject(mKeyboard);
			mKeyboard = 0;
		}
 
        OIS::InputManager::destroyInputSystem(mInputManager);
        mInputManager = 0;
    }

	switch (inputSystem)
	{
	case InputSystemType::IST_GUI:
	case InputSystemType::IST_INGAME_PAUSE:
		{
			OIS::ParamList pl;
			size_t windowHnd = 0;
			std::ostringstream windowHndStr;
 
			mWindow->getCustomAttribute("WINDOW", &windowHnd);
			windowHndStr << windowHnd;
			pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
			pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_FOREGROUND" )));
			pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_NONEXCLUSIVE")));
			//pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_FOREGROUND")));
			//pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_NONEXCLUSIVE")));
 
			mInputManager = OIS::InputManager::createInputSystem( pl );

			mInputManager->getNumberOfDevices(OIS::Type::OISJoyStick);

			mKeyboard = 0; mMouse = 0;
			try
			{
				mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject( OIS::OISKeyboard, true ));
			}
			catch(...) { }
			if (mInputManager->getNumberOfDevices(OIS::Type::OISJoyStick) > 0)
			{
				try
				{
					mJoystick = static_cast<OIS::JoyStick*>(mInputManager->createInputObject( OIS::OISJoyStick, true ));
					_::options.axisMax = mJoystick->MAX_AXIS;
				}
				catch(...) { }
			}
			try
			{
				mMouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject( OIS::OISMouse, true ));
				mMouse->getMouseState().width = mWindow->getWidth();
				mMouse->getMouseState().height = mWindow->getHeight();
			}
			catch(...) { }
		}
		break;
	case InputSystemType::IST_INGAME:
		{
			OIS::ParamList pl;
			size_t windowHnd = 0;
			std::ostringstream windowHndStr;
 
			mWindow->getCustomAttribute("WINDOW", &windowHnd);
			windowHndStr << windowHnd;
			pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

			mInputManager = OIS::InputManager::createInputSystem( pl );

			mInputManager->getNumberOfDevices(OIS::Type::OISJoyStick);

			mKeyboard = 0; mMouse = 0;
			try
			{
				mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject( OIS::OISKeyboard, true ));
			}
			catch(...) { }
			if (mInputManager->getNumberOfDevices(OIS::Type::OISJoyStick) > 0)
			{
				try
				{
					mJoystick = static_cast<OIS::JoyStick*>(mInputManager->createInputObject( OIS::OISJoyStick, true ));
					_::options.axisMax = mJoystick->MAX_AXIS;
				}
				catch(...) { }
			}
			try
			{
				mMouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject( OIS::OISMouse, true ));
				mMouse->getMouseState().width = mWindow->getWidth();
				mMouse->getMouseState().height = mWindow->getHeight();
			}
			catch(...) { }
		}
		break;
	}

	if (mGameGui != 0)
	{
		mGameGui->onInputRecreation(mKeyboard, mMouse, mJoystick);
	}

	if (mKeyboard != 0)
	{
		Keys::singleton->onRecreateInputSystem(*mKeyboard);
		mKeyboard->setEventCallback(this);
	}

	mCurrentInputSystemType = inputSystem;
}

void Playground::takeScreenshot()
{
	auto unixtime = time(0);
	auto timestamp = localtime(&unixtime);
	char buffer[256];
	strftime(buffer, 256, "%m,%d,%Y-%H,%M,%S", timestamp);
	mWindow->writeContentsToFile(String("screenshots/platformer-") + buffer + ".png");
}

void Playground::destroySceneNode(SceneNode* sceneNode)
{
	SceneNode::ObjectIterator itObject = sceneNode->getAttachedObjectIterator();

	while (itObject.hasMoreElements())
	{
		MovableObject* pObject = static_cast<MovableObject*>(itObject.getNext());
		sceneNode->getCreator()->destroyMovableObject(pObject);
	}

	// Recurse to child SceneNodes
	SceneNode::ChildNodeIterator itChild = sceneNode->getChildIterator();

	while (itChild.hasMoreElements())
	{
		SceneNode* pChildNode = static_cast<SceneNode*>(itChild.getNext());
		destroySceneNode( pChildNode );
	}

	sceneNode->removeAndDestroyAllChildren();
	Root::getSingleton().getSceneManager(SCENE_MANAGER_NAME)->destroySceneNode(sceneNode);
}

void Playground::startGame(NewGameArgs& args)
{
	if (DebugDrawer::getSingletonPtr())
	{
		delete DebugDrawer::getSingletonPtr();
	}
	destroyCurrentScene();
	Physics::getSingleton()->recreateWorld();
	Physics::getSingleton()->initDebugDrawer(mSceneMgr);
	new DebugDrawer(mSceneMgr, 0.5);

	// debug remove later
	//format->gameType = MapFormat::GT_EDITOR;

	int level = mGameState ? mGameState->level + 1 : 1;
	if (mMenuPlayer) delete mMenuPlayer;
	mMenuPlayer = 0;
	

	switch (args.type)
	{
	case WorldGenerator::WGT_GENERIC:
	case WorldGenerator::WGT_START_MENU:
		{
			mGenerator = new WorldGenerator(mSceneMgr);
			mSceneMgr->getRootSceneNode()->detachObject(mCamera);
			mGenerator->setMapStructure(args.optionalMapFormat);
			if (args.type == WorldGenerator::WGT_START_MENU)
			{
				mMenuPlayer = new MenuPlayer(mGenerator, mCamera, mSceneMgr, mWindow);
			}
		}
		break;
	case WorldGenerator::WGT_TUTORIAL:
		{
			mGenerator = new TutorialWorldGenerator(mSceneMgr);
			mSceneMgr->getRootSceneNode()->detachObject(mCamera);
			mGenerator->setMapStructure(getTutorialMap(0));
		}
		break;
	case WorldGenerator::WGT_INFINITE_PATH:
		{
			_::stats.scoreHistory[args.difficulty].numberOfGames++;
			mGenerator = new InfinitePathWorldGenerator(mSceneMgr);
			static_cast<InfinitePathWorldGenerator*>(mGenerator)->setDifficulty(args.difficulty);
			mSceneMgr->getRootSceneNode()->detachObject(mCamera);
			break;
		}
	case WorldGenerator::WGT_WORLD_EDITOR:
		mGenerator = new WorldEditor(mSceneMgr);
		mGenerator->setMapStructure(args.optionalMapFormat);
		mSceneMgr->getRootSceneNode()->detachObject(mCamera);
		mEditorPlayer = new EditorPlayer(mPlayer, mGameGui, mCamera, mSceneMgr, (WorldEditor*)mGenerator);
		break;
		break;
	case WorldGenerator::WGT_MINI_MAPS:
		// not implemented
		throw 0;
		break;
	}
	
	mGenerator->generate();
	if (args.type != WorldGenerator::WGT_START_MENU)
	{
		mPlayer = new Player(mGenerator, mCamera, mSceneMgr, mWindow);
	}

	if (args.type != WorldGenerator::WGT_START_MENU)
	{
		if (args.type == WorldGenerator::WGT_WORLD_EDITOR)
		{
			mGameGui->changeCurrentGui(GameGUI::GT_PLAYING);
		}
		else 
		{
			mGameGui->changeCurrentGui(GameGUI::GT_EDITING_WORLD);
		}
		
		mCurrentGameLocation = GameStateLocation::GSL_INGAME;
		mGameState = new GameState();
		mGameState->level = level;
		mGenerator->setGameState(mGameState);
		mGameState->difficulty = args.difficulty;
		if (args.type == WorldGenerator::WGT_WORLD_EDITOR)
		{
			mGameGui->changeCurrentGui(GameGUI::GT_EDITING_WORLD);
			mUsingEditorPlayer = true;
		}
		else
		{
			mGameGui->changeCurrentGui(GameGUI::GT_PLAYING);
			mUsingEditorPlayer = false;
		}

		mGameState->hasTime = args.usesTime;
		if (mGameState->hasTime)
		{
			mGameState->timeLeft = mGenerator->calculateTimeLeft();
		}
	}

	mSoundGenerator->playNewSound2D(SoundManager::Music::PACHELBEL_CANON_IN_D);
}

void Playground::destroyCurrentScene()
{
	if (mGameState) delete mGameState;
	if (mGenerator) delete mGenerator;
	if (mPlayer) delete mPlayer;
	if (mEditorPlayer) delete mEditorPlayer;
	mGameState = 0;
	mGenerator = 0;
	mPlayer = 0;
	mEditorPlayer = 0;
	//mSceneMgr->destroyAllCameras();
	//mSceneMgr->destroyAllAnimations();
	//mSceneMgr->destroyAllAnimationStates();
	//mSceneMgr->destroyAllStaticGeometry();
	//mSceneMgr->destroyAllEntities();
	//mSceneMgr->destroyAllInstanceManagers();
	//mSceneMgr->destroyAllLights();
	mSceneMgr->clearScene();
}

void Playground::initializeCurrentScene()
{
}

void Playground::toggleFullScreen()
{
	mGameGui->pauseFullscreenClick = false;
	static bool isFullScreen = false;
	static int windowWidth = DEFAULT_WINDOW_WIDTH, windowHeight = DEFAULT_WINDOW_HEIGHT;

	if (mWindow->isFullScreen())
	{
		mWindow->setFullscreen(false, windowWidth, windowHeight);
		mGameGui->onWindowResize(windowWidth, windowHeight);
	}
	else
	{
		isFullScreen = true;
		windowWidth = mWindow->getWidth();
		windowHeight = mWindow->getHeight();

		const Ogre::ConfigOptionMap::iterator opti = 
		mRoot->getRenderSystem()->getConfigOptions().find("Video Mode");
		unsigned int w = 0, h = 0;
		for (int i = 0; i < opti->second.possibleValues.size(); i++)
		{
			Ogre::StringVector vmopts = Ogre::StringUtil::split(opti->second.possibleValues[i], "x");
			int width = Ogre::StringConverter::parseUnsignedInt(vmopts[0]);
			int height = Ogre::StringConverter::parseUnsignedInt(vmopts[1]);
			if (width > w)
			{
				w = width;
				h = height;
			}
		}
			
		mWindow->setFullscreen(true, w, h);
		mGameGui->onWindowResize(w, h);
	}
}
//Adjust mouse clipping area
void Playground::windowResized(Ogre::RenderWindow* rw)
{
    unsigned int width, height, depth;
    int left, top;
    rw->getMetrics(width, height, depth, left, top);
 
    /*const OIS::MouseState &ms = mMouse->getMouseState();
    ms.width = width;
    ms.height = height;*/
}
 
//Unattach OIS before window shutdown (very important under Linux)
void Playground::windowClosed(Ogre::RenderWindow* rw)
{
    //Only close for window that created OIS (the main window in these demos)
    if( rw == mWindow )
    {
        if( mInputManager )
        {
			if (mMouse != 0)
			{
				mInputManager->destroyInputObject( mMouse );
				mMouse = 0;
			}
			if (mJoystick != 0)
			{
				mInputManager->destroyInputObject( mJoystick );
				mJoystick = 0;
			}
 
            OIS::InputManager::destroyInputSystem(mInputManager);
            mInputManager = 0;
        }
    }
	 unsigned int aa;
	 mWindow->getMetrics(_::options.window.width, _::options.window.height, aa, _::options.window.x, _::options.window.y);
	_::options.saveConfig(CONFIG_FILENAME);
	//_::stats.saveConfig(STATS_FILENAME);
} 

bool Playground::keyPressed(const OIS::KeyEvent &arg)
{
	Rocket::Core::Input::KeyIdentifier key = Keys::singleton->ois2rocket(arg.key);
	
	int modifier_state = 0;

	// TODO fix this
	//if (mKeyboard->isModifierDown(OIS::Keyboard::Ctrl))
	//	modifier_state |= Rocket::Core::Input::KM_CTRL;
	//if (mKeyboard->isModifierDown(OIS::Keyboard::Shift))
	//	modifier_state |= Rocket::Core::Input::KM_SHIFT;
	//if (mKeyboard->isModifierDown(OIS::Keyboard::Alt))
	//	modifier_state |= Rocket::Core::Input::KM_ALT;

	if (GetKeyState(VK_CAPITAL) > 0)
		modifier_state |= Rocket::Core::Input::KM_CAPSLOCK;
	if (GetKeyState(VK_NUMLOCK) > 0)
		modifier_state |= Rocket::Core::Input::KM_NUMLOCK;
	if (GetKeyState(VK_SCROLL) > 0)
		modifier_state |= Rocket::Core::Input::KM_SCROLLLOCK;

	if (key != Rocket::Core::Input::KI_UNKNOWN)
		mGameGui->getContext()->ProcessKeyDown(key, modifier_state);

	if (arg.text >= 32)
		mGameGui->getContext()->ProcessTextInput((Rocket::Core::word) arg.text);
	else if (key  == Rocket::Core::Input::KI_RETURN)
		mGameGui->getContext()->ProcessTextInput((Rocket::Core::word) '\n');

	return true;
}

bool Playground::keyReleased(const OIS::KeyEvent &arg)
{
	Rocket::Core::Input::KeyIdentifier key = Keys::singleton->ois2rocket(arg.key);
	
	int modifier_state = 0;

	// TODO fix trhis
	//if (mKeyboard->isModifierDown(OIS::Keyboard::Ctrl))
	//	modifier_state |= Rocket::Core::Input::KM_CTRL;
	//if (mKeyboard->isModifierDown(OIS::Keyboard::Shift))
	//	modifier_state |= Rocket::Core::Input::KM_SHIFT;
	//if (mKeyboard->isModifierDown(OIS::Keyboard::Alt))
	//	modifier_state |= Rocket::Core::Input::KM_ALT;

	if (GetKeyState(VK_CAPITAL) > 0)
		modifier_state |= Rocket::Core::Input::KM_CAPSLOCK;
	if (GetKeyState(VK_NUMLOCK) > 0)
		modifier_state |= Rocket::Core::Input::KM_NUMLOCK;
	if (GetKeyState(VK_SCROLL) > 0)
		modifier_state |= Rocket::Core::Input::KM_SCROLLLOCK;

	if (key != Rocket::Core::Input::KI_UNKNOWN)
		mGameGui->getContext()->ProcessKeyUp(key, modifier_state);

	return true;
}

bool Playground::frameStarted(const Ogre::FrameEvent& evt)
{
	//const_cast<Ogre::FrameEvent&>(evt).timeSinceLastFrame /= 5;
	//const_cast<Ogre::FrameEvent&>(evt).timeSinceLastEvent /= 5;

	static int secondsCounter = 0;
	mTimer.update(evt.timeSinceLastFrame);
	if (secondsCounter != Math::IFloor(mTimer.getElapsedSeconds()))
	{
		secondsCounter = Math::IFloor(mTimer.getElapsedSeconds());
		SetWindowText(mHwnd, generateWindowTitle(mTimer.getFPS()).c_str());
	}

    if(mWindow->isClosed())
        return false;

	Ogre::FrameEvent evt2 = evt;
	if (evt2.timeSinceLastFrame > 1.0 / FRAMES_PER_SECOND * 3) evt2.timeSinceLastFrame = 1.0 / FRAMES_PER_SECOND * 3;

	if (Keys::singleton->keystroke(Keys::K_EXTRA))
	{
		if (_::options.slowMotion == 1)
		{
			_::options.slowMotion = 0.15;
		}
		else if (_::options.slowMotion == 0.5)
		{
			_::options.slowMotion = 1.0;
		}
		else
		{
			_::options.slowMotion = 1;
		}
	}

	if (Keys::singleton->keystroke(Keys::K_SCREENSHOT))
	{
		takeScreenshot();
	}

//	_::options.slowMotion = 0.3;

	evt2.timeSinceLastFrame *= _::options.slowMotion;
	evt2.timeSinceLastEvent *= _::options.slowMotion;

	//const_cast<Ogre::FrameEvent&>(evt).timeSinceLastFrame = evt.timeSinceLastFrame / 2;

    //Need to capture/update each device
	/*if (mJoystick != 0)
	{
	    mJoystick->capture();
	}*/
	mKeyboard->capture();
	if (mJoystick != 0)
	{
		mJoystick->capture();
	}
	if (mMouse != 0)
	{
		mMouse->capture();
	}

	if (
		mGameGui->pauseFullscreenClick ||
		(Keys::singleton->isKeyDown(Keys::K_TOGGLE_FULL_SCREEN) && !Keys::singleton->isKeyDownLastUpdate(Keys::K_TOGGLE_FULL_SCREEN)))
	{
		toggleFullScreen();
	}

	switch (mCurrentGameLocation)
	{
		case GameStateLocation::GSL_MENU:
		{
			mMenuPlayer->update(evt);
			if (mCurrentInputSystemType != InputSystemType::IST_GUI)
			{
				recreateInputSystem(InputSystemType::IST_GUI);
			}
			
			Keys::singleton->updateKeystroke(mKeyboard, mMouse, mJoystick);

			if (mGameGui->singlePlayerClick)
			{
				mGameGui->changeCurrentGui(GameGUI::GT_DIFFICULTY_SELECT);
				mGameGui->singlePlayerClick = false;
			}
			// world editor click -> quit game
			else if (mGameGui->tutorialClick)
			{
				NewGameArgs args;
				args.type = WorldGenerator::WorldGeneratorType::WGT_TUTORIAL;
				startGame(args);
				mGameGui->tutorialClick = false;
			}
			// world editor click -> quit game
			else if (mGameGui->worldEditorClick)
			{
				//args.seconds = 360;
				mQuit = true;
//				startGame(MapCollection::getSingleton()->_helperMap(), true);
//				mGameGui->worldEditorClick = false;
			}
			else if (mGameGui->singlePlayerMapSelectClick)
			{
				// not implemented
				mGameGui->singlePlayerMapSelectClick = false;
			}
			else if (mGameGui->difficultyVeryEasyClick)
			{
				NewGameArgs args;
				args.type = WorldGenerator::WorldGeneratorType::WGT_INFINITE_PATH;
				args.difficulty = GameState::D_VERY_EASY;
				args.optionalMapFormat = new MapFormat("resources/maps/editing.map");
				startGame(args);
				mGameGui->difficultyVeryEasyClick = false;
			}
			else if (mGameGui->difficultyEasyClick)
			{
				NewGameArgs args;
				args.type = WorldGenerator::WorldGeneratorType::WGT_INFINITE_PATH;
				args.difficulty = GameState::D_EASY;
				args.optionalMapFormat = new MapFormat("resources/maps/editing.map");
				startGame(args);
				mGameGui->difficultyEasyClick = false;
			}
			else if (mGameGui->difficultyNormalClick)
			{
				NewGameArgs args;
				args.type = WorldGenerator::WorldGeneratorType::WGT_INFINITE_PATH;
				args.difficulty = GameState::D_NORMAL;
				args.optionalMapFormat = new MapFormat("resources/maps/editing.map");
				startGame(args);
				mGameGui->difficultyNormalClick = false;
			}
			else if (mGameGui->difficultyHardClick)
			{
				NewGameArgs args;
				args.type = WorldGenerator::WorldGeneratorType::WGT_INFINITE_PATH;
				args.difficulty = GameState::D_HARD;
				args.optionalMapFormat = new MapFormat("resources/maps/editing.map");
				startGame(args);
				mGameGui->difficultyHardClick = false;
			}
			else if (mGameGui->difficultyVeryHardClick)
			{
				NewGameArgs args;
				args.type = WorldGenerator::WorldGeneratorType::WGT_INFINITE_PATH;
				args.difficulty = GameState::D_VERY_HARD;
				args.optionalMapFormat = new MapFormat("resources/maps/editing.map");
				startGame(args);
				mGameGui->difficultyVeryHardClick = false;
			}
			else if (mGameGui->difficultyBackClick)
			{
				mGameGui->changeCurrentGui(GameGUI::GT_MENU_START);
				mGameGui->difficultyBackClick = false;
			}
			else if (mGameGui->singlePlayerBackClick)
			{
				mGameGui->changeCurrentGui(GameGUI::GT_MENU_START);
				mGameGui->singlePlayerBackClick = false;
			}
			break;
		}
	case GameStateLocation::GSL_INGAME:
		//OIS::JoyStickState state = mJoystick->getJoyStickState();
		//Keys::singleton->updateKeyMapping(Keys::KeyName::K_CAMERA_ZOOM_ADD, _::options, mMouse, mJoystick);

		/*static EditorPlayer* aa = 0;
		if (!aa)
		{
			aa = new EditorPlayer(mCamera, mSceneMgr, mWindow);
		}

		aa->update(evt);*/

		if (mGameState->maxScore > _::stats.scoreHistory[mGameState->difficulty].maxScore)
		{
			_::stats.scoreHistory[mGameState->difficulty].maxScore = mGameState->maxScore;
		}
		_::stats.scoreHistory[mGameState->difficulty].timePlayed += evt.timeSinceLastFrame;
		
		DEBUG_PRINT(1, %.3f, mGameState->timeLeft);

		if (!mPause && mCurrentInputSystemType != InputSystemType::IST_INGAME)
		{
			recreateInputSystem(InputSystemType::IST_INGAME);
		}

		Keys::singleton->updateKeystroke(mKeyboard, mMouse, mJoystick);

		if (_::options.pauseWhileInactive && (!mWindow->isFullScreen() && (mHwnd != 0 && mHwnd != GetActiveWindow()) && !mPause))
		{
			mPause = true;
			mGameGui->changeCurrentGui(GameGUI::GT_PAUSE_MENU);
			if (mCurrentInputSystemType != InputSystemType::IST_INGAME_PAUSE)
			{
				recreateInputSystem(InputSystemType::IST_INGAME_PAUSE);
			}
		}

		if(mGameGui->pauseGoBackToMapClick || (Keys::singleton->isKeyDown(Keys::K_PAUSE) && !Keys::singleton->isKeyDownLastUpdate(Keys::K_PAUSE)))
		{
			mGameGui->pauseGoBackToMapClick = false;
			mPause = !mPause;
			if (mPause)
			{
				mGameGui->changeCurrentGui(GameGUI::GT_PAUSE_MENU);
				if (mCurrentInputSystemType != InputSystemType::IST_INGAME_PAUSE)
				{
					recreateInputSystem(InputSystemType::IST_INGAME_PAUSE);
				}
			}
			else
			{
				if (mGenerator->getGeneratorType() == WorldGenerator::WGT_WORLD_EDITOR)
				{
					if (mUsingEditorPlayer)
					{
						mGameGui->changeCurrentGui(GameGUI::GT_EDITING_WORLD);
					}
					else
					{
						if (mPlayer->isDead())
						{
							mGameGui->changeCurrentGui(GameGUI::GT_DEATH);
						}
						else
						{
							mGameGui->changeCurrentGui(GameGUI::GT_PLAYING);
						}
					}
				}
				else
				{
					if (mPlayer->isDead())
					{
						mGameGui->changeCurrentGui(GameGUI::GT_DEATH);
					}
					else
					{
						mGameGui->changeCurrentGui(GameGUI::GT_PLAYING);
					}
				}

				if (mCurrentInputSystemType != InputSystemType::IST_INGAME)
				{
					recreateInputSystem(InputSystemType::IST_INGAME);
				}

			}
		}

		if (mPause)
		{
			switch(mGameGui->getCurrentGui())
			{
			case GameGUI::GT_INGAME_OPTIONS:
				mGameGui->updateIngameOptionsGui();
				break;
			case GameGUI::GT_PAUSE_MENU:
				break;
			}

			if (mGameGui->pauseQuitClick || Keys::singleton->isKeyDown(Keys::K_GO_TO_MENU))
			{
				mPause = false;
				mGameGui->changeCurrentGui(GameGUI::GT_MENU_START);
				mCurrentGameLocation = GameStateLocation::GSL_MENU;
				NewGameArgs args;
				args.type = WorldGenerator::WGT_START_MENU;
				args.optionalMapFormat = getRandomStartMenuMap();
				startGame(args);
				mGameGui->pauseQuitClick = false;
				return true;
			}
			if (mGameGui->pauseOptionClick)
			{
				mGameGui->setGUISettings(_::options);
				mGameGui->changeCurrentGui(GameGUI::GT_INGAME_OPTIONS);
				mGameGui->pauseOptionClick = false;
			}
			if (mGameGui->ingameCancelClick)
			{
				mGameGui->changeCurrentGui(GameGUI::GT_PAUSE_MENU);
				mGameGui->ingameCancelClick = false;
			}
			if (mGameGui->pauseChangeThemeClick)
			{
				mGameGui->changeCurrentGui(GameGUI::GT_SELECT_THEME);
				mGameGui->pauseChangeThemeClick = false;
			}
			if (mGameGui->pauseMapInfo)
			{
				mGameGui->onMapInfoStart(mGenerator->getFormat());
				mGameGui->changeCurrentGui(GameGUI::GT_SET_MAP_INFO);
				mGameGui->pauseMapInfo = false;
			}
			if (mGameGui->ingameOkClick)
			{
				_::options = mGameGui->getGUISettings();
				mGameGui->changeCurrentGui(GameGUI::GT_PAUSE_MENU);
				mGameGui->ingameOkClick = false;
			}
			if (mGameGui->themeBackButton)
			{
				mGameGui->changeCurrentGui(GameGUI::GT_PAUSE_MENU);
				mGameGui->themeBackButton = false;
			}
			if (mGameGui->mapinfoBackButton)
			{
				mGameGui->changeCurrentGui(GameGUI::GT_PAUSE_MENU);
				mGameGui->mapinfoBackButton = false;
			}
			if (mGameGui->mapinfoOkButton)
			{
				mGameGui->onMapInfoFinish(mGenerator->getFormat());
				mGameGui->changeCurrentGui(GameGUI::GT_PAUSE_MENU);
				mGameGui->mapinfoOkButton = false;
			}
		}
	
	
		if (!mPause)
		{
			Physics::getSingleton()->getWorld()->stepSimulation(evt2.timeSinceLastFrame, 10, btScalar(1.)/btScalar(8 * 60.));
			
			if (mGenerator->getGeneratorType() == WorldGenerator::WGT_WORLD_EDITOR)
			{
				if (Keys::singleton->isKeyDown(Keys::KeyName::K_EDITOR_TOGGLE_PLAYER) && !Keys::singleton->isKeyDownLastUpdate(Keys::KeyName::K_EDITOR_TOGGLE_PLAYER))
				{
					if (mUsingEditorPlayer)
					{
						mPlayer->_onPlayerToggle();
						if (mPlayer->isDead())
						{
							mGameGui->changeCurrentGui(GameGUI::GT_DEATH);
						}
						else
						{
							mGameGui->changeCurrentGui(GameGUI::GT_PLAYING);
						}
					}
					else
					{
						mEditorPlayer->onPlayerToggle();
						mGameGui->changeCurrentGui(GameGUI::GT_EDITING_WORLD);
					}

					mUsingEditorPlayer = !mUsingEditorPlayer;
					static_cast<WorldEditor*>(mGenerator)->setEditing(mUsingEditorPlayer);
				}

				if (mUsingEditorPlayer)
				{
					mEditorPlayer->update(evt2, mKeyboard, mMouse);
				}
				else
				{
					mPlayer->update(evt2);
				}
			}
			else
			{
				mPlayer->update(evt2);
			}

			bool alreadyUsedPlayingGui = false;
			std::vector<Player*> aa;
			aa.push_back(mPlayer);
			mGenerator->update(evt2, &aa);
			mSoundGenerator->update(mPlayer);
			//Physics::getSingleton()->getWorld()->setDebugDrawer(
			Physics::getSingleton()->getWorld()->debugDrawWorld();
			
			if (mGameState->hasTime)
			{
				mGameState->timeLeft -= evt2.timeSinceLastFrame;
				if (mGameState->timeLeft <= 0)
				{
					mGameState->timeLeft = 0;
				}
//				mGameGui->setPlayingTimeLeft(mGameState->timeLeft);
				if (mGameState->timeLeft <= 0 && !mPlayer->isDead())
				{
					mGameGui->changeCurrentGui(GameGUI::GT_DEATH);
					_::stats.totalActions.deaths++;
					mPlayer->killCharacter();
					mGenerator->newCharacterDied(mPlayer);
					mTimeSinceForceRespawn = 0;
				}
			}

			mGameState->timeSincePlaying += evt2.timeSinceLastFrame;
			if (!mPlayer->isDead())// && 
			{
				//if (mGameState->gameType == MapFormat::GT_REACH_GOAL && !mGameState->hasReachedGoal)
				//{
				//	if (mGenerator->isOnTopOfGoal(mPlayer))
				//	{
				//		mPlayer->onGoalReaching();
				//		mGameState->hasReachedGoal = true;
				//		mGameGui->changeCurrentGui(GameGUI::GT_MAP_FINISHED);
				//		mGameGui->onGameFinished("Voxel 1", "Voxel 2", Keys::key2string(Keys::K_ACTION, _::options), mGameState->timeSincePlaying);
				//	}
				//}

				if (!mGenerator->isInsideWorld(mPlayer->getAabbMax(), (mPlayer->getAabbMin())) || mPlayer->shouldKillPlayer)
				{
					mGameGui->changeCurrentGui(GameGUI::GT_DEATH);
					mPlayer->killCharacter();
					_::stats.totalActions.deaths++;
					mGenerator->newCharacterDied(mPlayer);
					mTimeSinceForceRespawn = 0;
				}

				if (ENABLE_FORCE_RESPAWN && Keys::singleton->isKeyDown(Keys::K_FORCE_RESPAWN) && (mTimeSinceForceRespawn > 0.0 || !Keys::singleton->isKeyDownLastUpdate(Keys::K_FORCE_RESPAWN)))
				{
					mTimeSinceForceRespawn += evt2.timeSinceLastFrame;
					if (!alreadyUsedPlayingGui)
					{
						char buffer[32];
						_snprintf(buffer, 32, "%.1f", PLAYER_TIME_TO_FORCE_RESPAWN - mTimeSinceForceRespawn);
						mGameGui->updatePlayingGuiBottom(String(buffer) + " seconds left to force death");
						alreadyUsedPlayingGui = true;
					}
					if (mTimeSinceForceRespawn >= PLAYER_TIME_TO_FORCE_RESPAWN)
					{
						mGameGui->changeCurrentGui(GameGUI::GT_DEATH);
						mPlayer->killCharacter();
						_::stats.totalActions.deaths++;
						mGenerator->newCharacterDied(mPlayer);
						mTimeSinceForceRespawn = 0;
					}
				}
				else
				{
					mTimeSinceForceRespawn = 0;
				}

				WorldActivator* respawn = mGenerator->getRespawnAt(mPlayer->getAabbMax(), mPlayer->getAabbMin());
				if (respawn != 0)
				{
					if (!alreadyUsedPlayingGui)
					{
						if (!respawn->isActivated())
						{
							mGameGui->updatePlayingGuiBottom("Press " + String(Keys::key2string(Keys::K_ACTION, _::options)) + " to activate the respawn point");
							alreadyUsedPlayingGui = true;
						}
						else
						{
							mGameGui->updatePlayingGuiBottom("Respawn point is active");
							alreadyUsedPlayingGui = true;
						}
					}

					if (!respawn->isActivated() && Keys::singleton->isKeyDown(Keys::K_ACTION) && !Keys::singleton->isKeyDownLastUpdate(Keys::K_ACTION))
					{
						mGenerator->activateRespawn(respawn);
						mPlayer->setRespawnPoint(mGenerator->worldRespawn2vector(respawn));
					}
				}
				WorldActivator* point = mGenerator->getPointAt(mPlayer->getAabbMax(), mPlayer->getAabbMin());
				if (point)
				{
					if (!alreadyUsedPlayingGui)
					{
						if (!point->isActivated())
						{
							mGameGui->updatePlayingGuiBottom("Press " + String(Keys::key2string(Keys::K_ACTION, _::options)) + " to activate the activator");
							alreadyUsedPlayingGui = true;
						}
						else
						{
							mGameGui->updatePlayingGuiBottom("Activator is active");
							alreadyUsedPlayingGui = true;
						}
					}

					if (!point->isActivated() && Keys::singleton->isKeyDown(Keys::K_ACTION) && !Keys::singleton->isKeyDownLastUpdate(Keys::K_ACTION))
					{
						point->setActivation(true);
					}
				}

				auto area = mGenerator->getCurrentMapZone(mPlayer->getBodyNode()->_getDerivedPosition());
				if (area != mCurrentMapZone)
				{
					mCurrentMapZone = area;
					mTimeOnArea = 0;
				}
				mTimeOnArea += evt.timeSinceLastFrame;
				mGameGui->setPlayingScore(mGameState->currentScore, mGameState->maxScore, mGameState->difficulty);
				mGameGui->updatePlayingGuiMap(mTimeOnArea, mGenerator->mapzone2string(mCurrentMapZone));
			}
			else
			{
				mGameGui->updateDeathGui(mGameState->timeSincePlaying, mPlayer->getTimeUntilRespawn(), mGenerator->getFormat());
				if (mPlayer->getTimeUntilRespawn() <= 0.0 && (Keys::singleton->isKeyDown(Keys::K_ACTION) || Keys::singleton->isKeyDown(Keys::K_FORCE_RESPAWN)))
				{
					mGameGui->changeCurrentGui(GameGUI::GT_PLAYING);
					if (mGenerator->getGeneratorType() == WorldGenerator::WGT_INFINITE_PATH)
					{
						mPlayer->setRespawnPoint(mGenerator->worldRespawn2vector(0));
					}
					mPlayer->respawnCharacter();
					mGenerator->newCharacterRespawn(mPlayer);

					if (mGameState->hasTime)
					{
						mGameState->timeLeft = mGenerator->calculateTimeLeft();
					}

				}

				//TextRenderer::getSingleton().printf("textbox-1", "false");
			}
			//Vector3 vector = mPlayer->getBodyNode()->_getDerivedPosition();


			if (!mPlayer->isDead())
			{
			}

			if (!alreadyUsedPlayingGui)
			{
				mGameGui->updatePlayingGuiBottom("");
			}

			DebugDrawer::getSingleton().build();
		}
		else
		{
		}

		mGameGui->allFlagsToFalse();


		if (mGameGui->getCurrentGui() == GameGUI::GT_SET_MAP_INFO) mGameGui->updateMapInfo();

		break;
	}

    return true;
}

void Playground::configHelper()
{
    RenderSystem *renderSystem;
    RenderSystemList renderers = Root::getSingleton().getAvailableRenderers();
	String string("OpenGL Rendering SubSystem");

    for(RenderSystemList::iterator it = renderers.begin();
        it != renderers.end(); it++)
    {
        renderSystem = (*it);
		Ogre::String aaa = renderSystem->getName();
		if((renderSystem->getName().compare(string)) == 0)
        {
            break;
        }
    }

    Root::getSingleton().setRenderSystem(renderSystem);

    //renderSystem->setConfigOption("Video Mode", "1680x1050");
    //renderSystem->setConfigOption("Full Screen", "Yes");
    renderSystem->setConfigOption("VSync", "Yes");
}