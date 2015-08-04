#include "ConfigOptions.h"
#include "dirent.h"
#include <stdio.h>

ConfigOptions::ConfigOptions(void)
	: mouseSensitivity(0.5),
	slowMotion(1),
	soundEffectsLevel(0.5),
	mCurrentSkin("default"),
	invertX(true),
	invertY(true),
	pauseWhileInactive(false),
	axisDeadzone(DEADZONE),
	bunnyJumping(true),
	showHelp(true),
	muteSoundEffects(false),
	useKeyboardAndMouse(true),
	useMouseForCameraMovement(true),
	showJumpEffects(true),
	leftClick(Keys::KeyName::K_NONE),
	rightClick(Keys::KeyName::K_NONE),
	middleClick(Keys::KeyName::K_NONE),
	wheelUp(Keys::KeyName::K_NONE),
	wheelDown(Keys::KeyName::K_NONE)
{
	window.width = DEFAULT_WINDOW_WIDTH;
	window.height = DEFAULT_WINDOW_HEIGHT;
	window.x = GetSystemMetrics(SM_CXSCREEN) / 2 - window.width / 2;
	window.y = GetSystemMetrics(SM_CYSCREEN) / 2 - window.height / 2;

	for (int i = 0; i < Keys::K_KEYBOARD_ENUM_SIZE; i++)
	{
		keyMapping[i] = OIS::KeyCode::KC_PAUSE;
		joystickMapping[i] = -1;
	}

	for (int i = 0; i < NUMBER_OF_AXIS; i++)
	{
		joystickAxis[i] = Keys::K_NONE;
	}

	keyMapping[Keys::KeyName::K_MOVEMENT_FORWARD] = OIS::KeyCode::KC_W;
	keyMapping[Keys::KeyName::K_MOVEMENT_BACK] = OIS::KeyCode::KC_S;
	keyMapping[Keys::KeyName::K_MOVEMENT_LEFT] = OIS::KeyCode::KC_A;
	keyMapping[Keys::KeyName::K_MOVEMENT_RIGHT] = OIS::KeyCode::KC_D;
	keyMapping[Keys::KeyName::K_JUMP_NORMAL] = OIS::KeyCode::KC_SPACE;
	keyMapping[Keys::KeyName::K_JUMP_LONG] = OIS::KeyCode::KC_Q;
#if ENABLE_BACKWARDS_JUMP
	keyMapping[Keys::KeyName::K_JUMP_BACKWARD] = OIS::KeyCode::KC_LSHIFT;
#endif
	keyMapping[Keys::KeyName::K_CAMERA_TOP] = OIS::KeyCode::KC_UP;
	keyMapping[Keys::KeyName::K_CAMERA_BOTTOM] = OIS::KeyCode::KC_DOWN;
	keyMapping[Keys::KeyName::K_CAMERA_LEFT] = OIS::KeyCode::KC_LEFT;
	keyMapping[Keys::KeyName::K_CAMERA_RIGHT] = OIS::KeyCode::KC_RIGHT;
	keyMapping[Keys::KeyName::K_CAMERA_ZOOM_RESET] = OIS::KeyCode::KC_NUMPAD0;
	keyMapping[Keys::KeyName::K_CAMERA_ZOOM_ADD] = OIS::KeyCode::KC_EQUALS;
	keyMapping[Keys::KeyName::K_CAMERA_ZOOM_REDUCE] = OIS::KeyCode::KC_MINUS;
	keyMapping[Keys::KeyName::K_EXTRA] = OIS::KeyCode::KC_F3;
	keyMapping[Keys::KeyName::K_EXTRA2] = OIS::KeyCode::KC_F2;
	keyMapping[Keys::KeyName::K_TOGGLE_DEBUG_MODE] = OIS::KeyCode::KC_F1;
	keyMapping[Keys::KeyName::K_PAUSE] = OIS::KeyCode::KC_ESCAPE;
//	keyMapping[Keys::KeyName::K_RESPAWN] = OIS::KeyCode::KC_R;
	keyMapping[Keys::KeyName::K_FORCE_RESPAWN] = OIS::KeyCode::KC_R;
	keyMapping[Keys::KeyName::K_ACTION] = OIS::KeyCode::KC_E;
	keyMapping[Keys::KeyName::K_EDITOR_TOGGLE_PLAYER] = OIS::KeyCode::KC_F5;
	keyMapping[Keys::KeyName::K_GO_TO_MENU] = OIS::KeyCode::KC_F11;
	keyMapping[Keys::KeyName::K_EDITOR_TOGGLE_PLAYER] = OIS::KeyCode::KC_F5;
	keyMapping[Keys::KeyName::K_TOGGLE_FULL_SCREEN] = OIS::KeyCode::KC_F10;
	keyMapping[Keys::KeyName::K_KEYBOARD_ENUM_SIZE] = OIS::KeyCode::KC_PAUSE;
	keyMapping[Keys::KeyName::K_SCREENSHOT] = OIS::KeyCode::KC_SYSRQ;

	keyMapping[Keys::KeyName::K_EXTRA] = OIS::KeyCode::KC_1;
	keyMapping[Keys::KeyName::K_EXTRA2] = OIS::KeyCode::KC_2;
	
	joystickMapping[Keys::KeyName::K_MOVEMENT_FORWARD] = -1;
	joystickMapping[Keys::KeyName::K_MOVEMENT_BACK] = -1;
	joystickMapping[Keys::KeyName::K_MOVEMENT_LEFT] = -1;
	joystickMapping[Keys::KeyName::K_MOVEMENT_RIGHT] = -1;
	joystickMapping[Keys::KeyName::K_JUMP_NORMAL] = 5;
	joystickMapping[Keys::KeyName::K_JUMP_LONG] = 4;
#if ENABLE_BACKWARDS_JUMP
	joystickMapping[Keys::KeyName::K_JUMP_BACKWARD] = 3;
#endif
	joystickMapping[Keys::KeyName::K_CAMERA_TOP] = -1;
	joystickMapping[Keys::KeyName::K_CAMERA_BOTTOM] = -1;
	joystickMapping[Keys::KeyName::K_CAMERA_LEFT] = -1;
	joystickMapping[Keys::KeyName::K_CAMERA_RIGHT] = -1;
	joystickMapping[Keys::KeyName::K_CAMERA_ZOOM_RESET] = 9;
	joystickMapping[Keys::KeyName::K_CAMERA_ZOOM_ADD] = -1;
	joystickMapping[Keys::KeyName::K_CAMERA_ZOOM_REDUCE] = -1;
	joystickMapping[Keys::KeyName::K_EXTRA] = -1;
	joystickMapping[Keys::KeyName::K_EXTRA2] = -1;
	joystickMapping[Keys::KeyName::K_TOGGLE_DEBUG_MODE] = -1;
	joystickMapping[Keys::KeyName::K_PAUSE] = 7;
//	joystickMapping[Keys::KeyName::K_RESPAWN] = OIS::KeyCode::KC_R;
	joystickMapping[Keys::KeyName::K_FORCE_RESPAWN] = 1;
	joystickMapping[Keys::KeyName::K_ACTION] = 2;
	joystickMapping[Keys::KeyName::K_GO_TO_MENU] = -1;
	joystickMapping[Keys::KeyName::K_TOGGLE_FULL_SCREEN] = -1;
	joystickMapping[Keys::KeyName::K_KEYBOARD_ENUM_SIZE] = -1;

	joystickAxis[0] = Keys::KeyName::K_MOVEMENT_FORWARD;
	joystickAxis[1] = Keys::KeyName::K_MOVEMENT_BACK;
	joystickAxis[2] = Keys::KeyName::K_MOVEMENT_LEFT;
	joystickAxis[3] = Keys::KeyName::K_MOVEMENT_RIGHT;
	joystickAxis[4] = Keys::KeyName::K_CAMERA_TOP;
	joystickAxis[5] = Keys::KeyName::K_CAMERA_BOTTOM;
	joystickAxis[6] = Keys::KeyName::K_CAMERA_LEFT;
	joystickAxis[7] = Keys::KeyName::K_CAMERA_RIGHT;
	joystickAxis[8] = Keys::KeyName::K_CAMERA_ZOOM_REDUCE;
	joystickAxis[9] = Keys::KeyName::K_CAMERA_ZOOM_ADD;

	leftClick = Keys::K_JUMP_NORMAL;
	rightClick = Keys::K_JUMP_LONG;
	middleClick = Keys::K_CAMERA_ZOOM_RESET;
	wheelUp = Keys::K_CAMERA_ZOOM_ADD;
	wheelDown = Keys::K_CAMERA_ZOOM_REDUCE;

}

