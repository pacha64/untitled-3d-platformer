#include "GameGUI.h"
#include "Keys.h"
#include "TextRenderer.h"
#include <stdio.h>
#include <stdlib.h>

void ButtonClickHandler::ProcessEvent(Rocket::Core::Event& event)
{
	if (event.GetCurrentElement()->GetId() == "button-single-player")
	{
		mGameGui->singlePlayerClick = true;
	}
	else if (event.GetCurrentElement()->GetId() == "button-select-map")
	{
		mGameGui->singlePlayerMapSelectClick = true;
	}
	else if (event.GetCurrentElement()->GetId() == "button-tutorial")
	{
		mGameGui->tutorialClick = true;
	}
	else if (event.GetCurrentElement()->GetId() == "button-world-editor")
	{
		mGameGui->worldEditorClick = true;
	}
	// select-difficulty
	else if (event.GetCurrentElement()->GetId() == "lvl-d-very-easy")
	{
		mGameGui->difficultyVeryEasyClick = true;
	}
	else if (event.GetCurrentElement()->GetId() == "lvl-d-easy")
	{
		mGameGui->difficultyEasyClick = true;
	}
	else if (event.GetCurrentElement()->GetId() == "lvl-d-normal")
	{
		mGameGui->difficultyNormalClick = true;
	}
	else if (event.GetCurrentElement()->GetId() == "lvl-d-hard")
	{
		mGameGui->difficultyHardClick = true;
	}
	else if (event.GetCurrentElement()->GetId() == "lvl-d-very-hard")
	{
		mGameGui->difficultyVeryHardClick = true;
	}
	else if (event.GetCurrentElement()->GetId() == "lvl-d-back")
	{
		mGameGui->difficultyBackClick = true;
	}
	// pause
	else if (event.GetCurrentElement()->GetId() == "pause-go-back-to-map")
	{
		mGameGui->pauseGoBackToMapClick = true;
	}
	else if (event.GetCurrentElement()->GetId() == "pause-quit-to-menu")
	{
		mGameGui->pauseQuitClick = true;
	}
	else if (event.GetCurrentElement()->GetId() == "pause-options")
	{
		mGameGui->pauseOptionClick = true;
	}
	else if (event.GetCurrentElement()->GetId() == "pause-change-theme")
	{
		mGameGui->pauseChangeThemeClick = true;
	}
	else if (event.GetCurrentElement()->GetId() == "pause-fullscreen")
	{
		mGameGui->pauseFullscreenClick = true;
	}
	// ingame options
	else if (event.GetCurrentElement()->GetId() == "igo-cancel-button")
	{
		mGameGui->ingameCancelClick = true;
	}
	else if (event.GetCurrentElement()->GetId() == "igo-ok-button")
	{
		mGameGui->ingameOkClick = true;
	}
	else if (event.GetCurrentElement()->GetId() == "pause-map-info")
	{
		mGameGui->pauseMapInfo = true;
	}
	else if (event.GetCurrentElement()->GetId() == "theme-back-button")
	{
		mGameGui->themeBackButton = true;
	}
	else if (event.GetCurrentElement()->GetId() == "map-info-back-button")
	{
		mGameGui->mapinfoBackButton = true;
	}
	else if (event.GetCurrentElement()->GetId() == "map-info-ok-button")
	{
		mGameGui->mapinfoOkButton = true;
	}
	else if (event.GetCurrentElement()->GetClassNames() == "theme-individual")
	{
		if (!event.GetCurrentElement()->GetId().Empty())
		{
			_::options.changeSkin(String(event.GetCurrentElement()->GetId().CString()));
		}
	}
	else
	{
		mGameGui->getSoundManager()->playNewSound2D(SoundManager::GUI_CLICK);
	}
	// pauseGoBackToMapClick pauseOptionClick pauseQuitClick
}

