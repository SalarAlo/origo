#include "origo/events/EventTypes.h"
#include <origo/events/MouseEvent.h>

namespace Origo {

EventType MouseEvent::GetEventType() const {
	return EventType::Mouse;
}

#pragma region MOUSE_MOVE_EVENT

MouseEventType MouseMoveEvent::GetMouseEventType() const {
	return MouseEventType::MouseMove;
}

MouseMoveEvent::MouseMoveEvent(const glm::vec2& coordinate)
    : m_Coordinate(coordinate) {
}
glm::vec2 MouseMoveEvent::GetCoordinate() const {
	return m_Coordinate;
}

#pragma endregion

#pragma region MOUSE_SCROLL_EVENT

MouseScrollEvent::MouseScrollEvent(bool scrollUp)
    : m_IsScrollUp(scrollUp) {
}

bool MouseScrollEvent::IsScrollUp() const {
	return m_IsScrollUp;
}
MouseEventType MouseScrollEvent::GetMouseEventType() const {
	return MouseEventType::MouseScroll;
}

#pragma endregion

#pragma region MOUSE_CLICK_EVENT
MouseClickEvent::MouseClickEvent(MouseClickEventType type)
    : m_Type(type) {
}

MouseClickEventType MouseClickEvent::GetMouseClickEventType() const {
	return m_Type;
}
MouseEventType MouseClickEvent::GetMouseEventType() const {
	return MouseEventType::MouseClick;
}

#pragma endregion

};
