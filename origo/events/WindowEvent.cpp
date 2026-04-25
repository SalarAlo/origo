#include "origo/events/WindowEvent.h"

#include "origo/events/EventTypes.h"

namespace Origo {

#pragma region WINDOW_FOCUS_CHANGE

WindowFocusChangeEvent::WindowFocusChangeEvent(bool focusWon)
    : m_focus_won(focusWon) {
}
EventType WindowFocusChangeEvent::get_event_type() const {
	return get_static_type();
}

bool WindowFocusChangeEvent::is_focus_won() const {
	return m_focus_won;
}

EventType WindowFocusChangeEvent::get_static_type() {
	return EventType::WindowFocusChange;
}

#pragma endregion

#pragma region WINDOW_RESIZE

WindowResizeEvent::WindowResizeEvent(Vec2 size)
    : m_size(size) { };

EventType WindowResizeEvent::get_event_type() const {
	return get_static_type();
}

Vec2 WindowResizeEvent::get_size() const {
	return m_size;
}

EventType WindowResizeEvent::get_static_type() {
	return EventType::WindowResize;
}
#pragma endregion

}