ConfigOptions::~ConfigOptions(void)
{
}

void ConfigOptions::loadConfig(String filename)
{
	Ogre::ResourceManager::ResourceMapIterator it = Ogre::MaterialManager::getSingleton().getResourceIterator();
	std::vector<Ogre::MaterialPtr> v;
	while(it.hasMoreElements())
	{
		Ogre::Material* mat = static_cast<Material*>(it.getNext().getPointer());
		if(mat->getGroup()=="Skinneable")
		{
			int index = mat->getName() == "Character/Default" ? 1 : 0;
			if (mat->getNumTechniques() > 0 &&
				mat->getTechnique(0)->getNumPasses() > 0 &&
				mat->getTechnique(0)->getPass(index)->getNumTextureUnitStates() > 0 &&
				!mat->getTechnique(0)->getPass(index)->getTextureUnitState(0)->getTextureName().empty())
			{
				mSkinneableMaterials.push_back(mat);
				mTextureNames.push_back(mat->getTechnique(0)->getPass(index)->getTextureUnitState(0)->getTextureName());
			}
		}
	}

	changeSkin("default");
	
	std::fstream file(filename);
	std::string jsonString;
	if (file.is_open())
	{
		while (file.good())
		{
			std::string helperString;
			getline(file, helperString);
			jsonString.append(helperString);
			json_error_t error;
			json_t* root = json_loadb(jsonString.c_str(), jsonString.size(), 0, &error);
			if (!root)
			{
				// error, parse error
				return;
			}
			else
			{
				json_t* config = json_object_get(root, "basic-config");
				if (json_is_object(config))
				{
					json_t* helper = json_object_get(config, "mouse-sensitivity");
					if (json_is_real(helper))
					{
						double mouse = json_real_value(helper);
						if (mouse < 0) mouseSensitivity = 0;
						else if (mouse > 1) mouseSensitivity = 1;
						else mouseSensitivity = mouse;
					}
					helper = json_object_get(config, "sound-level");
					if (json_is_real (helper))
					{
						double sound = json_real_value(helper);
						if (sound < 0) soundEffectsLevel = 0;
						else if (sound > 1) soundEffectsLevel = 1;
						else soundEffectsLevel = sound;
					}
					helper = json_object_get(config, "invert-x");
					if (json_is_integer(helper))
					{
						invertX = json_integer_value(helper);
					}
					helper = json_object_get(config, "invert-y");
					if (json_is_integer(helper))
					{
						invertY = json_integer_value(helper);
					}
					helper = json_object_get(config, "bunny-jumping");
					if (json_is_integer(helper))
					{
						bunnyJumping = json_integer_value(helper);
					}
					helper = json_object_get(config, "show-help");
					if (json_is_integer(helper))
					{
						showHelp = json_integer_value(helper);
					}
					helper = json_object_get(config, "use-keyboard-and-mouse");
					if (json_is_integer(helper))
					{
						useKeyboardAndMouse = json_integer_value(helper);
					}
					helper = json_object_get(config, "mouse-camera-movement");
					if (json_is_integer(helper))
					{
						useMouseForCameraMovement = json_integer_value(helper);
					}
					helper = json_object_get(config, "mute-sounds");
					if (json_is_integer(helper))
					{
						muteSoundEffects = json_integer_value(helper);
					}
					helper = json_object_get(config, "pause-while-inactive");
					if (json_is_integer(helper))
					{
						pauseWhileInactive = json_integer_value(helper);
					}
					helper = json_object_get(config, "skin");
					if (json_is_string(helper))
					{
						changeSkin(json_string_value(helper));
					}
					helper = json_object_get(config, "window");
					helper = json_object_get(config, "window");
					auto helper2 = json_object_get(helper, "x");
					window.x = json_integer_value(helper2);
					 helper2 = json_object_get(helper, "y");
					window.y = json_integer_value(helper2);
					 helper2 = json_object_get(helper, "width");
					window.width = json_integer_value(helper2);
					 helper2 = json_object_get(helper, "height");
					window.height = json_integer_value(helper2);
				}
				json_decref(root);
			}
		}
	}
}