GameGUI::GameGUI(SoundManager* soundManager, int width, int height, SceneManager* sceneManager)
	: mSoundManager(soundManager),
	mMouse(0),
	mKeyboard(0),
	mJoyStick(0),
	mWindowWidth(width),
	mWindowHeight(height),
	joystickAbsoluteX(mWindowWidth / 2),
	joystickAbsoluteY(mWindowHeight / 2),
	singlePlayerClick(false),
	difficultyVeryEasyClick(false),
	difficultyEasyClick(false),
	difficultyNormalClick(false),
	difficultyHardClick(false),
	difficultyVeryHardClick(false),
	difficultyBackClick(false),
	multiplayerClick(false),
	singlePlayerMapSelectClick(false),
	singlePlayerBackClick(false),
	singlePlayerOKClick(false),
	pauseGoBackToMapClick(false),
	pauseOptionClick(false),
	pauseQuitClick(false),
	pauseFullscreenClick(false),
	ingameOkClick(false),
	ingameCancelClick(false),
	pauseChangeThemeClick(false),
	pauseMapInfo(false),
	themeBackButton(false),
	mapinfoOkButton(false),
	mapinfoBackButton(false),
	worldEditorClick(false),
	tutorialClick(false),
	mButtonClickHandler(0),
	mMouseSensitivtySpanTextElement(0),
	//mCursor(0),
	mPlayingBottomScore1(0),
	mPlayingBottomScore2(0),
	mPlayingBottomScore3(0),
	mDeathCamRespawnTimeLeft(0),
	mDeathCamExtra(0),
	mDeathCamMapName(0)
{
	for (int i = 0; i < GT_SIZE; i++)
	{
		mGuis[i] = 0;
	}

	mRenderInterfaceOgre3D = new RenderInterfaceOgre3D(mWindowWidth, mWindowHeight);
	mSystemInterfaceOgre3D = new SystemInterfaceOgre3D();

	Rocket::Core::SetRenderInterface(mRenderInterfaceOgre3D);
	Rocket::Core::SetSystemInterface(mSystemInterfaceOgre3D);

	Rocket::Core::Initialise();
	Rocket::Controls::Initialise();

	Rocket::Core::FontDatabase::LoadFontFace(
		"resources/gui/FreePixel.ttf"
	);

	//Rocket::Core::FontDatabase::LoadFontFace(
	//	"resources/gui/Inconsolata-bold.ttf"/*,
	//	"Inconsolata",
	//	Rocket::Core::Font::Style::STYLE_NORMAL,
	//	Rocket::Core::Font::Weight::WEIGHT_BOLD*/
	//);
	mContext = Rocket::Core::CreateContext("main", Rocket::Core::Vector2i(mWindowWidth, mWindowHeight));

	mGuis[GT_DEATH] = mContext->LoadDocument("resources/gui/death-cam.rml");
	if (mGuis[GT_DEATH])
	{
		mGuis[GT_DEATH]->RemoveReference();
	}
	mGuis[GT_MENU_START] = mContext->LoadDocument("resources/gui/start.rml");
	if (mGuis[GT_MENU_START])
	{
		mGuis[GT_MENU_START]->RemoveReference();
	}
	mGuis[GT_DIFFICULTY_SELECT] = mContext->LoadDocument("resources/gui/select-difficulty.rml");
	if (mGuis[GT_DIFFICULTY_SELECT])
	{
		mGuis[GT_DIFFICULTY_SELECT]->RemoveReference();
	}
	mGuis[GT_PLAYING] = mContext->LoadDocument("resources/gui/playing.rml");
	if (mGuis[GT_PLAYING])
	{
		mGuis[GT_PLAYING]->RemoveReference();
	}
	mGuis[GT_PAUSE_MENU] = mContext->LoadDocument("resources/gui/pause.rml");
	if (mGuis[GT_PAUSE_MENU])
	{
		mGuis[GT_PAUSE_MENU]->RemoveReference();
	}
	mGuis[GT_SELECT_THEME] = mContext->LoadDocument("resources/gui/change-theme.rml");
	if (mGuis[GT_SELECT_THEME])
	{
		mGuis[GT_SELECT_THEME]->RemoveReference();
	}
	mGuis[GT_INGAME_OPTIONS] = mContext->LoadDocument("resources/gui/ingame-options.rml");
	if (mGuis[GT_INGAME_OPTIONS])
	{
		mGuis[GT_INGAME_OPTIONS]->RemoveReference();
	}
	mGuis[GT_SET_MAP_INFO] = mContext->LoadDocument("resources/gui/set-map-info.rml");
	if (mGuis[GT_SET_MAP_INFO])
	{
		mGuis[GT_SET_MAP_INFO]->RemoveReference();
	}
	mGuis[GT_MAP_FINISHED] = mContext->LoadDocument("resources/gui/map-finished.rml");
	if (mGuis[GT_MAP_FINISHED])
	{
		mGuis[GT_MAP_FINISHED]->RemoveReference();
	}
	mGuis[GT_EDITING_WORLD] = mContext->LoadDocument("resources/gui/editing-world.rml");
	if (mGuis[GT_EDITING_WORLD])
	{
		mGuis[GT_EDITING_WORLD]->RemoveReference();
	}
	
	mButtonClickHandler = new ButtonClickHandler(this);
	
	mGuis[GT_MENU_START]->AddEventListener("click", mButtonClickHandler);
	mGuis[GT_DIFFICULTY_SELECT]->AddEventListener("click", mButtonClickHandler);
	mGuis[GT_PAUSE_MENU]->AddEventListener("click", mButtonClickHandler);
	mGuis[GT_INGAME_OPTIONS]->AddEventListener("click", mButtonClickHandler);
	mGuis[GT_SELECT_THEME]->AddEventListener("click", mButtonClickHandler);
	mGuis[GT_SET_MAP_INFO]->AddEventListener("click", mButtonClickHandler);
	
	// start
	Rocket::Core::Element* helper = mGuis[GT_MENU_START]->GetElementById("button-single-player");
	helper->AddEventListener("click", mButtonClickHandler);
	//helper = mGuis[GT_MENU_START]->GetElementById("button-tutorial");
	//helper->AddEventListener("click", mButtonClickHandler);
	//helper = mGuis[GT_MENU_START]->GetElementById("button-select-map");
	//helper->AddEventListener("click", mButtonClickHandler);
	helper = mGuis[GT_MENU_START]->GetElementById("button-world-editor");
	helper->AddEventListener("click", mButtonClickHandler);

	// single-player
	helper = mGuis[GT_DIFFICULTY_SELECT]->GetElementById("lvl-d-very-easy");
	helper->AddEventListener("click", mButtonClickHandler);
	helper = mGuis[GT_DIFFICULTY_SELECT]->GetElementById("lvl-d-easy");
	helper->AddEventListener("click", mButtonClickHandler);
	helper = mGuis[GT_DIFFICULTY_SELECT]->GetElementById("lvl-d-normal");
	helper->AddEventListener("click", mButtonClickHandler);
	helper = mGuis[GT_DIFFICULTY_SELECT]->GetElementById("lvl-d-hard");
	helper->AddEventListener("click", mButtonClickHandler);
	helper = mGuis[GT_DIFFICULTY_SELECT]->GetElementById("lvl-d-very-hard");
	helper->AddEventListener("click", mButtonClickHandler);
	helper = mGuis[GT_DIFFICULTY_SELECT]->GetElementById("lvl-d-back");
	helper->AddEventListener("click", mButtonClickHandler);
	
	// pause
	helper = mGuis[GT_PAUSE_MENU]->GetElementById("pause-go-back-to-map");
	helper->AddEventListener("click", mButtonClickHandler);
	helper = mGuis[GT_PAUSE_MENU]->GetElementById("pause-quit-to-menu");
	helper->AddEventListener("click", mButtonClickHandler);
	//helper = mGuis[GT_PAUSE_MENU]->GetElementById("pause-options");
	//helper->AddEventListener("click", mButtonClickHandler);
	helper = mGuis[GT_PAUSE_MENU]->GetElementById("pause-fullscreen");
	helper->AddEventListener("click", mButtonClickHandler);
	//helper = mGuis[GT_PAUSE_MENU]->GetElementById("pause-change-theme");
	//helper->AddEventListener("click", mButtonClickHandler);
	//helper = mGuis[GT_PAUSE_MENU]->GetElementById("pause-map-info");
	//helper->AddEventListener("click", mButtonClickHandler);

	// theme
	Rocket::Core::ElementList themeList;
	helper = mGuis[GT_SELECT_THEME]->GetElementById("theme-back-button");
	helper->AddEventListener("click", mButtonClickHandler);
	mGuis[GT_SELECT_THEME]->GetElementsByClassName(themeList, "theme-individual");
	for (int i = 0; i < themeList.size(); i++)
	{
		themeList[i]->AddEventListener("click", mButtonClickHandler);
	}

	// ingame-options
	helper = mGuis[GT_INGAME_OPTIONS]->GetElementById("igo-cancel-button");
	helper->AddEventListener("click", mButtonClickHandler);
	helper = mGuis[GT_INGAME_OPTIONS]->GetElementById("igo-ok-button");
	helper->AddEventListener("click", mButtonClickHandler);
	
	helper = (mGuis[GT_SET_MAP_INFO]->GetElementById("name"));
	helper->SetAttribute("maxlength", Ogre::StringConverter::toString(MAP_NAME_MAX_LENGTH).c_str());
	helper = (mGuis[GT_SET_MAP_INFO]->GetElementById("description"));
	helper->SetAttribute("maxlength", Ogre::StringConverter::toString(MAP_DESCRIPTION_MAX_LENGTH).c_str());
	helper = mGuis[GT_SET_MAP_INFO]->GetElementById("map-info-back-button");
	helper->AddEventListener("click", mButtonClickHandler);
	helper = mGuis[GT_SET_MAP_INFO]->GetElementById("map-info-ok-button");
	helper->AddEventListener("click", mButtonClickHandler);

	setGUISettings(_::options);

	memset(mEditorText, 0, sizeof(mEditorText));

	mEditorText[K_BOTTOM_1] = mGuis[GT_EDITING_WORLD]->CreateTextNode("");
	helper = mGuis[GT_EDITING_WORLD]->GetElementById("bottom-text-1");
	helper->AppendChild(mEditorText[K_BOTTOM_1]);
	mEditorText[K_BOTTOM_2] = mGuis[GT_EDITING_WORLD]->CreateTextNode("");
	helper = mGuis[GT_EDITING_WORLD]->GetElementById("bottom-text-2");
	helper->AppendChild(mEditorText[K_BOTTOM_2]);

	mEditorText[K_TOP_1] = mGuis[GT_EDITING_WORLD]->CreateTextNode("");
	helper = mGuis[GT_EDITING_WORLD]->GetElementById("top-text-1");
	helper->AppendChild(mEditorText[K_TOP_1]);
	mEditorText[K_TOP_2] = mGuis[GT_EDITING_WORLD]->CreateTextNode("");
	helper = mGuis[GT_EDITING_WORLD]->GetElementById("top-text-2");
	helper->AppendChild(mEditorText[K_TOP_2]);

	mEditorText[K_PLATFORMS_LEFT] = mGuis[GT_EDITING_WORLD]->CreateTextNode("");
	helper = mGuis[GT_EDITING_WORLD]->GetElementById("platforms");
	helper->AppendChild(mEditorText[K_PLATFORMS_LEFT]);
	mEditorText[K_NAME] = mGuis[GT_EDITING_WORLD]->CreateTextNode("");
	helper = mGuis[GT_EDITING_WORLD]->GetElementById("map-name");
	helper->AppendChild(mEditorText[K_NAME]);
	mEditorText[K_WORLD_SIZE] = mGuis[GT_EDITING_WORLD]->CreateTextNode("");
	helper = mGuis[GT_EDITING_WORLD]->GetElementById("world-size");
	helper->AppendChild(mEditorText[K_WORLD_SIZE]);
	
	mEditorText[K_PLATFORM_1] = mGuis[GT_EDITING_WORLD]->CreateTextNode("");
	helper = mGuis[GT_EDITING_WORLD]->GetElementById("platform-1");
	helper->AppendChild(mEditorText[K_PLATFORM_1]);
	mEditorText[K_PLATFORM_2] = mGuis[GT_EDITING_WORLD]->CreateTextNode("");
	helper = mGuis[GT_EDITING_WORLD]->GetElementById("platform-2");
	helper->AppendChild(mEditorText[K_PLATFORM_2]);
	mEditorText[K_PLATFORM_3] = mGuis[GT_EDITING_WORLD]->CreateTextNode("");
	helper = mGuis[GT_EDITING_WORLD]->GetElementById("platform-3");
	helper->AppendChild(mEditorText[K_PLATFORM_3]);
	mEditorText[K_PLATFORM_4] = mGuis[GT_EDITING_WORLD]->CreateTextNode("");
	helper = mGuis[GT_EDITING_WORLD]->GetElementById("platform-4");
	helper->AppendChild(mEditorText[K_PLATFORM_4]);
	mEditorText[K_PLATFORM_5] = mGuis[GT_EDITING_WORLD]->CreateTextNode("");
	helper = mGuis[GT_EDITING_WORLD]->GetElementById("platform-5");
	helper->AppendChild(mEditorText[K_PLATFORM_5]);
	mEditorText[K_PLATFORM_6] = mGuis[GT_EDITING_WORLD]->CreateTextNode("");
	helper = mGuis[GT_EDITING_WORLD]->GetElementById("platform-6");
	helper->AppendChild(mEditorText[K_PLATFORM_6]);
	mEditorText[K_PLATFORM_7] = mGuis[GT_EDITING_WORLD]->CreateTextNode("");
	helper = mGuis[GT_EDITING_WORLD]->GetElementById("platform-7");
	helper->AppendChild(mEditorText[K_PLATFORM_7]);


	mPlayingBottomScore1 = mGuis[GT_PLAYING]->CreateTextNode("");
	helper = mGuis[GT_PLAYING]->GetElementById("score-1");
	helper->AppendChild(mPlayingBottomScore1);
	mPlayingBottomScore2 = mGuis[GT_PLAYING]->CreateTextNode("");
	helper = mGuis[GT_PLAYING]->GetElementById("score-2");
	helper->AppendChild(mPlayingBottomScore2);
	mPlayingBottomScore3 = mGuis[GT_PLAYING]->CreateTextNode("");
	helper = mGuis[GT_PLAYING]->GetElementById("score-3");
	helper->AppendChild(mPlayingBottomScore3);

	mDeathCamRespawnTimeLeft = mGuis[GT_DEATH]->CreateTextNode("");
	helper = mGuis[GT_DEATH]->GetElementById("time-left");
	helper->AppendChild(mDeathCamRespawnTimeLeft);

	mDeathCamMapName = mGuis[GT_DEATH]->CreateTextNode("");
	helper = mGuis[GT_DEATH]->GetElementById("map-name");
	helper->AppendChild(mDeathCamMapName);

	mDeathCamExtra = mGuis[GT_DEATH]->CreateTextNode("");
	helper = mGuis[GT_DEATH]->GetElementById("extra");
	helper->AppendChild(mDeathCamExtra);

	mMouseSensitivtySpanTextElement = mGuis[GT_INGAME_OPTIONS]->CreateTextNode("");
	Rocket::Core::Element* elementHelper = mGuis[GT_INGAME_OPTIONS]->GetElementById("mouse-sensitivity-span");
	elementHelper->AppendChild(mMouseSensitivtySpanTextElement);

	reloadIngameControlMapping(_::options.useKeyboardAndMouse);

	//mCursor = mContext->LoadMouseCursor("D:/platformer/resources/gui/cursor.rml");
	//if (mCursor)
	//{
	//	mCursor->RemoveReference();
	//}

	changeCurrentGui(GuiType::GT_MENU_START);
	//mGuis[GT_DEATH]->Show();
	//mGuis[GT_DEATH]->RemoveReference();

	sceneManager->addRenderQueueListener(this);
}

