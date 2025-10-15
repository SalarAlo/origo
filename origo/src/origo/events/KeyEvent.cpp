#include "origo/events/KeyEvent.h"
#include "origo/events/EventTypes.h"

namespace Origo {
#pragma region KEY_MAP
KeyboardKey GlfwKeyToKeyboardKey(int glfwKey) {
	using namespace Origo;
	using enum KeyboardKey;
	switch (glfwKey) {
	// Letters
	case GLFW_KEY_A:
		return KEY_A;
	case GLFW_KEY_B:
		return KEY_B;
	case GLFW_KEY_C:
		return KEY_C;
	case GLFW_KEY_D:
		return KEY_D;
	case GLFW_KEY_E:
		return KEY_E;
	case GLFW_KEY_F:
		return KEY_F;
	case GLFW_KEY_G:
		return KEY_G;
	case GLFW_KEY_H:
		return KEY_H;
	case GLFW_KEY_I:
		return KEY_I;
	case GLFW_KEY_J:
		return KEY_J;
	case GLFW_KEY_K:
		return KEY_K;
	case GLFW_KEY_L:
		return KEY_L;
	case GLFW_KEY_M:
		return KEY_M;
	case GLFW_KEY_N:
		return KEY_N;
	case GLFW_KEY_O:
		return KEY_O;
	case GLFW_KEY_P:
		return KEY_P;
	case GLFW_KEY_Q:
		return KEY_Q;
	case GLFW_KEY_R:
		return KEY_R;
	case GLFW_KEY_S:
		return KEY_S;
	case GLFW_KEY_T:
		return KEY_T;
	case GLFW_KEY_U:
		return KEY_U;
	case GLFW_KEY_V:
		return KEY_V;
	case GLFW_KEY_W:
		return KEY_W;
	case GLFW_KEY_X:
		return KEY_X;
	case GLFW_KEY_Y:
		return KEY_Y;
	case GLFW_KEY_Z:
		return KEY_Z;

	// Digits
	case GLFW_KEY_0:
		return KEY_0;
	case GLFW_KEY_1:
		return KEY_1;
	case GLFW_KEY_2:
		return KEY_2;
	case GLFW_KEY_3:
		return KEY_3;
	case GLFW_KEY_4:
		return KEY_4;
	case GLFW_KEY_5:
		return KEY_5;
	case GLFW_KEY_6:
		return KEY_6;
	case GLFW_KEY_7:
		return KEY_7;
	case GLFW_KEY_8:
		return KEY_8;
	case GLFW_KEY_9:
		return KEY_9;

	// Control and navigation keys
	case GLFW_KEY_SPACE:
		return KEY_SPACE;
	case GLFW_KEY_ENTER:
		return KEY_ENTER;
	case GLFW_KEY_ESCAPE:
		return KEY_ESCAPE;
	case GLFW_KEY_TAB:
		return KEY_TAB;
	case GLFW_KEY_BACKSPACE:
		return KEY_BACKSPACE;
	case GLFW_KEY_LEFT_SHIFT:
		return KEY_LEFT_SHIFT;
	case GLFW_KEY_RIGHT_SHIFT:
		return KEY_RIGHT_SHIFT;
	case GLFW_KEY_LEFT_CONTROL:
		return KEY_LEFT_CONTROL;
	case GLFW_KEY_RIGHT_CONTROL:
		return KEY_RIGHT_CONTROL;
	case GLFW_KEY_LEFT_ALT:
		return KEY_LEFT_ALT;
	case GLFW_KEY_RIGHT_ALT:
		return KEY_RIGHT_ALT;
	case GLFW_KEY_UP:
		return KEY_UP;
	case GLFW_KEY_DOWN:
		return KEY_DOWN;
	case GLFW_KEY_LEFT:
		return KEY_LEFT;
	case GLFW_KEY_RIGHT:
		return KEY_RIGHT;

	// Default fallback
	default:
		return KEY_UNKNOWN;
	}
}

int KeyboardKeyToGlfwKey(Origo::KeyboardKey key) {
	using namespace Origo;
	using enum KeyboardKey;

	switch (key) {
	// Letters
	case KEY_A:
		return GLFW_KEY_A;
	case KEY_B:
		return GLFW_KEY_B;
	case KEY_C:
		return GLFW_KEY_C;
	case KEY_D:
		return GLFW_KEY_D;
	case KEY_E:
		return GLFW_KEY_E;
	case KEY_F:
		return GLFW_KEY_F;
	case KEY_G:
		return GLFW_KEY_G;
	case KEY_H:
		return GLFW_KEY_H;
	case KEY_I:
		return GLFW_KEY_I;
	case KEY_J:
		return GLFW_KEY_J;
	case KEY_K:
		return GLFW_KEY_K;
	case KEY_L:
		return GLFW_KEY_L;
	case KEY_M:
		return GLFW_KEY_M;
	case KEY_N:
		return GLFW_KEY_N;
	case KEY_O:
		return GLFW_KEY_O;
	case KEY_P:
		return GLFW_KEY_P;
	case KEY_Q:
		return GLFW_KEY_Q;
	case KEY_R:
		return GLFW_KEY_R;
	case KEY_S:
		return GLFW_KEY_S;
	case KEY_T:
		return GLFW_KEY_T;
	case KEY_U:
		return GLFW_KEY_U;
	case KEY_V:
		return GLFW_KEY_V;
	case KEY_W:
		return GLFW_KEY_W;
	case KEY_X:
		return GLFW_KEY_X;
	case KEY_Y:
		return GLFW_KEY_Y;
	case KEY_Z:
		return GLFW_KEY_Z;

	// Digits
	case KEY_0:
		return GLFW_KEY_0;
	case KEY_1:
		return GLFW_KEY_1;
	case KEY_2:
		return GLFW_KEY_2;
	case KEY_3:
		return GLFW_KEY_3;
	case KEY_4:
		return GLFW_KEY_4;
	case KEY_5:
		return GLFW_KEY_5;
	case KEY_6:
		return GLFW_KEY_6;
	case KEY_7:
		return GLFW_KEY_7;
	case KEY_8:
		return GLFW_KEY_8;
	case KEY_9:
		return GLFW_KEY_9;

	// Control and navigation keys
	case KEY_SPACE:
		return GLFW_KEY_SPACE;
	case KEY_ENTER:
		return GLFW_KEY_ENTER;
	case KEY_ESCAPE:
		return GLFW_KEY_ESCAPE;
	case KEY_TAB:
		return GLFW_KEY_TAB;
	case KEY_BACKSPACE:
		return GLFW_KEY_BACKSPACE;
	case KEY_LEFT_SHIFT:
		return GLFW_KEY_LEFT_SHIFT;
	case KEY_RIGHT_SHIFT:
		return GLFW_KEY_RIGHT_SHIFT;
	case KEY_LEFT_CONTROL:
		return GLFW_KEY_LEFT_CONTROL;
	case KEY_RIGHT_CONTROL:
		return GLFW_KEY_RIGHT_CONTROL;
	case KEY_LEFT_ALT:
		return GLFW_KEY_LEFT_ALT;
	case KEY_RIGHT_ALT:
		return GLFW_KEY_RIGHT_ALT;
	case KEY_UP:
		return GLFW_KEY_UP;
	case KEY_DOWN:
		return GLFW_KEY_DOWN;
	case KEY_LEFT:
		return GLFW_KEY_LEFT;
	case KEY_RIGHT:
		return GLFW_KEY_RIGHT;

	// Default fallback
	default:
		return GLFW_KEY_UNKNOWN;
	}
}
#pragma endregion

#pragma region KEY_PRESS_EVENT

KeyPressEvent::KeyPressEvent(int glfwKey, KeyPressType keyPressType)
    : m_KeyPressed(GlfwKeyToKeyboardKey(glfwKey))
    , m_KeyPressType(keyPressType) {
}

KeyboardKey KeyPressEvent::GetKeyPressed() const {
	return m_KeyPressed;
}

EventType KeyPressEvent::GetEventType() const {
	return GetStaticType();
}

EventType KeyPressEvent::GetStaticType() {
	return EventType::KeyPress;
}

KeyPressType KeyPressEvent::GetKeyPressType() const {
	return m_KeyPressType;
}

#pragma endregion

}
