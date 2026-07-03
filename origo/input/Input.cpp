#include "origo/input/Input.h"
#include <array>
#include <optional>

#include "GLFW/glfw3.h"
#include "origo/core/ScreenWindow.h"
#include "origo/events/KeyEvent.h"

namespace Origo::Input {
static ScreenWindow* s_context;
static std::array<bool, static_cast<std::size_t>(KeyboardKey::KEY_UNKNOWN) + 1> s_current_keys {};
static std::array<bool, static_cast<std::size_t>(KeyboardKey::KEY_UNKNOWN) + 1> s_previous_keys {};
static std::array<bool, 3> s_current_mouse {};
static std::array<bool, 3> s_previous_mouse {};
static Vec2 s_mouse_position { 0.0f };
static Vec2 s_previous_mouse_position { 0.0f };
static Vec2 s_mouse_delta { 0.0f };
static Vec2 s_scroll_delta { 0.0f };

namespace {
	std::size_t key_index(KeyboardKey key) {
		return static_cast<std::size_t>(key);
	}

	std::optional<std::size_t> mouse_index(MouseButton button) {
		switch (button) {
		case MouseButton::Left:
			return 0;
		case MouseButton::Right:
			return 1;
		case MouseButton::Middle:
			return 2;
		case MouseButton::Unknown:
			return std::nullopt;
		}
		return std::nullopt;
	}

	int mouse_button_to_glfw(MouseButton button) {
		switch (button) {
		case MouseButton::Left:
			return GLFW_MOUSE_BUTTON_LEFT;
		case MouseButton::Right:
			return GLFW_MOUSE_BUTTON_RIGHT;
		case MouseButton::Middle:
			return GLFW_MOUSE_BUTTON_MIDDLE;
		case MouseButton::Unknown:
			return -1;
		}
		return -1;
	}
}

void update_frame_state() {
	s_previous_keys = s_current_keys;
	s_previous_mouse = s_current_mouse;
	s_previous_mouse_position = s_mouse_position;
	s_scroll_delta = Vec2 { 0.0f };

	if (!s_context) {
		s_current_keys.fill(false);
		s_current_mouse.fill(false);
		s_mouse_delta = Vec2 { 0.0f };
		return;
	}

	for (std::size_t i = 0; i < s_current_keys.size(); ++i) {
		const auto key = static_cast<KeyboardKey>(i);
		if (key == KeyboardKey::KEY_UNKNOWN) {
			s_current_keys[i] = false;
			continue;
		}
		s_current_keys[i] = glfwGetKey(s_context->get_native_window(), keyboard_key_to_glfw_key(key)) == GLFW_PRESS;
	}

	for (MouseButton button : { MouseButton::Left, MouseButton::Right, MouseButton::Middle }) {
		const auto index = mouse_index(button);
		s_current_mouse[*index] = glfwGetMouseButton(s_context->get_native_window(), mouse_button_to_glfw(button)) == GLFW_PRESS;
	}

	double x = 0.0;
	double y = 0.0;
	glfwGetCursorPos(s_context->get_native_window(), &x, &y);
	s_mouse_position = Vec2 { static_cast<float>(x), static_cast<float>(y) };
	s_mouse_delta = s_mouse_position - s_previous_mouse_position;
}

bool is_key_pressed(KeyboardKey k) {
	if (!s_context || k == KeyboardKey::KEY_UNKNOWN)
		return false;
	return glfwGetKey(s_context->get_native_window(), keyboard_key_to_glfw_key(k)) == GLFW_PRESS;
}

bool is_key_down(KeyboardKey k) {
	if (k == KeyboardKey::KEY_UNKNOWN)
		return false;
	return s_current_keys[key_index(k)];
}

bool was_key_pressed(KeyboardKey k) {
	if (k == KeyboardKey::KEY_UNKNOWN)
		return false;
	return s_current_keys[key_index(k)] && !s_previous_keys[key_index(k)];
}

bool was_key_released(KeyboardKey k) {
	if (k == KeyboardKey::KEY_UNKNOWN)
		return false;
	return !s_current_keys[key_index(k)] && s_previous_keys[key_index(k)];
}

bool is_mouse_down(MouseButton button) {
	const auto index = mouse_index(button);
	return index ? s_current_mouse[*index] : false;
}

bool was_mouse_pressed(MouseButton button) {
	const auto index = mouse_index(button);
	return index ? s_current_mouse[*index] && !s_previous_mouse[*index] : false;
}

bool was_mouse_released(MouseButton button) {
	const auto index = mouse_index(button);
	return index ? !s_current_mouse[*index] && s_previous_mouse[*index] : false;
}

Vec2 mouse_position() {
	return s_mouse_position;
}

Vec2 mouse_delta() {
	return s_mouse_delta;
}

Vec2 scroll_delta() {
	return s_scroll_delta;
}

void record_scroll_delta(const Vec2& delta) {
	s_scroll_delta += delta;
}

bool has_context() {
	return s_context != nullptr;
}

void set_context(ScreenWindow* ctx) {
	s_context = ctx;
}

void set_cursor_mode(CursorMode mode) {
	if (!s_context)
		return;
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