GameGUI::~GameGUI(void)
{
	//context->RemoveReference();
	//Rocket::Core::Shutdown();

	//delete ogre_system;
	//ogre_system = NULL;

	//delete ogre_renderer;
	//ogre_renderer = NULL;
	mContext->RemoveReference();
	Rocket::Core::Shutdown();
	delete mSystemInterfaceOgre3D;
	delete mRenderInterfaceOgre3D;
}

void GameGUI::reloadIngameControlMapping(bool keyboardAndMouse)
{
	Rocket::Core::Element* elementHelper = mGuis[GT_INGAME_OPTIONS]->GetElementById("movement-forward");
	elementHelper->SetInnerRML(Keys::key2string(Keys::K_MOVEMENT_FORWARD, _::options).c_str());
	elementHelper = mGuis[GT_INGAME_OPTIONS]->GetElementById("movement-backwards");
	elementHelper->SetInnerRML(Keys::key2string(Keys::K_MOVEMENT_BACK, _::options).c_str());
	elementHelper = mGuis[GT_INGAME_OPTIONS]->GetElementById("movement-right");
	elementHelper->SetInnerRML(Keys::key2string(Keys::K_MOVEMENT_RIGHT, _::options).c_str());
	elementHelper = mGuis[GT_INGAME_OPTIONS]->GetElementById("movement-left");
	elementHelper->SetInnerRML(Keys::key2string(Keys::K_MOVEMENT_LEFT, _::options).c_str());
	elementHelper = mGuis[GT_INGAME_OPTIONS]->GetElementById("jump-normal");
	elementHelper->SetInnerRML(Keys::key2string(Keys::K_JUMP_NORMAL, _::options).c_str());
	elementHelper = mGuis[GT_INGAME_OPTIONS]->GetElementById("jump-long");
	elementHelper->SetInnerRML(Keys::key2string(Keys::K_JUMP_LONG, _::options).c_str());
#if ENABLE_BACKWARDS_JUMP
	elementHelper = mGuis[GT_INGAME_OPTIONS]->GetElementById("jump-backwards");
	elementHelper->SetInnerRML(Keys::key2string(Keys::K_JUMP_BACKWARD, _::options).c_str());
#endif
	elementHelper = mGuis[GT_INGAME_OPTIONS]->GetElementById("camera-up");
	elementHelper->SetInnerRML(Keys::key2string(Keys::K_CAMERA_TOP, _::options).c_str());
	elementHelper = mGuis[GT_INGAME_OPTIONS]->GetElementById("camera-down");
	elementHelper->SetInnerRML(Keys::key2string(Keys::K_CAMERA_BOTTOM, _::options).c_str());
	elementHelper = mGuis[GT_INGAME_OPTIONS]->GetElementById("camera-right");
	elementHelper->SetInnerRML(Keys::key2string(Keys::K_CAMERA_RIGHT, _::options).c_str());
	elementHelper = mGuis[GT_INGAME_OPTIONS]->GetElementById("camera-left");
	elementHelper->SetInnerRML(Keys::key2string(Keys::K_CAMERA_LEFT, _::options).c_str());
	elementHelper = mGuis[GT_INGAME_OPTIONS]->GetElementById("camera-reset");
	elementHelper->SetInnerRML(Keys::key2string(Keys::K_CAMERA_ZOOM_RESET, _::options).c_str());
	elementHelper = mGuis[GT_INGAME_OPTIONS]->GetElementById("camera-add");
	elementHelper->SetInnerRML(Keys::key2string(Keys::K_CAMERA_ZOOM_ADD, _::options).c_str());
	elementHelper = mGuis[GT_INGAME_OPTIONS]->GetElementById("camera-reduce");
	elementHelper->SetInnerRML(Keys::key2string(Keys::K_CAMERA_ZOOM_REDUCE, _::options).c_str());
	elementHelper = mGuis[GT_INGAME_OPTIONS]->GetElementById("pause");
	elementHelper->SetInnerRML(Keys::key2string(Keys::K_PAUSE, _::options).c_str());
//	elementHelper = mGuis[GT_INGAME_OPTIONS]->GetElementById("respawn");
//	elementHelper->SetInnerRML(Keys::key2string(Keys::K_RESPAWN, _::options).c_str());
	elementHelper = mGuis[GT_INGAME_OPTIONS]->GetElementById("action");
	elementHelper->SetInnerRML(Keys::key2string(Keys::K_ACTION, _::options).c_str());
}