void ConfigOptions::saveConfig(String filename)
{
	json_t* root = json_object();
	json_t* basicConfig = json_object();
	json_object_set(root, "basic-config", basicConfig);
	json_t* helper = json_real(mouseSensitivity);
	json_object_set(basicConfig, "mouse-sensitivity", helper);
	helper = json_real(soundEffectsLevel);
	json_object_set(basicConfig, "sound-level", helper);
	helper = json_integer(invertX ? 1 : 0);
	json_object_set(basicConfig, "invert-x", helper);
	helper = json_integer(invertX ? 1 : 0);
	json_object_set(basicConfig, "invert-y", helper);
	helper = json_integer(bunnyJumping ? 1 : 0);
	json_object_set(basicConfig, "bunny-jumping", helper);
	helper = json_integer(showHelp ? 1 : 0);
	json_object_set(basicConfig, "show-help", helper);
	helper = json_integer(useKeyboardAndMouse ? 1 : 0);
	json_object_set(basicConfig, "use-keyboard-and-mouse", helper);
	helper = json_integer(useMouseForCameraMovement ? 1 : 0);
	json_object_set(basicConfig, "mouse-camera-movement", helper);
	helper = json_integer(muteSoundEffects ? 1 : 0);
	json_object_set(basicConfig, "mute-sounds", helper);
	helper = json_integer(pauseWhileInactive ? 1 : 0);
	json_object_set(basicConfig, "pause-while-inactive", helper);
	helper = json_string(mCurrentSkin.c_str());
	json_object_set(basicConfig, "skin", helper);
	json_t* windowJson = json_object();
	json_object_set(basicConfig, "window", windowJson);
	helper = json_integer(window.width);
	json_object_set(windowJson, "width", helper);
	helper = json_integer(window.height);
	json_object_set(windowJson, "height", helper);
	helper = json_integer(window.x);
	json_object_set(windowJson, "x", helper);
	helper = json_integer(window.y);
	json_object_set(windowJson, "y", helper);
	json_object_set(basicConfig, "skin", helper);
	json_dump_file(root, filename.c_str(), JSON_INDENT(2) & JSON_SORT_KEYS & JSON_PRESERVE_ORDER & JSON_ENCODE_ANY);
}

