#include "Keys.h"
#include "TextRenderer.h"
#include "Globals.h"
#include "ConfigOptions.h"

Keys::Keys()
	: JoystickXcamera(0),
	JoystickYcamera(0)
{
	MouseX = 0;
	MouseY = 0;
	//MouseXabs = 0;
	//MouseYabs = 0;
	Wheel = 0;
	//joystickSensitivity = 10;
	for (int i = 0; i < K_KEYBOARD_ENUM_SIZE; i++)
	{
		keysPressedLastUpdate[i] = false;
		keysPressedCurrentUpdate[i] = false;
	}
	for (int i = 0; i < NUMBER_OF_AXIS; i++)
	{
		mJoystickAxesMovement[i] = 0;
	}

	buildRocketKeyMap();
}

Rocket::Core::Input::KeyIdentifier Keys::ois2rocket(OIS::KeyCode keycode)
{
	return key_identifiers[keycode];
}

void Keys::buildRocketKeyMap()
{
	key_identifiers[OIS::KC_UNASSIGNED] = Rocket::Core::Input::KI_UNKNOWN;
	key_identifiers[OIS::KC_ESCAPE] = Rocket::Core::Input::KI_ESCAPE;
	key_identifiers[OIS::KC_1] = Rocket::Core::Input::KI_1;
	key_identifiers[OIS::KC_2] = Rocket::Core::Input::KI_2;
	key_identifiers[OIS::KC_3] = Rocket::Core::Input::KI_3;
	key_identifiers[OIS::KC_4] = Rocket::Core::Input::KI_4;
	key_identifiers[OIS::KC_5] = Rocket::Core::Input::KI_5;
	key_identifiers[OIS::KC_6] = Rocket::Core::Input::KI_6;
	key_identifiers[OIS::KC_7] = Rocket::Core::Input::KI_7;
	key_identifiers[OIS::KC_8] = Rocket::Core::Input::KI_8;
	key_identifiers[OIS::KC_9] = Rocket::Core::Input::KI_9;
	key_identifiers[OIS::KC_0] = Rocket::Core::Input::KI_0;
	key_identifiers[OIS::KC_MINUS] = Rocket::Core::Input::KI_OEM_MINUS;
	key_identifiers[OIS::KC_EQUALS] = Rocket::Core::Input::KI_OEM_PLUS;
	key_identifiers[OIS::KC_BACK] = Rocket::Core::Input::KI_BACK;
	key_identifiers[OIS::KC_TAB] = Rocket::Core::Input::KI_TAB;
	key_identifiers[OIS::KC_Q] = Rocket::Core::Input::KI_Q;
	key_identifiers[OIS::KC_W] = Rocket::Core::Input::KI_W;
	key_identifiers[OIS::KC_E] = Rocket::Core::Input::KI_E;
	key_identifiers[OIS::KC_R] = Rocket::Core::Input::KI_R;
	key_identifiers[OIS::KC_T] = Rocket::Core::Input::KI_T;
	key_identifiers[OIS::KC_Y] = Rocket::Core::Input::KI_Y;
	key_identifiers[OIS::KC_U] = Rocket::Core::Input::KI_U;
	key_identifiers[OIS::KC_I] = Rocket::Core::Input::KI_I;
	key_identifiers[OIS::KC_O] = Rocket::Core::Input::KI_O;
	key_identifiers[OIS::KC_P] = Rocket::Core::Input::KI_P;
	key_identifiers[OIS::KC_LBRACKET] = Rocket::Core::Input::KI_OEM_4;
	key_identifiers[OIS::KC_RBRACKET] = Rocket::Core::Input::KI_OEM_6;
	key_identifiers[OIS::KC_RETURN] = Rocket::Core::Input::KI_RETURN;
	key_identifiers[OIS::KC_LCONTROL] = Rocket::Core::Input::KI_LCONTROL;
	key_identifiers[OIS::KC_A] = Rocket::Core::Input::KI_A;
	key_identifiers[OIS::KC_S] = Rocket::Core::Input::KI_S;
	key_identifiers[OIS::KC_D] = Rocket::Core::Input::KI_D;
	key_identifiers[OIS::KC_F] = Rocket::Core::Input::KI_F;
	key_identifiers[OIS::KC_G] = Rocket::Core::Input::KI_G;
	key_identifiers[OIS::KC_H] = Rocket::Core::Input::KI_H;
	key_identifiers[OIS::KC_J] = Rocket::Core::Input::KI_J;
	key_identifiers[OIS::KC_K] = Rocket::Core::Input::KI_K;
	key_identifiers[OIS::KC_L] = Rocket::Core::Input::KI_L;
	key_identifiers[OIS::KC_SEMICOLON] = Rocket::Core::Input::KI_OEM_1;
	key_identifiers[OIS::KC_APOSTROPHE] = Rocket::Core::Input::KI_OEM_7;
	key_identifiers[OIS::KC_GRAVE] = Rocket::Core::Input::KI_OEM_3;
	key_identifiers[OIS::KC_LSHIFT] = Rocket::Core::Input::KI_LSHIFT;
	key_identifiers[OIS::KC_BACKSLASH] = Rocket::Core::Input::KI_OEM_5;
	key_identifiers[OIS::KC_Z] = Rocket::Core::Input::KI_Z;
	key_identifiers[OIS::KC_X] = Rocket::Core::Input::KI_X;
	key_identifiers[OIS::KC_C] = Rocket::Core::Input::KI_C;
	key_identifiers[OIS::KC_V] = Rocket::Core::Input::KI_V;
	key_identifiers[OIS::KC_B] = Rocket::Core::Input::KI_B;
	key_identifiers[OIS::KC_N] = Rocket::Core::Input::KI_N;
	key_identifiers[OIS::KC_M] = Rocket::Core::Input::KI_M;
	key_identifiers[OIS::KC_COMMA] = Rocket::Core::Input::KI_OEM_COMMA;
	key_identifiers[OIS::KC_PERIOD] = Rocket::Core::Input::KI_OEM_PERIOD;
	key_identifiers[OIS::KC_SLASH] = Rocket::Core::Input::KI_OEM_2;
	key_identifiers[OIS::KC_RSHIFT] = Rocket::Core::Input::KI_RSHIFT;
	key_identifiers[OIS::KC_MULTIPLY] = Rocket::Core::Input::KI_MULTIPLY;
	key_identifiers[OIS::KC_LMENU] = Rocket::Core::Input::KI_LMENU;
	key_identifiers[OIS::KC_SPACE] = Rocket::Core::Input::KI_SPACE;
	key_identifiers[OIS::KC_CAPITAL] = Rocket::Core::Input::KI_CAPITAL;
	key_identifiers[OIS::KC_F1] = Rocket::Core::Input::KI_F1;
	key_identifiers[OIS::KC_F2] = Rocket::Core::Input::KI_F2;
	key_identifiers[OIS::KC_F3] = Rocket::Core::Input::KI_F3;
	key_identifiers[OIS::KC_F4] = Rocket::Core::Input::KI_F4;
	key_identifiers[OIS::KC_F5] = Rocket::Core::Input::KI_F5;
	key_identifiers[OIS::KC_F6] = Rocket::Core::Input::KI_F6;
	key_identifiers[OIS::KC_F7] = Rocket::Core::Input::KI_F7;
	key_identifiers[OIS::KC_F8] = Rocket::Core::Input::KI_F8;
	key_identifiers[OIS::KC_F9] = Rocket::Core::Input::KI_F9;
	key_identifiers[OIS::KC_F10] = Rocket::Core::Input::KI_F10;
	key_identifiers[OIS::KC_NUMLOCK] = Rocket::Core::Input::KI_NUMLOCK;
	key_identifiers[OIS::KC_SCROLL] = Rocket::Core::Input::KI_SCROLL;
	key_identifiers[OIS::KC_NUMPAD7] = Rocket::Core::Input::KI_7;
	key_identifiers[OIS::KC_NUMPAD8] = Rocket::Core::Input::KI_8;
	key_identifiers[OIS::KC_NUMPAD9] = Rocket::Core::Input::KI_9;
	key_identifiers[OIS::KC_SUBTRACT] = Rocket::Core::Input::KI_SUBTRACT;
	key_identifiers[OIS::KC_NUMPAD4] = Rocket::Core::Input::KI_4;
	key_identifiers[OIS::KC_NUMPAD5] = Rocket::Core::Input::KI_5;
	key_identifiers[OIS::KC_NUMPAD6] = Rocket::Core::Input::KI_6;
	key_identifiers[OIS::KC_ADD] = Rocket::Core::Input::KI_ADD;
	key_identifiers[OIS::KC_NUMPAD1] = Rocket::Core::Input::KI_1;
	key_identifiers[OIS::KC_NUMPAD2] = Rocket::Core::Input::KI_2;
	key_identifiers[OIS::KC_NUMPAD3] = Rocket::Core::Input::KI_3;
	key_identifiers[OIS::KC_NUMPAD0] = Rocket::Core::Input::KI_0;
	key_identifiers[OIS::KC_DECIMAL] = Rocket::Core::Input::KI_DECIMAL;
	key_identifiers[OIS::KC_OEM_102] = Rocket::Core::Input::KI_OEM_102;
	key_identifiers[OIS::KC_F11] = Rocket::Core::Input::KI_F11;
	key_identifiers[OIS::KC_F12] = Rocket::Core::Input::KI_F12;
	key_identifiers[OIS::KC_F13] = Rocket::Core::Input::KI_F13;
	key_identifiers[OIS::KC_F14] = Rocket::Core::Input::KI_F14;
	key_identifiers[OIS::KC_F15] = Rocket::Core::Input::KI_F15;
	key_identifiers[OIS::KC_KANA] = Rocket::Core::Input::KI_KANA;
	key_identifiers[OIS::KC_ABNT_C1] = Rocket::Core::Input::KI_UNKNOWN;
	key_identifiers[OIS::KC_CONVERT] = Rocket::Core::Input::KI_CONVERT;
	key_identifiers[OIS::KC_NOCONVERT] = Rocket::Core::Input::KI_NONCONVERT;
	key_identifiers[OIS::KC_YEN] = Rocket::Core::Input::KI_UNKNOWN;
	key_identifiers[OIS::KC_ABNT_C2] = Rocket::Core::Input::KI_UNKNOWN;
	key_identifiers[OIS::KC_NUMPADEQUALS] = Rocket::Core::Input::KI_OEM_NEC_EQUAL;
	key_identifiers[OIS::KC_PREVTRACK] = Rocket::Core::Input::KI_MEDIA_PREV_TRACK;
	key_identifiers[OIS::KC_AT] = Rocket::Core::Input::KI_UNKNOWN;
	key_identifiers[OIS::KC_COLON] = Rocket::Core::Input::KI_OEM_1;
	key_identifiers[OIS::KC_UNDERLINE] = Rocket::Core::Input::KI_OEM_MINUS;
	key_identifiers[OIS::KC_KANJI] = Rocket::Core::Input::KI_KANJI;
	key_identifiers[OIS::KC_STOP] = Rocket::Core::Input::KI_UNKNOWN;
	key_identifiers[OIS::KC_AX] = Rocket::Core::Input::KI_OEM_AX;
	key_identifiers[OIS::KC_UNLABELED] = Rocket::Core::Input::KI_UNKNOWN;
	key_identifiers[OIS::KC_NEXTTRACK] = Rocket::Core::Input::KI_MEDIA_NEXT_TRACK;
	key_identifiers[OIS::KC_NUMPADENTER] = Rocket::Core::Input::KI_NUMPADENTER;
	key_identifiers[OIS::KC_RCONTROL] = Rocket::Core::Input::KI_RCONTROL;
	key_identifiers[OIS::KC_MUTE] = Rocket::Core::Input::KI_VOLUME_MUTE;
	key_identifiers[OIS::KC_CALCULATOR] = Rocket::Core::Input::KI_UNKNOWN;
	key_identifiers[OIS::KC_PLAYPAUSE] = Rocket::Core::Input::KI_MEDIA_PLAY_PAUSE;
	key_identifiers[OIS::KC_MEDIASTOP] = Rocket::Core::Input::KI_MEDIA_STOP;
	key_identifiers[OIS::KC_VOLUMEDOWN] = Rocket::Core::Input::KI_VOLUME_DOWN;
	key_identifiers[OIS::KC_VOLUMEUP] = Rocket::Core::Input::KI_VOLUME_UP;
	key_identifiers[OIS::KC_WEBHOME] = Rocket::Core::Input::KI_BROWSER_HOME;
	key_identifiers[OIS::KC_NUMPADCOMMA] = Rocket::Core::Input::KI_SEPARATOR;
	key_identifiers[OIS::KC_DIVIDE] = Rocket::Core::Input::KI_DIVIDE;
	key_identifiers[OIS::KC_SYSRQ] = Rocket::Core::Input::KI_SNAPSHOT;
	key_identifiers[OIS::KC_RMENU] = Rocket::Core::Input::KI_RMENU;
	key_identifiers[OIS::KC_PAUSE] = Rocket::Core::Input::KI_PAUSE;
	key_identifiers[OIS::KC_HOME] = Rocket::Core::Input::KI_HOME;
	key_identifiers[OIS::KC_UP] = Rocket::Core::Input::KI_UP;
	key_identifiers[OIS::KC_PGUP] = Rocket::Core::Input::KI_PRIOR;
	key_identifiers[OIS::KC_LEFT] = Rocket::Core::Input::KI_LEFT;
	key_identifiers[OIS::KC_RIGHT] = Rocket::Core::Input::KI_RIGHT;
	key_identifiers[OIS::KC_END] = Rocket::Core::Input::KI_END;
	key_identifiers[OIS::KC_DOWN] = Rocket::Core::Input::KI_DOWN;
	key_identifiers[OIS::KC_PGDOWN] = Rocket::Core::Input::KI_NEXT;
	key_identifiers[OIS::KC_INSERT] = Rocket::Core::Input::KI_INSERT;
	key_identifiers[OIS::KC_DELETE] = Rocket::Core::Input::KI_DELETE;
	key_identifiers[OIS::KC_LWIN] = Rocket::Core::Input::KI_LWIN;
	key_identifiers[OIS::KC_RWIN] = Rocket::Core::Input::KI_RWIN;
	key_identifiers[OIS::KC_APPS] = Rocket::Core::Input::KI_APPS;
	key_identifiers[OIS::KC_POWER] = Rocket::Core::Input::KI_POWER;
	key_identifiers[OIS::KC_SLEEP] = Rocket::Core::Input::KI_SLEEP;
	key_identifiers[OIS::KC_WAKE] = Rocket::Core::Input::KI_WAKE;
	key_identifiers[OIS::KC_WEBSEARCH] = Rocket::Core::Input::KI_BROWSER_SEARCH;
	key_identifiers[OIS::KC_WEBFAVORITES] = Rocket::Core::Input::KI_BROWSER_FAVORITES;
	key_identifiers[OIS::KC_WEBREFRESH] = Rocket::Core::Input::KI_BROWSER_REFRESH;
	key_identifiers[OIS::KC_WEBSTOP] = Rocket::Core::Input::KI_BROWSER_STOP;
	key_identifiers[OIS::KC_WEBFORWARD] = Rocket::Core::Input::KI_BROWSER_FORWARD;
	key_identifiers[OIS::KC_WEBBACK] = Rocket::Core::Input::KI_BROWSER_BACK;
	key_identifiers[OIS::KC_MYCOMPUTER] = Rocket::Core::Input::KI_UNKNOWN;
	key_identifiers[OIS::KC_MAIL] = Rocket::Core::Input::KI_LAUNCH_MAIL;
	key_identifiers[OIS::KC_MEDIASELECT] = Rocket::Core::Input::KI_LAUNCH_MEDIA_SELECT;
}