void GameGUI::setPlayingScore(int current, int max, GameState::Difficulty difficulty)
{
	mPlayingBottomScore1->SetText(Rocket::Core::String("Difficulty: ") + GameState::difficulty2string(difficulty).c_str());
	mPlayingBottomScore2->SetText(Rocket::Core::String("Current score: ") + StringConverter::toString(current).c_str());
	mPlayingBottomScore3->SetText(Rocket::Core::String("Max score: ") + StringConverter::toString(max).c_str());
}

void GameGUI::setGUISettings(ConfigOptions& config)
{
	Rocket::Core::String helper;// = Rocket::Core::String(dynamic_cast<Rocket::Controls::ElementFormControl*>(mGuis[GT_INGAME_OPTIONS]->GetElementById("mouse-sensitivity"))->GetValue());
	Rocket::Core::TypeConverter<int, Rocket::Core::String>().Convert((int)(config.mouseSensitivity * 100), helper);
	dynamic_cast<Rocket::Controls::ElementFormControl*>(mGuis[GT_INGAME_OPTIONS]->GetElementById("mouse-sensitivity"))->SetValue(helper);

	//helper = Rocket::Core::String(dynamic_cast<Rocket::Controls::ElementFormControl*>(mGuis[GT_INGAME_OPTIONS]->GetElementById("sound-effect-level"))->GetValue());
	Rocket::Core::TypeConverter<int, Rocket::Core::String>().Convert((int)(config.soundEffectsLevel * 100), helper);
	dynamic_cast<Rocket::Controls::ElementFormControl*>(mGuis[GT_INGAME_OPTIONS]->GetElementById("sound-effect-level"))->SetValue(helper);
	
	Rocket::Core::Element* elementHelper = mGuis[GT_DEATH]->GetElementById("bottom-respawn-text");
	elementHelper->SetInnerRML(Rocket::Core::String("Press ") + Keys::key2string(Keys::K_ACTION, _::options).c_str() + Rocket::Core::String(" or ") + Keys::key2string(Keys::K_FORCE_RESPAWN, _::options).c_str() + Rocket::Core::String(" to respawn"));

	if (config.invertX)
	{
		static_cast<Rocket::Controls::ElementFormControl*>(mGuis[GT_INGAME_OPTIONS]->GetElementById("mouse-x"))->SetAttribute("checked", "checked");
	}
	else
	{
		static_cast<Rocket::Controls::ElementFormControl*>(mGuis[GT_INGAME_OPTIONS]->GetElementById("mouse-x"))->RemoveAttribute("checked");//, "checked");
	}

	if (config.invertY)
	{
		dynamic_cast<Rocket::Controls::ElementFormControl*>(mGuis[GT_INGAME_OPTIONS]->GetElementById("mouse-y"))->SetAttribute("checked", "checked");
	}
	else
	{
		dynamic_cast<Rocket::Controls::ElementFormControl*>(mGuis[GT_INGAME_OPTIONS]->GetElementById("mouse-y"))->RemoveAttribute("checked");//, "checked");
	}

	if (config.bunnyJumping)
	{
		dynamic_cast<Rocket::Controls::ElementFormControl*>(mGuis[GT_INGAME_OPTIONS]->GetElementById("bunny-hopping"))->SetAttribute("checked", "checked");
	}
	else
	{
		dynamic_cast<Rocket::Controls::ElementFormControl*>(mGuis[GT_INGAME_OPTIONS]->GetElementById("bunny-hopping"))->RemoveAttribute("checked");//, "checked");
	}

	if (config.showHelp)
	{
		dynamic_cast<Rocket::Controls::ElementFormControl*>(mGuis[GT_INGAME_OPTIONS]->GetElementById("show-help"))->SetAttribute("checked", "checked");
	}
	else
	{
		dynamic_cast<Rocket::Controls::ElementFormControl*>(mGuis[GT_INGAME_OPTIONS]->GetElementById("show-help"))->RemoveAttribute("checked");//, "checked");
	}

	if (config.muteSoundEffects)
	{
		dynamic_cast<Rocket::Controls::ElementFormControl*>(mGuis[GT_INGAME_OPTIONS]->GetElementById("mute-sound-effect"))->SetAttribute("checked", "checked");
	}
	else
	{
		dynamic_cast<Rocket::Controls::ElementFormControl*>(mGuis[GT_INGAME_OPTIONS]->GetElementById("mute-sound-effect"))->RemoveAttribute("checked");//, "checked");
	}
}

ConfigOptions GameGUI::getGUISettings()
{
	ConfigOptions toReturn = ConfigOptions();
	Rocket::Controls::ElementFormControl* controlHelper = dynamic_cast<Rocket::Controls::ElementFormControl*>(mGuis[GT_INGAME_OPTIONS]->GetElementById("mouse-sensitivity"));
	Rocket::Core::String stringHelper = controlHelper->GetValue();
	double doubleHelper = 0;
	Rocket::Core::TypeConverter<Rocket::Core::String, double>().Convert(stringHelper, doubleHelper);
	if (doubleHelper > 100)
	{
		doubleHelper = 100;
	}
	if (doubleHelper < 0)
	{
		doubleHelper = 0;
	}
	toReturn.mouseSensitivity = doubleHelper / 100;

	controlHelper = dynamic_cast<Rocket::Controls::ElementFormControl*>(mGuis[GT_INGAME_OPTIONS]->GetElementById("sound-effect-level"));
	stringHelper = controlHelper->GetValue();
	doubleHelper = 0;
	Rocket::Core::TypeConverter<Rocket::Core::String, double>().Convert(stringHelper, doubleHelper);
	if (doubleHelper > 100)
	{
		doubleHelper = 100;
	}
	if (doubleHelper < 0)
	{
		doubleHelper = 0;
	}
	toReturn.soundEffectsLevel = doubleHelper / 100;

	toReturn.invertX = dynamic_cast<Rocket::Controls::ElementFormControl*>(mGuis[GT_INGAME_OPTIONS]->GetElementById("mouse-x"))->GetAttribute("checked") == 0 ? false : true;
	toReturn.invertY = dynamic_cast<Rocket::Controls::ElementFormControl*>(mGuis[GT_INGAME_OPTIONS]->GetElementById("mouse-y"))->GetAttribute("checked") == 0 ? false : true;
	toReturn.bunnyJumping = dynamic_cast<Rocket::Controls::ElementFormControl*>(mGuis[GT_INGAME_OPTIONS]->GetElementById("bunny-hopping"))->GetAttribute("checked") == 0 ? false : true;
	toReturn.showHelp = dynamic_cast<Rocket::Controls::ElementFormControl*>(mGuis[GT_INGAME_OPTIONS]->GetElementById("show-help"))->GetAttribute("checked") == 0 ? false : true;
	toReturn.muteSoundEffects = dynamic_cast<Rocket::Controls::ElementFormControl*>(mGuis[GT_INGAME_OPTIONS]->GetElementById("mute-sound-effect"))->GetAttribute("checked") == 0 ? false : true;
	toReturn.useMouseForCameraMovement = dynamic_cast<Rocket::Controls::ElementFormControl*>(mGuis[GT_INGAME_OPTIONS]->GetElementById("mouse-for-camera-movement"))->GetAttribute("checked") == 0 ? false : true;
	return toReturn;
}