void ConfigOptions::changeSkin(String skinName)
{
	// not implemented yet
	return;

	// TODO remove this
	static bool a = false;
	if (!a)
	{
		a = true;
		skinName = "emerald";
	}
	bool alreadyLoaded = false;
		
	for (int i = 0; i < mLoadedSkins.size(); i++)
	{
		if (mLoadedSkins[i] == skinName)
		{
			alreadyLoaded = true;
			break;
		}
	}

	if (!alreadyLoaded)
	{
		String filenameBase = "D:\\platformer\\resources\\skins\\" + skinName + "\\";

		if (skinName == "default")
		{
			filenameBase = "D:\\platformer\\resources\\default-textures\\";// + skinName + "\\";
		}

		for (int i = 0; i < mTextureNames.size(); i++)
		{
			if (!mTextureNames[i].empty())
			{
				String completeFilename = filenameBase + mTextureNames[i];
				std::ifstream ifs(completeFilename.c_str(), std::ios::binary|std::ios::in);
				if (!ifs.is_open())
				{
					continue;
				}
				DataStreamPtr dataStream(new FileStreamDataStream(completeFilename, &ifs, false));
	
				Image img;
				String ext;
				String::size_type indexOfExtension = completeFilename.find_last_of('.');
				ext = completeFilename.substr(indexOfExtension+1);
				img.load(dataStream, ext);
				TextureManager::getSingleton().loadImage(
					skinName + "-" + mTextureNames[i],
					ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
					img,
					TEX_TYPE_2D,
					0
				);
			}
		}
	}

	for (int i = 0; i < mSkinneableMaterials.size(); i++)
	{
		if (!mTextureNames[i].empty())
		{
			String textureName = skinName + "-" + mTextureNames[i];
			TexturePtr ptr = TextureManager::getSingleton().getByName(textureName);
			if (!ptr.isNull())
			{
				String name = mSkinneableMaterials[i]->getName();
				int index = mSkinneableMaterials[i]->getName() == "Character/Default" ? 1 : 0;
				mSkinneableMaterials[i]->getTechnique(0)->getPass(index)->getTextureUnitState(0)->setTexture(ptr);
			}
		}
	}

	mLoadedSkins.push_back(skinName);
	mCurrentSkin = skinName;

	/*MaterialManager& materialManager = MaterialManager::getSingleton();
	MaterialPtr material = materialManager.getByName("Platform/FinishBox");
	String name = material->getTechnique(0)->getPass(0)->getTextureUnitState(0)->getTextureName();

	String filename = "D:\\platformer\\resources\\skins\\" + skinName + "\\" + name;
	std::ifstream ifs(filename.c_str(), std::ios::binary|std::ios::in);
	DataStreamPtr dataStream(new FileStreamDataStream(filename, &ifs, false));
	
	Image img;
	String ext;
	String::size_type indexOfExtension = filename.find_last_of('.');
	ext = filename.substr(indexOfExtension+1);
	img.load(dataStream, ext);
	
	TexturePtr texturePtr = TextureManager::getSingleton().loadImage(
		skinName + "-" + name,
		ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		img,
		TEX_TYPE_2D,
		0
	);

	material->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setTexture(texturePtr);
	ifs.close();*/
}