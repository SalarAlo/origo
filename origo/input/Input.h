#pragma once

#include "origo/core/ScreenWindow.h"
#include "origo/events/KeyEvent.h"

namespace Origo::Input {
enum class CursorMode {
	Locked,
	Free
};

enum class MouseButton {
	Left,
	Right,
	Middle,
	Unknown
};

void update_frame_state();
bool is_key_pressed(KeyboardKey k);
bool is_key_down(KeyboardKey k);
bool was_key_pressed(KeyboardKey k);
bool was_key_released(KeyboardKey k);
bool is_mouse_down(MouseButton button);
bool was_mouse_pressed(MouseButton button);
bool was_mouse_released(MouseButton button);
Vec2 mouse_position();
Vec2 mouse_delta();
Vec2 scroll_delta();
void record_scroll_delta(const Vec2& delta);
bool has_context();
void set_cursor_mode(CursorMode mode);
void set_context(ScreenWindow* win);
}