void GameGUI::onInputRecreation(OIS::Keyboard* keyboard, OIS::Mouse* mouse, OIS::JoyStick* joystick)
{
	mMouse = mouse;
	mKeyboard = keyboard;
	mJoyStick = joystick;

	if (mMouse != 0)
	{
		mMouse->setEventCallback(this);
	}
	if (mJoyStick != 0)
	{
		mJoyStick->setEventCallback(this);
	}
}

bool GameGUI::buttonPressed( const OIS::JoyStickEvent &arg, int button )
{
	if (button == 0)
	{
		mContext->ProcessMouseButtonDown(0, GetKeyModifierState());
	}
	return true;
}

bool GameGUI::buttonReleased( const OIS::JoyStickEvent &arg, int button )
{
	if (button == 0)
	{
		mContext->ProcessMouseButtonDown(0, GetKeyModifierState());
	}
	return true;
}

bool GameGUI::axisMoved( const OIS::JoyStickEvent &arg, int axis )
{
	if (axis == 0)
	{
		joystickAbsoluteX;
	}
	return true;
}

void GameGUI::setAllMaps(MapCollection::MapDirectory& directory)
{
	mDirectories = &directory;
}

int GameGUI::GetKeyModifierState()
{
	int modifier_state = 0;

	// TODO fix this
	//if (mKeyboard->isModifierDown(OIS::Keyboard::Ctrl))
	//	modifier_state |= Rocket::Core::Input::KM_CTRL;
	//if (mKeyboard->isModifierDown(OIS::Keyboard::Shift))
	//	modifier_state |= Rocket::Core::Input::KM_SHIFT;
	//if (mKeyboard->isModifierDown(OIS::Keyboard::Alt))
	//	modifier_state |= Rocket::Core::Input::KM_ALT;

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32

	if (GetKeyState(VK_CAPITAL) > 0)
		modifier_state |= Rocket::Core::Input::KM_CAPSLOCK;
	if (GetKeyState(VK_NUMLOCK) > 0)
		modifier_state |= Rocket::Core::Input::KM_NUMLOCK;
	if (GetKeyState(VK_SCROLL) > 0)
		modifier_state |= Rocket::Core::Input::KM_SCROLLLOCK;

#elif OGRE_PLATFORM == OGRE_PLATFORM_LINUX

	XKeyboardState keyboard_state;
	XGetKeyboardControl(DISPLAY!, &keyboard_state);

	if (keyboard_state.led_mask & (1 << 0))
		modifier_state |= Rocket::Core::Input::KM_CAPSLOCK;
	if (keyboard_state.led_mask & (1 << 1))
		modifier_state |= Rocket::Core::Input::KM_NUMLOCK;
	if (keyboard_state.led_mask & (1 << 2))
		modifier_state |= Rocket::Core::Input::KM_SCROLLLOCK;

#endif

	return modifier_state;
}

bool GameGUI::mouseMoved(const OIS::MouseEvent& e)
{
	int key_modifier_state = GetKeyModifierState();

	mContext->ProcessMouseMove(e.state.X.abs, e.state.Y.abs, key_modifier_state);
	if (e.state.Z.rel != 0)
	{
		mContext->ProcessMouseWheel(e.state.Z.rel / -120, key_modifier_state);
	}

	return true;
}

bool GameGUI::mousePressed(const OIS::MouseEvent& ROCKET_UNUSED(e), OIS::MouseButtonID id)
{
	mContext->ProcessMouseButtonDown((int) id, GetKeyModifierState());
	return true;
}

bool GameGUI::mouseReleased(const OIS::MouseEvent& ROCKET_UNUSED(e), OIS::MouseButtonID id)
{
	mContext->ProcessMouseButtonUp((int) id, GetKeyModifierState());
	return true;
}

void GameGUI::changeCurrentGui(GuiType type)
{
	hideAllGUIs();

	mCurrentGui = type;
	if (mGuis[type]) 
	{
		mGuis[type]->Show();
	}
}

void GameGUI::setMouseEnable(bool enable)
{
	mContext->ShowMouseCursor(enable);
	//if (mCursor != 0)
	//{
	//	if (enable)
	//	{
	//		mCursor->Show();
	//	}
	//	else
	//	{
	//		mCursor->Hide();
	//	}
	//}
}

void GameGUI::hideAllGUIs()
{
	for (int i = 0; i < GT_SIZE; i++)
	{
		if (mGuis[i] != 0 && mGuis[i]->IsVisible())
		{
			mGuis[i]->Hide();
		}
	}
}

void GameGUI::allFlagsToFalse()
{
	singlePlayerClick = false;
	multiplayerClick = false;
	worldEditorClick = false;
	singlePlayerMapSelectClick = false;
	singlePlayerBackClick = false;
	singlePlayerOKClick = false;
	pauseOptionClick = false;
	pauseGoBackToMapClick = false;
	pauseQuitClick = false;
	pauseFullscreenClick = false;
	ingameOkClick = false;
	ingameCancelClick = false;
	pauseChangeThemeClick = false;
	pauseMapInfo = false;
	themeBackButton = false;
	mapinfoOkButton = false;
	mapinfoBackButton = false;
}

void GameGUI::clearAllEditorGUI()
{
	for (int i = 0; i < K_LABELS_SIZE; i++) mEditorText[i]->SetText("");
}

void GameGUI::onMapInfoStart(MapFormat* map)
{
	Rocket::Controls::ElementFormControlInput* input = static_cast<Rocket::Controls::ElementFormControlInput*>(mGuis[GT_SET_MAP_INFO]->GetElementById("name"));
	input->SetValue(map->name);
	input = static_cast<Rocket::Controls::ElementFormControlInput*>(mGuis[GT_SET_MAP_INFO]->GetElementById("description"));
	input->SetValue(map->description);
//	dynamic_cast<Rocket::Controls::ElementFormControl*>(mGuis[GT_SET_MAP_INFO]->GetElementById("time"))->SetValue(StringConverter::toString(map->seconds).c_str());
}

void GameGUI::onMapInfoFinish(MapFormat* map)
{
	Rocket::Controls::ElementFormControlInput* input = static_cast<Rocket::Controls::ElementFormControlInput*>(mGuis[GT_SET_MAP_INFO]->GetElementById("name"));
	strcpy_s(map->name, input->GetValue().CString());
	input = static_cast<Rocket::Controls::ElementFormControlInput*>(mGuis[GT_SET_MAP_INFO]->GetElementById("description"));
	strcpy_s(map->description, input->GetValue().CString());
//	map->seconds = StringConverter::parseInt(dynamic_cast<Rocket::Controls::ElementFormControl*>(mGuis[GT_SET_MAP_INFO]->GetElementById("time"))->GetValue().CString());
}
	
