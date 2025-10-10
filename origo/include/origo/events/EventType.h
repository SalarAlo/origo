#pragma once

namespace Origo {

enum class EventType {
	Window,
	Mouse,
	Keyboard
};

enum class WindowEventType {
	WindowLoseFocus,
	WindowGainFocus,
	WindowResize
};

enum class KeyboardEventType {
	KeyPressStart,
	KeyPressEnd,
};

enum class MouseEventType {
	MouseMove,
	MouseLeftClick,
	MouseRightClick,
	MouseScroll,
};

}
