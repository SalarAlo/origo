#include "origo/input/Input.h"
#include "GLFW/glfw3.h"
#include "origo/core/ScreenWindow.h"
#include "origo/events/KeyEvent.h"

namespace Origo::Input {
static ScreenWindow* S_CONTEXT;

bool IsKeyPressed(KeyboardKey k) {
	return glfwGetKey(S_CONTEXT->GetNativeWindow(), KeyboardKeyToGlfwKey(k));
}

void SetContext(ScreenWindow* ctx) {
	S_CONTEXT = ctx;
}

void SetCursorMode(CursorMode mode) {
	switch (mode) {
	case CursorMode::Locked:
		glfwSetInputMode(S_CONTEXT->GetNativeWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		break;

	case CursorMode::Free:
		glfwSetInputMode(S_CONTEXT->GetNativeWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		break;
	}
}
}
