#pragma once

namespace Origo {

enum class EventType {
	WindowClose,
	WindowResize,
	WindowFocus,
	WindowLostFocus,
	WindowMoved,

	KeyPressed,
	KeyReleased,

	MouseButtonPress,
	MouseButtonRelease,
	MouseMove,
	MouseScroll
};

}