bool Keys::isKeyDown(KeyName name)
{
	return keysPressedCurrentUpdate[name];
}
bool Keys::isKeyDownLastUpdate(KeyName name)
{
	return keysPressedLastUpdate[name];
}
bool Keys::keystroke(KeyName name)
{
	return !isKeyDown(name) && isKeyDownLastUpdate(name);
}

void Keys::updateKeystroke(OIS::Keyboard* keyboard, OIS::Mouse* mouse, OIS::JoyStick* joystick)
{
	memcpy(keysPressedLastUpdate, keysPressedCurrentUpdate, sizeof(keysPressedCurrentUpdate));
	for (int i = 0; i < K_KEYBOARD_ENUM_SIZE; i++)
	{
		keysPressedCurrentUpdate[i] = false;
	}
	ConfigOptions& options = _::options;
	JoystickXcamera = 0,
	JoystickYcamera = 0;
	MouseX = 0;
	MouseY = 0;
	Wheel = 0;
	movementVector = Vector3::ZERO;
	useDegreeMovement = false;

	if (keyboard)
	{
		for (int i = 0; i < Keys::KeyName::K_KEYBOARD_ENUM_SIZE; i++)
		{
			if (keyboard->isKeyDown(options.keyMapping[i]))
			{
				keysPressedCurrentUpdate[i] = true;
			}
		}
	}

	if (mouse != 0)
	{
		if (mouse->getMouseState().buttonDown(OIS::MouseButtonID::MB_Left))
		{
			keysPressedCurrentUpdate[options.leftClick] = true;
		}
		if (mouse->getMouseState().buttonDown(OIS::MouseButtonID::MB_Right))
		{
			keysPressedCurrentUpdate[options.rightClick] = true;
		}
		if (mouse->getMouseState().buttonDown(OIS::MouseButtonID::MB_Middle))
		{
			keysPressedCurrentUpdate[options.middleClick] = true;
		}

		if (_::options.useMouseForCameraMovement)
		{
			MouseX = mouse->getMouseState().X.rel;
			MouseY = mouse->getMouseState().Y.rel;
		}
		Wheel = mouse->getMouseState().Z.rel;
	}

	if (joystick != 0)
	{
		const OIS::JoyStickState& joystickState = joystick->getJoyStickState();
		for (int i = 0; i < joystickState.mButtons.size(); i++)
		{
			for (int j = 0; j < KeyName::K_KEYBOARD_ENUM_SIZE; j++)
			{
				if (options.joystickMapping[j] == i && joystickState.mButtons[i])
				{
					keysPressedCurrentUpdate[j] = true;
				}
			}
		}

		if (Math::Abs(joystickState.mAxes[2].abs) >= options.axisDeadzone)
		{
			int pos = Math::Abs(joystickState.mAxes[2].abs) - options.axisDeadzone;
			double percentage = (double)pos / (OIS::JoyStick::MAX_AXIS - options.axisDeadzone);
			MouseY = percentage * JOYSTICK_MOVEMENT_MAX_AXIS;
			if (joystickState.mAxes[2].abs < 0)
			{
				MouseY *= -1;
			}
		}
		if (Math::Abs(joystickState.mAxes[3].abs) >= options.axisDeadzone)
		{
			int pos = Math::Abs(joystickState.mAxes[3].abs) - options.axisDeadzone;
			double percentage = (double)pos / (OIS::JoyStick::MAX_AXIS - options.axisDeadzone);
			MouseX = percentage * JOYSTICK_MOVEMENT_MAX_AXIS;
			if (joystickState.mAxes[3].abs < 0)
			{
				MouseX *= -1;
			}
		}


		for (int i = 0; i < joystickState.mAxes.size(); i++)
		{
			int newAxis = i;
			newAxis *= 2;
			if (joystickState.mAxes[i].abs > 0)
			{
				newAxis += 1;
			}
			int movementCounter = 0;
			
			if (i > NUMBER_OF_AXIS)
			{
				break;
			}
			else
			{
				if (Math::Abs(joystickState.mAxes[i].abs) > options.axisDeadzone)
				{
					keysPressedCurrentUpdate[options.joystickAxis[newAxis]] = true;
				}
			}

			for (int j = 0; j < 4; j++)
			{
				if (options.joystickAxis[j] == Keys::K_MOVEMENT_FORWARD ||
					options.joystickAxis[j] == Keys::K_MOVEMENT_BACK ||
					options.joystickAxis[j] == Keys::K_MOVEMENT_LEFT ||
					options.joystickAxis[j] == Keys::K_MOVEMENT_RIGHT)
				{
					movementCounter++;
				}
			}
			if (movementCounter == 4)
			{
				useDegreeMovement = true;
			}
			if (!useDegreeMovement)
			{
				for (int j = 4; j < 8; j++)
				{
					if (options.joystickAxis[j] == Keys::K_MOVEMENT_FORWARD ||
						options.joystickAxis[j] == Keys::K_MOVEMENT_BACK ||
						options.joystickAxis[j] == Keys::K_MOVEMENT_LEFT ||
						options.joystickAxis[j] == Keys::K_MOVEMENT_RIGHT)
					{
						movementCounter++;
					}
				}
				if (movementCounter == 4)
				{
					useDegreeMovement = true;
				}
			}
			if (useDegreeMovement)
			{
				double y = 0;
				double x = 0;
				if (options.joystickAxis[0] == Keys::K_MOVEMENT_FORWARD ||
					options.joystickAxis[0] == Keys::K_MOVEMENT_BACK ||
					options.joystickAxis[0] == Keys::K_MOVEMENT_LEFT ||
					options.joystickAxis[0] == Keys::K_MOVEMENT_RIGHT)
				{
					y = joystickState.mAxes[0].abs;
					x = joystickState.mAxes[1].abs;
				}
				if (options.joystickAxis[4] == Keys::K_MOVEMENT_FORWARD ||
					options.joystickAxis[4] == Keys::K_MOVEMENT_BACK ||
					options.joystickAxis[4] == Keys::K_MOVEMENT_LEFT ||
					options.joystickAxis[4] == Keys::K_MOVEMENT_RIGHT)
				{
				}
				if (Math::Abs(y) > options.axisDeadzone)
				{
					if (y > 0)
					{
						y -= options.axisDeadzone;
					}
					else
					{
						y += options.axisDeadzone;
					}

					if (options.axisDeadzone < options.axisMax)
					{
						y /= (options.axisMax - options.axisDeadzone);
					}
					else
					{
						y = 0;
					}
				}
				else
				{
					y = 0;
				}
				if (Math::Abs(x) > options.axisDeadzone)
				{
					if (x > 0)
					{
						x -= options.axisDeadzone;
					}
					else
					{
						x += options.axisDeadzone;
					}

					if (options.axisDeadzone < options.axisMax)
					{
						x /= (options.axisMax - options.axisDeadzone);
					}
					else
					{
						x = 0;
					}

				}
				else
				{
					x = 0;
				}

				movementVector.x = x;
				movementVector.z = y;
				movementVector.normalise();
			}
		}
	}
	
#ifdef DONT_DEFINE_ME
	if (joystick == 0)
	{
		if (keyboard.isKeyDown(OIS::case OIS::KC_E))
		{
			keysPressedCurrentUpdate[K_ACTION] = true;
		}
		if (keyboard.isKeyDown(OIS::case OIS::KC_F10))
		{
			keysPressedCurrentUpdate[K_TOGGLE_FULL_SCREEN] = true;
		}
		if (keyboard.isKeyDown(OIS::case OIS::KC_R))
		{
			keysPressedCurrentUpdate[K_RESPAWN] = true;
		}
		if (keyboard.isKeyDown(OIS::case OIS::KC_F11))
		{
			keysPressedCurrentUpdate[K_GO_TO_MENU] = true;
		}
		if (keyboard.isKeyDown(OIS::case OIS::KC_F12))
		{
			keysPressedCurrentUpdate[K_EXIT_GAME] = true;
		}
		if (keyboard.isKeyDown(OIS::case OIS::KC_ESCAPE))
		{
			keysPressedCurrentUpdate[K_PAUSE] = true;
		}
		if (keyboard.isKeyDown(OIS::case OIS::KC_F1))
		{
			keysPressedCurrentUpdate[K_TOGGLE_DEBUG_MODE] = true;
		}
		if (keyboard.isKeyDown(OIS::case OIS::KC_1))
		{
			keysPressedCurrentUpdate[K_EXTRA] = true;
		}
		if (keyboard.isKeyDown(OIS::case OIS::KC_2))
		{
			keysPressedCurrentUpdate[K_EXTRA2] = true;
		}
		if (keyboard.isKeyDown(OIS::case OIS::KC_W))
		{
			keysPressedCurrentUpdate[K_MOVEMENT_FORWARD] = true;
		}
		if (keyboard.isKeyDown(OIS::case OIS::KC_S))
		{
			keysPressedCurrentUpdate[K_MOVEMENT_BACK] = true;
		}
		if (keyboard.isKeyDown(OIS::case OIS::KC_A))
		{
			keysPressedCurrentUpdate[K_MOVEMENT_LEFT] = true;
		}
		if (keyboard.isKeyDown(OIS::case OIS::KC_D))
		{
			keysPressedCurrentUpdate[K_MOVEMENT_RIGHT] = true;
		}
		/*if (mouse.getMouseState().)
		{
			keysPressedCurrentUpdate[K_CAMERA_TOP] = true;
		}
		if (state.mAxes[2].abs > DEADZONE)
		{
			keysPressedCurrentUpdate[K_CAMERA_BOTTOM] = true;
		}
		if (state.mAxes[3].abs < -DEADZONE)
		{
			keysPressedCurrentUpdate[K_CAMERA_LEFT] = true;
		}
		if (state.mAxes[3].abs > DEADZONE)
		{
			keysPressedCurrentUpdate[K_CAMERA_RIGHT] = true;
		}*/
		if (mouse != 0)
		{
			if (mouse->getMouseState().buttonDown(OIS::MouseButtonID::MB_Left))
			{
				keysPressedCurrentUpdate[K_JUMP_NORMAL] = true;
			}
			if (mouse->getMouseState().buttonDown(OIS::MouseButtonID::MB_Right))
			{
				keysPressedCurrentUpdate[K_JUMP_LONG] = true;
			}
			if (mouse->getMouseState().buttonDown(OIS::MouseButtonID::MB_Middle))
			{
				keysPressedCurrentUpdate[K_CAMERA_ZOOM_RESET] = true;
			}
		}
		/*if (state.mButtons[9])
		{
			keysPressedCurrentUpdate[K_CAMERA_CENTER] = true;
		}
		if (state.mAxes[4].abs < -DEADZONE)
		{
			keysPressedCurrentUpdate[K_CAMERA_ZOOM_REDUCE] = true;
		}
		if (state.mAxes[4].abs > DEADZONE)
		{
			keysPressedCurrentUpdate[K_CAMERA_ZOOM_ADD] = true;
		}*/
		if (mouse != 0)
		{
			MouseX = mouse->getMouseState().X.rel;
			MouseY = mouse->getMouseState().Y.rel;
			Wheel = mouse->getMouseState().Z.rel;
		}
		else
		{
			MouseX = 0;
			MouseY = 0;
			Wheel = 0;
		}
	}
	else
	{
		MouseX = 0, MouseY = 0;
		OIS::JoyStickState state = joystick->getJoyStickState();
		if (state.mButtons[4])
		{
			keysPressedCurrentUpdate[K_EXTRA] = true;
		}
		if (state.mButtons[5])
		{
			keysPressedCurrentUpdate[K_EXTRA2] = true;
		}
		if (state.mAxes[0].abs < -DEADZONE)
		{
			keysPressedCurrentUpdate[K_MOVEMENT_FORWARD] = true;
		}
		if (state.mAxes[0].abs > DEADZONE)
		{
			keysPressedCurrentUpdate[K_MOVEMENT_BACK] = true;
		}
		if (state.mAxes[1].abs < -DEADZONE)
		{
			keysPressedCurrentUpdate[K_MOVEMENT_LEFT] = true;
		}
		if (state.mAxes[1].abs > DEADZONE)
		{
			keysPressedCurrentUpdate[K_MOVEMENT_RIGHT] = true;
		}
		if (state.mAxes[2].abs < -DEADZONE)
		{
			MouseY = joystickSensitivity * (joystick->MIN_AXIS / state.mAxes[2].abs);
			MouseY *= -1;
			keysPressedCurrentUpdate[K_CAMERA_TOP] = true;
		}
		if (state.mAxes[2].abs > DEADZONE)
		{
			MouseY = joystickSensitivity * (joystick->MAX_AXIS / state.mAxes[2].abs);
			keysPressedCurrentUpdate[K_CAMERA_BOTTOM] = true;
		}
		if (state.mAxes[3].abs < -DEADZONE)
		{
			MouseX = joystickSensitivity * (joystick->MIN_AXIS / state.mAxes[3].abs);
			MouseX *= -1;
			keysPressedCurrentUpdate[K_CAMERA_LEFT] = true;
		}
		if (state.mAxes[3].abs > DEADZONE)
		{
			MouseX = joystickSensitivity * (joystick->MAX_AXIS / state.mAxes[3].abs);
			keysPressedCurrentUpdate[K_CAMERA_RIGHT] = true;
		}
		if (state.mButtons[1])
		{
			keysPressedCurrentUpdate[K_JUMP_NORMAL] = true;
		}
		if (state.mButtons[3])
		{
			keysPressedCurrentUpdate[K_JUMP_BACKWARD] = true;
		}
		if (state.mButtons[0])
		{
			keysPressedCurrentUpdate[K_JUMP_LONG] = true;
		}
		if (state.mButtons[8])
		{
			keysPressedCurrentUpdate[K_CAMERA_ZOOM_RESET] = true;
		}
		if (state.mButtons[9])
		{
			keysPressedCurrentUpdate[K_CAMERA_CENTER] = true;
		}
		if (state.mAxes[4].abs < -DEADZONE)
		{
			keysPressedCurrentUpdate[K_CAMERA_ZOOM_REDUCE] = true;
		}
		if (state.mAxes[4].abs > DEADZONE)
		{
			keysPressedCurrentUpdate[K_CAMERA_ZOOM_ADD] = true;
		}
	}
	
	if (keyboard.isKeyDown(OIS::case OIS::KC_F1))
	{
		keysPressedCurrentUpdate[K_TOGGLE_DEBUG_MODE] = true;
	}
	if (state.mButtons[4])
	{
		keysPressedCurrentUpdate[K_EXTRA] = true;
	}
	if (state.mButtons[5])
	{
		keysPressedCurrentUpdate[K_EXTRA2] = true;
	}
	if (state.mAxes[0].abs < -DEADZONE)
	{
		keysPressedCurrentUpdate[K_MOVEMENT_FORWARD] = true;
	}
	if (state.mAxes[0].abs > DEADZONE)
	{
		keysPressedCurrentUpdate[K_MOVEMENT_BACK] = true;
	}
	if (state.mAxes[1].abs < -DEADZONE)
	{
		keysPressedCurrentUpdate[K_MOVEMENT_LEFT] = true;
	}
	if (state.mAxes[1].abs > DEADZONE)
	{
		keysPressedCurrentUpdate[K_MOVEMENT_RIGHT] = true;
	}
	if (state.mAxes[2].abs < -DEADZONE)
	{
		keysPressedCurrentUpdate[K_CAMERA_TOP] = true;
	}
	if (state.mAxes[2].abs > DEADZONE)
	{
		keysPressedCurrentUpdate[K_CAMERA_BOTTOM] = true;
	}
	if (state.mAxes[3].abs < -DEADZONE)
	{
		keysPressedCurrentUpdate[K_CAMERA_LEFT] = true;
	}
	if (state.mAxes[3].abs > DEADZONE)
	{
		keysPressedCurrentUpdate[K_CAMERA_RIGHT] = true;
	}
	if (state.mButtons[1])
	{
		keysPressedCurrentUpdate[K_JUMP_NORMAL] = true;
	}
	if (state.mButtons[3])
	{
		keysPressedCurrentUpdate[K_JUMP_BACKWARD] = true;
	}
	if (state.mButtons[0])
	{
		keysPressedCurrentUpdate[K_JUMP_LONG] = true;
	}
	if (state.mButtons[8])
	{
		keysPressedCurrentUpdate[K_CAMERA_ZOOM_RESET] = true;
	}
	if (state.mButtons[9])
	{
		keysPressedCurrentUpdate[K_CAMERA_CENTER] = true;
	}
	if (state.mAxes[4].abs < -DEADZONE)
	{
		keysPressedCurrentUpdate[K_CAMERA_ZOOM_REDUCE] = true;
	}
	if (state.mAxes[4].abs > DEADZONE)
	{
		keysPressedCurrentUpdate[K_CAMERA_ZOOM_ADD] = true;
	}
#endif
}

