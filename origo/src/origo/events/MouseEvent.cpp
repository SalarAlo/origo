#include <origo/events/MouseEvent.h>

#include "origo/events/EventTypes.h"

namespace Origo {

#pragma region MOUSE_MOVE_EVENT

EventType MouseMoveEvent::get_event_type() const {
	return get_static_type();
}

MouseMoveEvent::MouseMoveEvent(const Vec2& coordinate)
    : m_coordinate(coordinate) {
}
Vec2 MouseMoveEvent::get_coordinate() const {
	return m_coordinate;
}

EventType MouseMoveEvent::get_static_type() {
	return EventType::MouseMove;
}

#pragma endregion

#pragma region MOUSE_SCROLL_EVENT

MouseScrollEvent::MouseScrollEvent(bool scrollUp)
    : m_is_scroll_up(scrollUp) {
}

bool MouseScrollEvent::is_scroll_up() const {
	return m_is_scroll_up;
}
EventType MouseScrollEvent::get_event_type() const {
	return get_static_type();
}

EventType MouseScrollEvent::get_static_type() {
	return EventType::MouseMove;
}

#pragma endregion

#pragma region MOUSE_CLICK_EVENT
MouseClickEvent::MouseClickEvent(MouseClickEventType type)
    : m_type(type) {
}

MouseClickEventType MouseClickEvent::get_mouse_click_event_type() const {
	return m_type;
}

EventType MouseClickEvent::get_event_type() const {
	return get_static_type();
}

EventType MouseClickEvent::get_static_type() {
	return EventType::MouseClick;
}

#pragma endregion

};
