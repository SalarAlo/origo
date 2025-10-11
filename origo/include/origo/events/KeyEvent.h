#pragma once

#include "origo/events/Event.h"
#include "origo/events/EventTypes.h"

namespace Origo {
#pragma region KEYBOARD_KEYS
enum class KeyboardKey {
	KEY_A,
	KEY_B,
	KEY_C,
	KEY_D,
	KEY_E,
	KEY_F,
	KEY_G,
	KEY_H,
	KEY_I,
	KEY_J,
	KEY_K,
	KEY_L,
	KEY_M,
	KEY_N,
	KEY_O,
	KEY_P,
	KEY_Q,
	KEY_R,
	KEY_S,
	KEY_T,
	KEY_U,
	KEY_V,
	KEY_W,
	KEY_X,
	KEY_Y,
	KEY_Z,

	KEY_0,
	KEY_1,
	KEY_2,
	KEY_3,
	KEY_4,
	KEY_5,
	KEY_6,
	KEY_7,
	KEY_8,
	KEY_9,

	KEY_SPACE,
	KEY_ENTER,
	KEY_ESCAPE,
	KEY_TAB,
	KEY_BACKSPACE,
	KEY_LEFT_SHIFT,
	KEY_RIGHT_SHIFT,
	KEY_LEFT_CONTROL,
	KEY_RIGHT_CONTROL,
	KEY_LEFT_ALT,
	KEY_RIGHT_ALT,
	KEY_UP,
	KEY_DOWN,
	KEY_LEFT,
	KEY_RIGHT,

	KEY_UNKNOWN
};
#pragma endregion

class KeyEvent : public Event {
public:
	EventType GetEventType() const override;
	virtual KeyEventType GetKeyEventType() const = 0;

private:
};

class KeyPressEvent : public KeyEvent {
public:
	KeyPressEvent(int glfwKey, KeyPressType keyPressType);
	KeyPressType GetKeyPressType() const;
	KeyboardKey GetKeyPressed() const;
	KeyEventType GetKeyEventType() const override;

private:
	KeyPressType m_KeyPressType;
	KeyboardKey m_KeyPressed;
};

};