bool Keys::updateKeyMapping(Keys::KeyName keyname, ConfigOptions& options, OIS::Mouse* mouse, OIS::JoyStick* joystick)
{
	if (mPressedKeyCodes.size() > 0)
	{
		for (int i = 0; i < mPressedKeyCodes.size(); i++)
		{
			if (validKeyCode(mPressedKeyCodes[i]))
			{
				options.keyMapping[keyname] = mPressedKeyCodes[i];
				return true;
			}
		}
	}

	const OIS::MouseState& mouseState = mouse->getMouseState();
	if (mouseState.buttonDown(OIS::MouseButtonID::MB_Left))
	{
		options.leftClick = keyname;
				return true;
	}
	if (mouseState.buttonDown(OIS::MouseButtonID::MB_Right))
	{
		options.rightClick = keyname;
				return true;
	}
	if (mouseState.buttonDown(OIS::MouseButtonID::MB_Middle))
	{
		options.middleClick  = keyname;
				return true;
	}
	if (mouseState.Z.rel != 0)
	{
		if (mouseState.Z.rel > 0)
		{
			options.wheelUp = keyname;
		}
		else 
		{
			options.wheelDown = keyname;
		}
		return true;
	}

	// TODO joystick

	return false;
}

void Keys::onRecreateInputSystem(OIS::Keyboard& keyboard)
{
	mPressedKeyCodes.clear();
	keyboard.setEventCallback(this);
}

