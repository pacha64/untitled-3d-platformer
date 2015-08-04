#pragma once
#include "stdafx.h"
#include "balance.h"

class ConfigOptions;

#define NUMBER_OF_AXIS 10

class Keys : public OIS::KeyListener, public OIS::JoyStickListener
{
public:
	Keys();
	void onRecreateInputSystem(OIS::Keyboard&);
	enum KeyName
	{
		K_NONE,
		K_MOVEMENT_FORWARD,
		K_MOVEMENT_BACK,
		K_MOVEMENT_LEFT,
		K_MOVEMENT_RIGHT,
		K_JUMP_NORMAL,
		K_JUMP_LONG,
#if ENABLE_BACKWARDS_JUMP
		K_JUMP_BACKWARD,
#endif
		K_CAMERA_TOP,
		K_CAMERA_BOTTOM,
		K_CAMERA_LEFT,
		K_CAMERA_RIGHT,
		K_CAMERA_ZOOM_RESET,
		K_CAMERA_ZOOM_ADD,
		K_CAMERA_ZOOM_REDUCE,
		K_EXTRA,
		K_EXTRA2,
		K_TOGGLE_DEBUG_MODE,
		K_PAUSE,
		//K_RESPAWN,
		K_ACTION,
		K_FORCE_RESPAWN,
		K_GO_TO_MENU,
		K_TOGGLE_FULL_SCREEN,
		K_EDITOR_TOGGLE_PLAYER,
		K_SCREENSHOT,
		K_KEYBOARD_ENUM_SIZE
	};
	int MouseX, MouseY, Wheel, /*MouseXabs, MouseYabs,*/ JoystickXcamera, JoystickYcamera;//, joystickSensitivity;
	static Keys* singleton;
	bool useDegreeMovement;
	Vector3 movementVector;
	bool updateKeyMapping(KeyName, ConfigOptions&, OIS::Mouse* mouse, OIS::JoyStick* joystick);
	bool isKeyDown(KeyName name);
	bool isKeyDownLastUpdate(KeyName name);
	bool keystroke(KeyName name);
	void updateKeystroke(OIS::Keyboard* keyboard, OIS::Mouse* mouse, OIS::JoyStick* joystick);
	static Ogre::String key2string(KeyName, ConfigOptions&);
#if ENABLE_BACKWARDS_JUMP
	void onHighJumpClick()
	{
		keysPressedCurrentUpdate[K_JUMP_BACKWARD] = true;
	}
#endif
	Rocket::Core::Input::KeyIdentifier ois2rocket(OIS::KeyCode);
private:
	typedef std::map< OIS::KeyCode, Rocket::Core::Input::KeyIdentifier > KeyIdentifierMap;
	KeyIdentifierMap key_identifiers;
	bool keysPressedLastUpdate[K_KEYBOARD_ENUM_SIZE];
	bool keysPressedCurrentUpdate[K_KEYBOARD_ENUM_SIZE];
	// keyboard
	virtual bool keyPressed(const OIS::KeyEvent &arg);
	virtual bool keyReleased(const OIS::KeyEvent &arg);
	// joystick
	virtual bool buttonPressed( const OIS::JoyStickEvent &arg, int button );
	virtual bool buttonReleased( const OIS::JoyStickEvent &arg, int button );
	virtual bool axisMoved( const OIS::JoyStickEvent &arg, int axis );
	virtual bool sliderMoved( const OIS::JoyStickEvent &, int index);
	virtual bool povMoved( const OIS::JoyStickEvent &arg, int index);
	virtual bool vector3Moved( const OIS::JoyStickEvent &arg, int index);
	
	bool validKeyCode(OIS::KeyCode);
	std::vector<OIS::KeyCode> mPressedKeyCodes;
	std::vector<int> mPressedJoystickButtons;
	int mJoystickAxesMovement[NUMBER_OF_AXIS];
	static Ogre::String key2string(OIS::KeyCode);
	void buildRocketKeyMap();
};