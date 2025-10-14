#include "origo/events/KeyEvent.h"
#include "origo/events/EventTypes.h"

#pragma region KEY_MAP
static Origo::KeyboardKey glfwKeyToKeyboardKey(int glfwKey) {
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
#pragma endregion

namespace Origo {

#pragma region KEY_PRESS_EVENT

KeyPressEvent::KeyPressEvent(int glfwKey, KeyPressType keyPressType)
    : m_KeyPressed(glfwKeyToKeyboardKey(glfwKey))
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
