#pragma once
#include "stdafx.h"
#include "WorldGenerator.h"
#include "InfinitePathWorldGenerator.h"
#include "Globals.h"
#include "MapCollection.h"
#include "balance.h"
#include "RenderInterfaceOgre3D.h"
#include "SystemInterfaceOgre3D.h"
#include "SoundManager.h"
#include "MapFormat.h"


class GameGUI;

class ButtonClickHandler : public Rocket::Core::EventListener
{
public:
	ButtonClickHandler(GameGUI* gameGui)
		: mGameGui(gameGui)
	{
	}
	virtual void ProcessEvent(Rocket::Core::Event& event);
private:
	GameGUI* mGameGui;
};

class GameGUI : public Ogre::RenderQueueListener, public OIS::MouseListener, public OIS::JoyStickListener
{
public:
	enum GuiType
	{
		GT_MENU_START,
		GT_NONE,
		GT_DEATH,
		GT_PLAYING,
		GT_DIFFICULTY_SELECT,
		GT_PAUSE_MENU,
		GT_INGAME_OPTIONS,
		GT_MAP_FINISHED,
		GT_SELECT_THEME,
		GT_SET_MAP_INFO,
		GT_EDITING_WORLD,
		GT_SIZE
	};
	GameGUI(SoundManager*, int width, int height, SceneManager*);
	~GameGUI(void);
	void allFlagsToFalse();
	void clearAllEditorGUI();
	void onMapInfoStart(MapFormat*);
	void onMapInfoFinish(MapFormat*);
	void updateMapInfo();
	void updateEditorSides(int platforms, String& mapName, std::vector<Platform*>&);
	void setEditorTop1(Vector3);
	void setEditorTop1(String str)
	{
		mEditorText[K_TOP_1]->SetText(str.c_str());
	}
	void setEditorTop2(String str)
	{
		mEditorText[K_TOP_2]->SetText(str.c_str());
	}
	void setEditorBottom1(String str)
	{
		mEditorText[K_BOTTOM_1]->SetText(str.c_str());
	}
	void setEditorBottom2(String str)
	{
		mEditorText[K_BOTTOM_2]->SetText(str.c_str());
	}
	void setEditingCrosshairVisible(bool visible)
	{
		Rocket::Core::Element* helper = mGuis[GT_EDITING_WORLD]->GetElementById("crosshair");
		if (visible && !helper->IsVisible())
		{
			helper->SetProperty("visibility", "visible");
		}
		if (!visible && helper->IsVisible())
		{
			helper->SetProperty("visibility", "hidden");
		}
	}
	void updatePlayingGuiMap(double timeSinceMap, String mapName);
	void updateIngameOptionsGui();
	void updateDeathGui(Real timeSincePlaying, Real respawnTime, MapFormat*);
	void onGameFinished(String mapName, String nextMapName, String actionKeyName, double timeFinish);
	void changeCurrentGui(GuiType);
	void updatePlayingGuiBottom(String) { /* not implemented */ }

	bool
		singlePlayerClick,
		difficultyVeryEasyClick,
		difficultyEasyClick,
		difficultyNormalClick,
		difficultyHardClick,
		difficultyVeryHardClick,
		difficultyBackClick,
		multiplayerClick,
		worldEditorClick,
		singlePlayerMapSelectClick,
		singlePlayerBackClick,
		singlePlayerOKClick,
		pauseOptionClick,
		pauseGoBackToMapClick,
		pauseQuitClick,
		pauseFullscreenClick,
		ingameOkClick,
		ingameCancelClick,
		pauseChangeThemeClick,
		pauseMapInfo,
		themeBackButton,
		mapinfoOkButton,
		mapinfoBackButton,
		tutorialClick;

	void setMouseEnable(bool enable);
	void setAllMaps(MapCollection::MapDirectory&);
	void onInputRecreation(OIS::Keyboard* keyboard, OIS::Mouse* mouse, OIS::JoyStick*);
	ConfigOptions getGUISettings();
	void setGUISettings(ConfigOptions&);
	void reloadIngameControlMapping(bool keyboardAndMouse);
	void onWindowResize(int width, int height)
	{
		mWindowWidth = width;
		mWindowHeight = height;
		mRenderInterfaceOgre3D->EnableScissorRegion(true);
		mRenderInterfaceOgre3D->SetScissorRegion(0, 0, width, height);
		mContext->SetDimensions(Rocket::Core::Vector2i(width, height));
	}
	GuiType getCurrentGui()
	{
		return mCurrentGui;
	}
	SoundManager* getSoundManager()
	{
		return mSoundManager;
	}
	void setPlayingScore(int current, int max, GameState::Difficulty difficulty);
	Rocket::Core::Context* getContext()
	{
		return mContext;
	}
protected:
	void hideAllGUIs();
	void ConfigureRenderSystem();

	virtual void renderQueueStarted(uint8 queueGroupId, const Ogre::String& invocation, bool& skipThisInvocation);
    virtual void renderQueueEnded(uint8 queueGroupId, const Ogre::String& invocation, bool& repeatThisInvocation);
	
	virtual bool buttonPressed( const OIS::JoyStickEvent &arg, int button );
	virtual bool buttonReleased( const OIS::JoyStickEvent &arg, int button );
	virtual bool axisMoved( const OIS::JoyStickEvent &arg, int axis );

	virtual bool mouseMoved(const OIS::MouseEvent& e);
	virtual bool mousePressed(const OIS::MouseEvent& e, OIS::MouseButtonID id);
	virtual bool mouseReleased(const OIS::MouseEvent& e, OIS::MouseButtonID id);
	int GetKeyModifierState();
private:
	SoundManager* mSoundManager;
	GuiType mCurrentGui;
	Rocket::Core::ElementText* mPlayingBottomScore1, *mPlayingBottomScore2, *mPlayingBottomScore3;
	Rocket::Core::ElementText *mMouseSensitivtySpanTextElement, *mDeathCamRespawnTimeLeft, *mDeathCamExtra, *mDeathCamMapName;
	enum EditorLabels
	{
		K_TOP_1,
		K_TOP_2,
		K_BOTTOM_1,
		K_BOTTOM_2,
		K_PLATFORMS_LEFT,
		K_NAME,
		K_WORLD_SIZE,
		K_PLATFORM_1,
		K_PLATFORM_2,
		K_PLATFORM_3,
		K_PLATFORM_4,
		K_PLATFORM_5,
		K_PLATFORM_6,
		K_PLATFORM_7,
		K_LABELS_SIZE
	};
	Rocket::Core::ElementText* mEditorText[K_LABELS_SIZE];
	int mWindowWidth, mWindowHeight;
	int joystickAbsoluteX, joystickAbsoluteY;
	MapCollection::MapDirectory* mDirectories;
	ButtonClickHandler* mButtonClickHandler;
	OIS::Keyboard* mKeyboard;
	OIS::Mouse* mMouse;
	OIS::JoyStick* mJoyStick;
	//Rocket::Core::ElementDocument* mCursor;
	Rocket::Core::Context* mContext;
	//Rocket::Core::ElementDocument* mDeathGui, *mStartGui, *mSinglePlayerSelect, *mPlayingGui, *mPauseGui, *mIngameOptions;
	Rocket::Core::ElementDocument* mGuis[GT_SIZE];
	RenderInterfaceOgre3D* mRenderInterfaceOgre3D;
	SystemInterfaceOgre3D* mSystemInterfaceOgre3D;
};