// keyboard
bool Keys::keyPressed(const OIS::KeyEvent &arg)
{
	mPressedKeyCodes.push_back(arg.key);
	return true;
}
bool Keys::keyReleased(const OIS::KeyEvent &arg)
{
	mPressedKeyCodes.erase(remove(mPressedKeyCodes.begin(), mPressedKeyCodes.end(), arg.key), mPressedKeyCodes.end());
	return true;
}

// joystick
bool Keys::buttonPressed(const OIS::JoyStickEvent &arg, int button)
{
	mPressedJoystickButtons.push_back(button);

	return true;
}
bool Keys::buttonReleased(const OIS::JoyStickEvent &arg, int button)
{
	mPressedKeyCodes.erase(remove(mPressedKeyCodes.begin(), mPressedKeyCodes.end(), button), mPressedKeyCodes.end());

	return true;
}
bool Keys::axisMoved(const OIS::JoyStickEvent &arg, int axis)
{
	int relative = arg.state.mAxes[axis].rel;
	axis *= 2;
	if (relative > 0)
	{
		axis += 1;
	}
	if (axis >= 0 && axis < NUMBER_OF_AXIS)
	{
		mJoystickAxesMovement[axis] = Math::Abs(relative);
	}

	return true;
}
bool Keys::sliderMoved(const OIS::JoyStickEvent &, int index)
{
	return true;
}
bool Keys::povMoved(const OIS::JoyStickEvent &arg, int index)
{
	return true;
}
bool Keys::vector3Moved(const OIS::JoyStickEvent &arg, int index)
{
	return true;
}

