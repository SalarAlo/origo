#pragma once

namespace Origo {

enum class EventType {
	Window,
	Mouse,
	Keyboard
};

enum class WindowEventType {
	WindowFocusChange,
	WindowResize
};

enum class KeyboardEventType {
	KeyPressStart,
	KeyPressEnd,
};

enum class MouseEventType {
	MouseMove,
	MouseClick,
	MouseScroll,
};

enum class MouseClickEventType {
	Left,
	Middle,
	Right
};

}