void GameGUI::updateMapInfo()
{
	struct InvalidChar
	{
		bool operator()(char c) const
		{
			return !isprint((unsigned)c);
		}
	};
	struct InvalidNumber
	{
		bool operator()(char c) const
		{
			return !isdigit((unsigned)c);
		}
	};

	Rocket::Controls::ElementFormControlInput* input = static_cast<Rocket::Controls::ElementFormControlInput*>(mGuis[GT_SET_MAP_INFO]->GetElementById("name"));
	std::string helper = std::string(input->GetValue().CString());//
	helper.erase(std::remove_if(helper.begin(),helper.end(),InvalidChar()), helper.end());
	input->SetValue(helper.c_str());
	
	input = static_cast<Rocket::Controls::ElementFormControlInput*>(mGuis[GT_SET_MAP_INFO]->GetElementById("description"));
	helper = std::string(input->GetValue().CString());//
	helper.erase(std::remove_if(helper.begin(),helper.end(),InvalidChar()), helper.end());
	input->SetValue(helper.c_str());
	
	input = static_cast<Rocket::Controls::ElementFormControlInput*>(mGuis[GT_SET_MAP_INFO]->GetElementById("time"));
	helper = std::string(input->GetValue().CString());//
	helper.erase(std::remove_if(helper.begin(),helper.end(),InvalidNumber()), helper.end());
	int number = StringConverter::parseInt(helper, 0);
	if (number > MAP_MAX_TIME)
	{
		number = MAP_MAX_TIME;
	}
	input->SetValue(StringConverter::toString(number).c_str());
}

void GameGUI::updateEditorSides(int totalPlatforms, String& mapName, std::vector<Platform*>& plataformVector)
{
	mEditorText[K_PLATFORMS_LEFT]->SetText(Rocket::Core::WString("Platforms: ") + Rocket::Core::WString(Ogre::StringConverter::toString(totalPlatforms).c_str()) + Rocket::Core::WString("/") + Rocket::Core::WString(Ogre::StringConverter::toString(MAX_PLATFORMS_NUM).c_str()));
	//mEditorText[K_NAME]->SetText(Rocket::Core::WString("Map name: ") + Rocket::Core::WString(mapName.c_str()));
	
	bool movable = true;
	for (int i = 0; i < plataformVector.size(); i++)
	{
		if (plataformVector[i]->Type() == PlatformType::PT_COMMON) { movable = false; break; }
	}
	
	if (plataformVector.size() == 0)
	{
		// do nothing
	}
	else if (plataformVector.size() == 1)
	{
		Vector3 origin = plataformVector[0]->getBox()->getNode()->getPosition();

		int rotation = Math::IFloor(plataformVector[0]->getBox()->getNode()->_getDerivedOrientation().getYaw().valueDegrees());

		if (movable)
		{
			MovablePlatform* movablePlat = static_cast<MovablePlatform*>(plataformVector[0]);
			
			Vector3 size = movablePlat->getBox()->size();
			
			mEditorText[K_PLATFORM_7]->SetText(
				Rocket::Core::WString("Size: (") +
				Rocket::Core::WString(Ogre::StringConverter::toString(size.x, 0).c_str()) +
				Rocket::Core::WString(", ") +
				Rocket::Core::WString(Ogre::StringConverter::toString(size.y, 0).c_str()) +
				Rocket::Core::WString(", ") +
				Rocket::Core::WString(Ogre::StringConverter::toString(size.z, 0).c_str()) +
				Rocket::Core::WString(")")
			);
			mEditorText[K_PLATFORM_6]->SetText(
				Rocket::Core::WString("Destiny: (") +
				Rocket::Core::WString(Ogre::StringConverter::toString(movablePlat->getLinearArgument().x, 0).c_str()) +
				Rocket::Core::WString(", ") +
				Rocket::Core::WString(Ogre::StringConverter::toString(movablePlat->getLinearArgument().y, 0).c_str()) +
				Rocket::Core::WString(", ") +
				Rocket::Core::WString(Ogre::StringConverter::toString(movablePlat->getLinearArgument().z, 0).c_str()) +
				Rocket::Core::WString(")")
			);
			mEditorText[K_PLATFORM_5]->SetText(
				Rocket::Core::WString("Linear speed: ") +
				Rocket::Core::WString(Ogre::StringConverter::toString(movablePlat->getLinearSpeed()).c_str())
			);
			mEditorText[K_PLATFORM_4]->SetText(
				Rocket::Core::WString("Rotation speed: ") +
				Rocket::Core::WString(Ogre::StringConverter::toString((int)fixAngleNumber(movablePlat->getRotateSpeed().valueDegrees())).c_str())
			);


			if (movablePlat->getActivator())
			{
				mEditorText[K_PLATFORM_3]->SetText(
					Rocket::Core::WString("Activator: ") +
					Rocket::Core::WString(WorldActivator::activatortype2string(movablePlat->getActivator()).c_str())
				);
			}
			else
			{
				mEditorText[K_PLATFORM_3]->SetText(Rocket::Core::WString("Activator: None"));
			}

			mEditorText[K_PLATFORM_2]->SetText(
				Rocket::Core::WString("Position: (") +
				Rocket::Core::WString(Ogre::StringConverter::toString(origin.x, 0).c_str()) +
				Rocket::Core::WString(", ") +
				Rocket::Core::WString(Ogre::StringConverter::toString(origin.y, 0).c_str()) +
				Rocket::Core::WString(", ") +
				Rocket::Core::WString(Ogre::StringConverter::toString(origin.z, 0).c_str()) +
				Rocket::Core::WString(")")
			);
			mEditorText[K_PLATFORM_1]->SetText(
				Rocket::Core::WString("Rotation: ") +
				Rocket::Core::WString(Ogre::StringConverter::toString(rotation, 0).c_str())
			);
		}
		else
		{
			mEditorText[K_PLATFORM_5]->SetText(
				Rocket::Core::WString("Material: ") +
				Rocket::Core::WString(WorldBox::materialId2string(plataformVector[0]->getBox()->getMaterial()).c_str())
			);
			
			Vector3 size = plataformVector[0]->getBox()->size();
			
			mEditorText[K_PLATFORM_4]->SetText(
				Rocket::Core::WString("Size: (") +
				Rocket::Core::WString(Ogre::StringConverter::toString(size.x, 0).c_str()) +
				Rocket::Core::WString(", ") +
				Rocket::Core::WString(Ogre::StringConverter::toString(size.y, 0).c_str()) +
				Rocket::Core::WString(", ") +
				Rocket::Core::WString(Ogre::StringConverter::toString(size.z, 0).c_str()) +
				Rocket::Core::WString(")")
			);

			if (plataformVector[0]->getActivator())
			{
				mEditorText[K_PLATFORM_3]->SetText(
					Rocket::Core::WString("Activator: ") +
					Rocket::Core::WString(WorldActivator::activatortype2string(plataformVector[0]->getActivator()).c_str())
				);
			}
			else
			{
				mEditorText[K_PLATFORM_3]->SetText(Rocket::Core::WString("Activator: None"));
			}

			mEditorText[K_PLATFORM_2]->SetText(
				Rocket::Core::WString("Position: (") +
				Rocket::Core::WString(Ogre::StringConverter::toString(origin.x, 0).c_str()) +
				Rocket::Core::WString(", ") +
				Rocket::Core::WString(Ogre::StringConverter::toString(origin.y, 0).c_str()) +
				Rocket::Core::WString(", ") +
				Rocket::Core::WString(Ogre::StringConverter::toString(origin.z, 0).c_str()) +
				Rocket::Core::WString(")")
			);
			mEditorText[K_PLATFORM_1]->SetText(
				Rocket::Core::WString("Rotation: ") +
				Rocket::Core::WString(Ogre::StringConverter::toString(rotation, 0).c_str())
			);
		}
	}
	else
	{
		//Vector3 center = Vector3::ZERO;
		//Degree averageRotation = Degree(0), rotSpeed = Degree(0);
		//bool sameType = true, sameRotationSpeed = true, sameLinearSpeed = true;
		//WorldBox::BoxType type = plataformVector[0]->getBox()->getBoxType();
		//int linearSpeed = 0;
		//if (movable)
		//{
		//	linearSpeed = static_cast<MovablePlatform*>(plataformVector[0])->getLinearSpeed();
		//	rotSpeed = static_cast<MovablePlatform*>(plataformVector[0])->getRotateSpeed();
		//}
		//
		//for (int i = 0; i < plataformVector.size(); i++)
		//{
		//	if (movable && sameRotationSpeed && static_cast<MovablePlatform*>(plataformVector[i])->getRotateSpeed() != rotSpeed)
		//	{
		//		sameRotationSpeed = false;
		//	}
		//	if (movable && sameLinearSpeed && static_cast<MovablePlatform*>(plataformVector[i])->getLinearSpeed() != linearSpeed)
		//	{
		//		sameLinearSpeed = false;
		//	}
		//	if (sameType && plataformVector[i]->getBox()->getBoxType() != type)
		//	{
		//		sameType = false;
		//	}
		//	averageRotation += plataformVector[i]->getBox()->getNode()->getOrientation().getYaw();
		//	center += plataformVector[i]->getBox()->getNode()->getPosition();
		//}
		//
		//center /= plataformVector.size();
		//averageRotation /= plataformVector.size();

		//if (movable)
		//{
		//	if (sameType)
		//	{
		//		mEditorText[K_PLATFORM_4]->SetText(
		//			Rocket::Core::WString("Type: ") +
		//			Rocket::Core::WString(WorldBox::boxtype2string(type).c_str())
		//		);
		//	}
		//	else
		//	{
		//		mEditorText[K_PLATFORM_4]->SetText(
		//			Rocket::Core::WString("Type: Mixed")
		//		);
		//	}

		//	if (sameLinearSpeed)
		//	{
		//		mEditorText[K_PLATFORM_3]->SetText(
		//			Rocket::Core::WString("Linear speed: ") +
		//			Rocket::Core::WString(Ogre::StringConverter::toString((int)linearSpeed).c_str())
		//		);
		//	}
		//	else
		//	{
		//		mEditorText[K_PLATFORM_3]->SetText(
		//			Rocket::Core::WString("Linear speed: Mixed")
		//		);
		//	}

		//	if (sameRotationSpeed)
		//	{
		//		mEditorText[K_PLATFORM_3]->SetText(
		//			Rocket::Core::WString("Rotation speed: ") +
		//			Rocket::Core::WString(Ogre::StringConverter::toString((int)fixAngleNumber(rotSpeed.valueAngleUnits())).c_str())
		//		);
		//	}
		//	else
		//	{
		//		mEditorText[K_PLATFORM_3]->SetText(
		//			Rocket::Core::WString("Rotation speed: Mixed")
		//		);
		//	}

		//	mEditorText[K_PLATFORM_2]->SetText(
		//		Rocket::Core::WString("Average position: (") +
		//		Rocket::Core::WString(Ogre::StringConverter::toString((int)center.x, 0).c_str()) +
		//		Rocket::Core::WString(", ") +
		//		Rocket::Core::WString(Ogre::StringConverter::toString((int)center.y, 0).c_str()) +
		//		Rocket::Core::WString(", ") +
		//		Rocket::Core::WString(Ogre::StringConverter::toString((int)center.z, 0).c_str()) +
		//		Rocket::Core::WString(")")
		//	);
		//	mEditorText[K_PLATFORM_1]->SetText(
		//		Rocket::Core::WString("Average rotation: ") +
		//		Rocket::Core::WString(Ogre::StringConverter::toString((int)fixAngleNumber(averageRotation.valueDegrees()), 0).c_str())
		//	);
		//}
		//else
		//{
		//	if (sameType)
		//	{
		//		mEditorText[K_PLATFORM_3]->SetText(
		//			Rocket::Core::WString("Type: ") +
		//			Rocket::Core::WString(WorldBox::boxtype2string(type).c_str())
		//		);
		//	}
		//	else
		//	{
		//		mEditorText[K_PLATFORM_3]->SetText(
		//			Rocket::Core::WString("Type: Mixed")
		//		);
		//	}

		//	mEditorText[K_PLATFORM_2]->SetText(
		//		Rocket::Core::WString("Average position: (") +
		//		Rocket::Core::WString(Ogre::StringConverter::toString((int)center.x, 0).c_str()) +
		//		Rocket::Core::WString(", ") +
		//		Rocket::Core::WString(Ogre::StringConverter::toString((int)center.y, 0).c_str()) +
		//		Rocket::Core::WString(", ") +
		//		Rocket::Core::WString(Ogre::StringConverter::toString((int)center.z, 0).c_str()) +
		//		Rocket::Core::WString(")")
		//	);
		//	mEditorText[K_PLATFORM_1]->SetText(
		//		Rocket::Core::WString("Average rotation: ") +
		//		Rocket::Core::WString(Ogre::StringConverter::toString((int)fixAngleNumber(averageRotation.valueDegrees()), 0).c_str())
		//	);
		//}
	}
}

