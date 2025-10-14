#pragma once

namespace Origo {

enum class EventType {
	WindowFocusChange,
	WindowResize,

	MouseMove,
	MouseClick,
	MouseScroll,

	KeyPress,
};

enum class KeyPressType {
	KeyPressStart,
	KeyPressRepeat,
	KeyPressStop,
};

enum class MouseClickEventType {
	Left,
	Middle,
	Right
};

}
