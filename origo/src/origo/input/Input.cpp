#include "origo/input/Input.h"
#include "GLFW/glfw3.h"
#include "origo/core/ScreenWindow.h"
#include "origo/events/KeyEvent.h"

namespace Origo::Input {
static ScreenWindow* s_context;

bool is_key_pressed(KeyboardKey k) {
	return glfwGetKey(s_context->get_native_window(), keyboard_key_to_glfw_key(k));
}

void set_context(ScreenWindow* ctx) {
	s_context = ctx;
}

void set_cursor_mode(CursorMode mode) {
	switch (mode) {
	case CursorMode::Locked:
		glfwSetInputMode(s_context->get_native_window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		break;

	case CursorMode::Free:
		glfwSetInputMode(s_context->get_native_window(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		break;
	}
}
}