bool Keys::validKeyCode(OIS::KeyCode keycode)
{
	switch (keycode)
	{
		case OIS::KC_ESCAPE:
			//"Escape";
			//;//01,
		case OIS::KC_GRAVE:
		case OIS::KC_1 :
			//"1";
			//;//01,
		case OIS::KC_2 :
			//"2";
			//;
		case OIS::KC_3 :
			//"3";
			//;
		case OIS::KC_4 :
			//"4";
			//;
		case OIS::KC_5 :
			//"5";
			//;
		case OIS::KC_6 :
			//"6";
			//;
		case OIS::KC_7 :
			//"7";
			//;
		case OIS::KC_8 :
			//"8";
			//;
		case OIS::KC_9 :
			//"9";
			//;
		case OIS::KC_0 :
			//"0";
			//;
		case OIS::KC_MINUS :
			//"-";
			//;
		case OIS::KC_EQUALS:
			//"=";
			//;
		case OIS::KC_BACK:
			//"Backspace";
			//;
		case OIS::KC_TAB :
			//"Tab";
			//;
		case OIS::KC_Q :
			//"Q";
			//;
		case OIS::KC_W :
			//"W";
			//;
		case OIS::KC_E :
			//"E";
			//;
		case OIS::KC_R :
			//"R";
			//;
		case OIS::KC_T :
			//"T";
			//;
		case OIS::KC_Y :
			//"Y";
			//;
		case OIS::KC_U :
			//"U";
			//;
		case OIS::KC_I :
			//"I";
			//;
		case OIS::KC_O :
			//"O";
			//;
		case OIS::KC_P :
			//"P";
			//;
		case OIS::KC_RETURN:
			//"Enter";
			//;
		case OIS::KC_LCONTROL:
			//"Left control";
			//;
		case OIS::KC_A :
			//"A";
			//;
		case OIS::KC_S :
			//"S";
			//;
		case OIS::KC_D :
			//"D";
			//;
		case OIS::KC_F :
			//"F";
			//;
		case OIS::KC_G :
			//"G";
			//;
		case OIS::KC_H :
			//"H";
			//;
		case OIS::KC_J :
			//"J";
			//;
		case OIS::KC_K :
			//"K";
			//;
		case OIS::KC_L :
			//"L";
			//;
		case OIS::KC_SEMICOLON :
			//";";
			//;
		case OIS::KC_APOSTROPHE:
			//"'";
			//;
		case OIS::KC_LSHIFT:
			//"Left shift";
			//;
		case OIS::KC_BACKSLASH :
			//"\\";
			//;
		case OIS::KC_Z :
			//"Z";
			//;
		case OIS::KC_X :
			//"X";
			//;
		case OIS::KC_C :
			//"C";
			//;
		case OIS::KC_V :
			//"V";
			//;
		case OIS::KC_B :
			//"B";
			//;
		case OIS::KC_N :
			//"N";
			//;
		case OIS::KC_M :
			//"M";
			//;
		case OIS::KC_COMMA :
			//",";
			//;
		case OIS::KC_PERIOD:
			//".";
			//;
		case OIS::KC_SLASH :
			//"/";
			//;
		case OIS::KC_RSHIFT:
			//"Right shift";
			//;
		case OIS::KC_MULTIPLY:
			//"*";
			//;
		case OIS::KC_LMENU :
			//"Left alt";
			//;
		case OIS::KC_SPACE :
			//"Space";
			//;
		case OIS::KC_CAPITAL :
			//"Caps lock";
			//;
		case OIS::KC_F1:
			//"F1";
			//;
		case OIS::KC_F2:
			//"F2";
			//;
		case OIS::KC_F3:
			//"F3";
			//;
		case OIS::KC_F4:
			//"F4";
			//;
		case OIS::KC_F5:
			//"F5";
			//;
		case OIS::KC_F6:
			//"F6";
			//;
		case OIS::KC_F7:
			//"F7";
			//;
		case OIS::KC_F8:
			//"F8";
			//;
		case OIS::KC_NUMPAD7 :
			//"7 (numpad)";
			//;
		case OIS::KC_NUMPAD8 :
			//"8 (numpad)";
			//;
		case OIS::KC_NUMPAD9 :
			//"9 (numpad)";
			//;
		case OIS::KC_SUBTRACT://4A,:// - on numeric keypad
			//"- (numpad)";
			//;
		case OIS::KC_NUMPAD4 ://4B,
			//"4 (numpad)";
			//;
		case OIS::KC_NUMPAD5 ://4C,
			//"5 (numpad)";
			//;
		case OIS::KC_NUMPAD6 ://4D,
			//"6 (numpad)";
			//;
		case OIS::KC_ADD ://4E,:// + on numeric keypad
			//"+ (numpad)";
			//;
		case OIS::KC_NUMPAD1 ://4F,
			//"1 (numpad)";
			//;
		case OIS::KC_NUMPAD2 ://50,
			//"2 (numpad)";
			//;
		case OIS::KC_NUMPAD3 ://51,
			//"3 (numpad)";
			//;
		case OIS::KC_NUMPAD0 ://52,
			//"0 (numpad)";
			//;
		case OIS::KC_DECIMAL ://53,:// . on numeric keypad
			//". (numpad)";
			//;
		case OIS::KC_NUMPADENTER://9C,:// Enter on numeric keypad
			//"Enter (numpad)";
			//;
		case OIS::KC_RCONTROL://9D,
			//"Right control";
			//;
		case OIS::KC_NUMPADCOMMA://B3,:// , on numeric keypad (NEC PC98)
			//", (numpad)";
			//;
		case OIS::KC_DIVIDE://B5,:// / on numeric keypad
			//"/ (numpad)";
			//;
		case OIS::KC_RMENU ://B8,:// right Alt
			//"Right alt";
			//;
		case OIS::KC_LEFT://CB,:// LeftArrow on arrow keypad
			//"Left arrow";
			//;
		case OIS::KC_RIGHT ://CD,:// RightArrow on arrow keypad
			//"Right arrow";
			//;
		case OIS::KC_UP ://CF,:// End on arrow keypad
			//"Up arrow";
			//;
		case OIS::KC_DOWN://D0,:// DownArrow on arrow keypad
			//"Down arrow";
			//;
			return true;
			break;
	}
	return false;
}