void GameGUI::setEditorTop1(Vector3 vector)
{
	String x = StringConverter::toString(vector.x, 0), y = StringConverter::toString(vector.y, 0), z = StringConverter::toString(vector.z, 0);
	mEditorText[K_TOP_1]->SetText(
		Rocket::Core::WString("Position: ") +
		Rocket::Core::WString(x.c_str()) +
		Rocket::Core::WString(", ") +
		Rocket::Core::WString(y.c_str()) +
		Rocket::Core::WString(", ") +
		Rocket::Core::WString(z.c_str())
	);
}

void GameGUI::updatePlayingGuiMap(double timeSinceMap, String mapName)
{
	return;
	// TODO fix all this
	const int timeStart = 5.5, timeNotFade = 2, timeFadding = 1;
	Rocket::Core::Element* helper = mGuis[GT_PLAYING]->GetElementById("playing-map-name");
	Rocket::Core::Element* helper2 = mGuis[GT_PLAYING]->GetElementById("playing-map-name-2");
	
	if (timeSinceMap <= timeStart)
	{
		double percentageFaded = (timeSinceMap) / timeStart;
		int number = (int)(256 * percentageFaded);
		if (number > 256) number = 256;
		if (number < 0) number = 0;
		char hex[3];
		_snprintf(hex, 3, "%02x", number);
		helper->SetProperty("color", Rocket::Core::String("#FFFFFF") + hex);
		helper2->SetProperty("color", Rocket::Core::String("#000000") + hex);
	}
	else if (timeSinceMap <= timeNotFade + timeStart)
	{
		if (!helper->IsVisible())
		{
			helper->SetProperty("visibility", "visible");
			helper2->SetProperty("visibility", "visible");
		}
		helper->SetProperty("color", "#FFFFFFFF");
		helper2->SetProperty("color", "#000000FF");
	}
	else if (timeSinceMap <= timeNotFade + timeFadding + timeStart)
	{
		double percentageFaded = 1 - (timeSinceMap - timeNotFade + timeStart) / timeFadding;
		int number = (int)(256 * percentageFaded);
		if (number > 256) number = 256;
		if (number < 0) number = 0;
		char hex[3];
		_snprintf(hex, 3, "%02x", number);
		helper->SetProperty("color", Rocket::Core::String("#FFFFFF") + hex);
		helper2->SetProperty("color", Rocket::Core::String("#000000") + hex);
	}
	else if (helper->IsVisible())
	{
		helper->SetProperty("visibility", "hidden");
		helper2->SetProperty("visibility", "hidden");
	}

	if (mGuis[GT_PLAYING]->GetElementById("playing-map-name")->GetInnerRML() != mapName.c_str())
	{
		mGuis[GT_PLAYING]->GetElementById("playing-map-name")->SetInnerRML(mapName.c_str());
		mGuis[GT_PLAYING]->GetElementById("playing-map-name-2")->SetInnerRML(mapName.c_str());
	}

}

