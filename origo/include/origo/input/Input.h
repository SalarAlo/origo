#pragma once

#include "origo/core/ScreenWindow.h"
#include "origo/events/KeyEvent.h"

namespace Origo::Input {
enum class CursorMode {
	Locked,
	Free
};

bool IsKeyPressed(KeyboardKey k);
void SetCursorMode(CursorMode mode);
void SetContext(Ref<ScreenWindow> win);
}