Ogre::String Keys::key2string(Keys::KeyName keyname, ConfigOptions& options)
{
	if (options.useKeyboardAndMouse)
	{
		if (options.useMouseForCameraMovement)
		{
			if (
				keyname == Keys::K_CAMERA_TOP ||
				keyname == Keys::K_CAMERA_BOTTOM ||
				keyname == Keys::K_CAMERA_LEFT ||
				keyname == Keys::K_CAMERA_RIGHT
			)
			{
				return "Mouse";
			}
		}
		if (options.keyMapping[keyname] != OIS::KC_PAUSE)
		{
			return Keys::key2string(options.keyMapping[keyname]);
		}

		if (options.leftClick == keyname)
		{
			return "Left click";
		}
		if (options.rightClick == keyname)
		{
			return "Right click";
		}
		if (options.middleClick == keyname)
		{
			return "Middle click";
		}
		if (options.wheelUp == keyname)
		{
			return "Wheel up";
		}
		if (options.wheelDown == keyname)
		{
			return "Wheel down";
		}
	}
	else
	{
		int joystickNumber = options.joystickMapping[keyname];
		if (joystickNumber != -1)
		{
			switch (joystickNumber)
			{
			case 0:
				return "A";
				break;
			case 1:
				return "B";
				break;
			case 2:
				return "X";
				break;
			case 3:
				return "Y";
				break;
			case 4:
				return "LB";
				break;
			case 5:
				return "RB";
				break;
			case 6:
				return "Back";
				break;
			case 7:
				return "Start";
				break;
			case 8:
				return "L3";
				break;
			case 9:
				return "R3";
				break;
			}
		}
		return "None";
	}
}
Ogre::String Keys::key2string(OIS::KeyCode keycode)
{
	if (keycode == OIS::KC_PAUSE)
	{
		return "None";
	}

	switch (keycode)
	{
		case OIS::KC_ESCAPE:
			return "Escape";
			break;//01,
		case OIS::KC_GRAVE:
			return "~";
			break;
		case OIS::KC_1 :
			return "1";
			break;//01,
		case OIS::KC_2 :
			return "2";
			break;
		case OIS::KC_3 :
			return "3";
			break;
		case OIS::KC_4 :
			return "4";
			break;
		case OIS::KC_5 :
			return "5";
			break;
		case OIS::KC_6 :
			return "6";
			break;
		case OIS::KC_7 :
			return "7";
			break;
		case OIS::KC_8 :
			return "8";
			break;
		case OIS::KC_9 :
			return "9";
			break;
		case OIS::KC_0 :
			return "0";
			break;
		case OIS::KC_MINUS :
			return "-";
			break;
		case OIS::KC_EQUALS:
			return "=";
			break;
		case OIS::KC_BACK:
			return "Backspace";
			break;
		case OIS::KC_TAB :
			return "Tab";
			break;
		case OIS::KC_Q :
			return "Q";
			break;
		case OIS::KC_W :
			return "W";
			break;
		case OIS::KC_E :
			return "E";
			break;
		case OIS::KC_R :
			return "R";
			break;
		case OIS::KC_T :
			return "T";
			break;
		case OIS::KC_Y :
			return "Y";
			break;
		case OIS::KC_U :
			return "U";
			break;
		case OIS::KC_I :
			return "I";
			break;
		case OIS::KC_O :
			return "O";
			break;
		case OIS::KC_P :
			return "P";
			break;
		case OIS::KC_RETURN:
			return "Enter";
			break;
		case OIS::KC_LCONTROL:
			return "Left control";
			break;
		case OIS::KC_A :
			return "A";
			break;
		case OIS::KC_S :
			return "S";
			break;
		case OIS::KC_D :
			return "D";
			break;
		case OIS::KC_F :
			return "F";
			break;
		case OIS::KC_G :
			return "G";
			break;
		case OIS::KC_H :
			return "H";
			break;
		case OIS::KC_J :
			return "J";
			break;
		case OIS::KC_K :
			return "K";
			break;
		case OIS::KC_L :
			return "L";
			break;
		case OIS::KC_SEMICOLON :
			return ";";
			break;
		case OIS::KC_APOSTROPHE:
			return "'";
			break;
		case OIS::KC_LSHIFT:
			return "Left shift";
			break;
		case OIS::KC_BACKSLASH :
			return "\\";
			break;
		case OIS::KC_Z :
			return "Z";
			break;
		case OIS::KC_X :
			return "X";
			break;
		case OIS::KC_C :
			return "C";
			break;
		case OIS::KC_V :
			return "V";
			break;
		case OIS::KC_B :
			return "B";
			break;
		case OIS::KC_N :
			return "N";
			break;
		case OIS::KC_M :
			return "M";
			break;
		case OIS::KC_COMMA :
			return ",";
			break;
		case OIS::KC_PERIOD:
			return ".";
			break;
		case OIS::KC_SLASH :
			return "/";
			break;
		case OIS::KC_RSHIFT:
			return "Right shift";
			break;
		case OIS::KC_MULTIPLY:
			return "* (numpad)";
			break;
		case OIS::KC_LMENU :
			return "Left alt";
			break;
		case OIS::KC_SPACE :
			return "Space";
			break;
		case OIS::KC_CAPITAL :
			return "Caps lock";
			break;
		case OIS::KC_F1:
			return "F1";
			break;
		case OIS::KC_F2:
			return "F2";
			break;
		case OIS::KC_F3:
			return "F3";
			break;
		case OIS::KC_F4:
			return "F4";
			break;
		case OIS::KC_F5:
			return "F5";
			break;
		case OIS::KC_F6:
			return "F6";
			break;
		case OIS::KC_F7:
			return "F7";
			break;
		case OIS::KC_F8:
			return "F8";
			break;
		case OIS::KC_NUMPAD7 :
			return "7 (numpad)";
			break;
		case OIS::KC_NUMPAD8 :
			return "8 (numpad)";
			break;
		case OIS::KC_NUMPAD9 :
			return "9 (numpad)";
			break;
		case OIS::KC_SUBTRACT://4A,:// - on numeric keypad
			return "- (numpad)";
			break;
		case OIS::KC_NUMPAD4 ://4B,
			return "4 (numpad)";
			break;
		case OIS::KC_NUMPAD5 ://4C,
			return "5 (numpad)";
			break;
		case OIS::KC_NUMPAD6 ://4D,
			return "6 (numpad)";
			break;
		case OIS::KC_ADD ://4E,:// + on numeric keypad
			return "+ (numpad)";
			break;
		case OIS::KC_NUMPAD1 ://4F,
			return "1 (numpad)";
			break;
		case OIS::KC_NUMPAD2 ://50,
			return "2 (numpad)";
			break;
		case OIS::KC_NUMPAD3 ://51,
			return "3 (numpad)";
			break;
		case OIS::KC_NUMPAD0 ://52,
			return "0 (numpad)";
			break;
		case OIS::KC_DECIMAL ://53,:// . on numeric keypad
			return ". (numpad)";
			break;
		case OIS::KC_NUMPADENTER://9C,:// Enter on numeric keypad
			return "Enter (numpad)";
			break;
		case OIS::KC_RCONTROL://9D,
			return "Right control";
			break;
		case OIS::KC_NUMPADCOMMA://B3,:// , on numeric keypad (NEC PC98)
			return ", (numpad)";
			break;
		case OIS::KC_DIVIDE://B5,:// / on numeric keypad
			return "/ (numpad)";
			break;
		case OIS::KC_RMENU ://B8,:// right Alt
			return "Right alt";
			break;
		case OIS::KC_LEFT://CB,:// LeftArrow on arrow keypad
			return "Left arrow";
			break;
		case OIS::KC_RIGHT ://CD,:// RightArrow on arrow keypad
			return "Right arrow";
			break;
		case OIS::KC_UP ://CF,:// End on arrow keypad
			return "Up arrow";
			break;
		case OIS::KC_DOWN://D0,:// DownArrow on arrow keypad
			return "Down arrow";
			break;
	}

	return "None";
}

Keys* Keys::singleton = new Keys();
//Keys::KeyIdentifierMap Keys::key_identifiers = KeyIdentifierMap();