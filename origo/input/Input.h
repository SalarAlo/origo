#pragma once

#include "origo/core/ScreenWindow.h"
#include "origo/events/KeyEvent.h"

namespace Origo::Input {
enum class CursorMode {
	Locked,
	Free
};

bool is_key_pressed(KeyboardKey k);
void set_cursor_mode(CursorMode mode);
void set_context(ScreenWindow* win);
}