void GameGUI::updateIngameOptionsGui()
{
	Rocket::Controls::ElementFormControl* controlHelper = dynamic_cast<Rocket::Controls::ElementFormControl*>(mGuis[GT_INGAME_OPTIONS]->GetElementById("mouse-sensitivity"));
	Rocket::Core::String stringHelper = controlHelper->GetValue();
	double doubleHelper = 0;
	Rocket::Core::TypeConverter<Rocket::Core::String, double>().Convert(stringHelper, doubleHelper);
	char buffer[32];
	sprintf_s(buffer, sizeof(char) * 32, "%.2f", doubleHelper / 100);
	mMouseSensitivtySpanTextElement->SetText(Rocket::Core::String(buffer));
}

void GameGUI::updateDeathGui(Real timeSincePlaying, Real timeToRespawn, MapFormat* meta)
{
	Rocket::Core::Element* elementHelper = mGuis[GT_DEATH]->GetElementById("bottom-respawn-text");
	if (_::options.showHelp)
	{
		if (!elementHelper->IsVisible())
		{
			elementHelper->SetProperty("visibility", "visible");
		}
	}
	else
	{
		if (elementHelper->IsVisible())
		{
			elementHelper->SetProperty("visibility", "hidden");
		}
	}

	char buffer[32];
	_snprintf(buffer, 32, "%.1f", timeToRespawn);
	mDeathCamRespawnTimeLeft->SetText(buffer);
	if (meta) mDeathCamMapName->SetText(meta->name);
	mDeathCamExtra->SetText("Playing time: " + Rocket::Core::String(timeToString(timeSincePlaying).c_str()));
}

void GameGUI::onGameFinished(String mapName, String nextMapName, String actionKeyName, double timeFinish)
{
	Rocket::Core::Element* elementHelper = mGuis[GT_MAP_FINISHED]->GetElementById("action-key");
	elementHelper->SetInnerRML(actionKeyName.c_str());

	elementHelper = mGuis[GT_MAP_FINISHED]->GetElementById("text-top-current-map-name");
	elementHelper->SetInnerRML(mapName.c_str());

	char buffer[32];
	_snprintf(buffer, 32, "%.1f", timeFinish);
	elementHelper = mGuis[GT_MAP_FINISHED]->GetElementById("time");
	elementHelper->SetInnerRML(buffer);

	elementHelper = mGuis[GT_MAP_FINISHED]->GetElementById("next-map");
	elementHelper->SetInnerRML(nextMapName.c_str());
}

void GameGUI::ConfigureRenderSystem()
{
	Ogre::RenderSystem* render_system = Ogre::Root::getSingleton().getRenderSystem();

	// Set up the projection and view matrices.
	Ogre::Matrix4 projection_matrix;
	float z_near = -1;
	float z_far = 1;

	projection_matrix = Ogre::Matrix4::ZERO;

	// Set up matrices.
	projection_matrix[0][0] = 2.0f / mWindowWidth;
	projection_matrix[0][3]= -1.0000000f;
	projection_matrix[1][1]= -2.0f / mWindowHeight;
	projection_matrix[1][3]= 1.0000000f;
	projection_matrix[2][2]= -2.0f / (z_far - z_near);
	projection_matrix[3][3]= 1.0000000f;
	//BuildProjectionMatrix(projection_matrix);
	render_system->_setProjectionMatrix(projection_matrix);
	render_system->_setViewMatrix(Ogre::Matrix4::IDENTITY);

	// Disable lighting, as all of Rocket's geometry is unlit.
	render_system->setLightingEnabled(false);
	// Disable depth-buffering; all of the geometry is already depth-sorted.
	render_system->_setDepthBufferParams(false, false);
	// Rocket generates anti-clockwise geometry, so enable clockwise-culling.
	render_system->_setCullingMode(Ogre::CULL_CLOCKWISE);
	// Disable fogging.
	render_system->_setFog(Ogre::FOG_NONE);
	// Enable writing to all four channels.
	render_system->_setColourBufferWriteEnabled(true, true, true, true);
	// Unbind any vertex or fragment programs bound previously by the application.
	render_system->unbindGpuProgram(Ogre::GPT_FRAGMENT_PROGRAM);
	render_system->unbindGpuProgram(Ogre::GPT_VERTEX_PROGRAM);

	// Set texture settings to clamp along both axes.
	Ogre::TextureUnitState::UVWAddressingMode addressing_mode;
	addressing_mode.u = Ogre::TextureUnitState::TAM_CLAMP;
	addressing_mode.v = Ogre::TextureUnitState::TAM_CLAMP;
	addressing_mode.w = Ogre::TextureUnitState::TAM_CLAMP;
	render_system->_setTextureAddressingMode(0, addressing_mode);

	// Set the texture coordinates for unit 0 to be read from unit 0.
	render_system->_setTextureCoordSet(0, 0);
	// Disable texture coordinate calculation.
	render_system->_setTextureCoordCalculation(0, Ogre::TEXCALC_NONE);
	// Enable linear filtering; images should be rendering 1 texel == 1 pixel, so point filtering could be used
	// except in the case of scaling tiled decorators.
	render_system->_setTextureUnitFiltering(0, Ogre::FO_LINEAR, Ogre::FO_LINEAR, Ogre::FO_POINT);
	// Disable texture coordinate transforms.
	render_system->_setTextureMatrix(0, Ogre::Matrix4::IDENTITY);
	// Reject pixels with an alpha of 0.
	render_system->_setAlphaRejectSettings(Ogre::CMPF_GREATER, 0, false);
	// Disable all texture units but the first.
	render_system->_disableTextureUnitsFrom(1);

	// Enable simple alpha blending.
	render_system->_setSceneBlending(Ogre::SBF_SOURCE_ALPHA, Ogre::SBF_ONE_MINUS_SOURCE_ALPHA);

	// Disable depth bias.
	render_system->_setDepthBias(0, 0);
}

void GameGUI::renderQueueStarted(uint8 queueGroupId, const Ogre::String& invocation, bool& ROCKET_UNUSED(skipThisInvocation))
{
	if (queueGroupId == Ogre::RENDER_QUEUE_OVERLAY && Ogre::Root::getSingleton().getRenderSystem()->_getViewport()->getOverlaysEnabled())
	{
		mContext->Update();
		ConfigureRenderSystem();
		mContext->Render();
	}
	/*if (queueGroupId == Ogre::RENDER_QUEUE_OVERLAY && Ogre::Root::getSingleton().getRenderSystem()->_getViewport()->getOverlaysEnabled())
	{
		context->Update();

		ConfigureRenderSystem();
		context->Render();
	}*/
}

void GameGUI::renderQueueEnded(uint8 ROCKET_UNUSED(queueGroupId), const Ogre::String& ROCKET_UNUSED(invocation), bool& ROCKET_UNUSED(repeatThisInvocation))
{
}