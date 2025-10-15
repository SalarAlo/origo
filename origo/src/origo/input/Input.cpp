#include "origo/input/Input.h"
#include "GLFW/glfw3.h"
#include "origo/core/ScreenWindow.h"
#include "origo/events/KeyEvent.h"

namespace Origo::Input {
static Ref<ScreenWindow> S_CONTEXT;

bool IsKeyPressed(KeyboardKey k) {
	return glfwGetKey(S_CONTEXT->GetNativeWindow(), KeyboardKeyToGlfwKey(k));
}

void SetContext(Ref<ScreenWindow> ctx) {
	S_CONTEXT = ctx;
}
}
