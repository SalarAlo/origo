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

enum class KeyEventType {
	KeyPress,

};

enum class KeyPressType {
	KeyPressStart,
	KeyPressRepeat,
	KeyPressStop,
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
