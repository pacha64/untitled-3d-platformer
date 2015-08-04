#pragma once
#include "stdafx.h"
#include "Keys.h"



struct ConfigOptions
{
public:
	ConfigOptions(void);
	~ConfigOptions(void);
	void loadConfig(String filename);
	void saveConfig(String filename);
	void changeSkin(String skinName);
	OIS::KeyCode keyMapping[Keys::KeyName::K_KEYBOARD_ENUM_SIZE];
	int joystickMapping[Keys::KeyName::K_KEYBOARD_ENUM_SIZE];
	/// 0-3 left axis, 4-7 right axis, 8 L trigger, 9 R trigger
	Keys::KeyName joystickAxis[NUMBER_OF_AXIS];
	Keys::KeyName leftClick, rightClick, middleClick, wheelUp, wheelDown;
	double mouseSensitivity, soundEffectsLevel;
	bool invertX, invertY, bunnyJumping, showHelp, useKeyboardAndMouse, useMouseForCameraMovement, muteSoundEffects, pauseWhileInactive, showJumpEffects;
	int axisDeadzone, axisMax;
	double slowMotion;
	struct WindowState
	{
		unsigned int width, height;
		int x, y;
	} window;
protected:
	String mCurrentSkin;
	std::vector<Material*> mSkinneableMaterials;
	std::vector<String> mTextureNames;
	std::vector<String> mLoadedSkins;

};

