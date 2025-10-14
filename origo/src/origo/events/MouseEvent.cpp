#include "origo/events/EventTypes.h"
#include <origo/events/MouseEvent.h>

namespace Origo {

#pragma region MOUSE_MOVE_EVENT

EventType MouseMoveEvent::GetEventType() const {
	return GetStaticType();
}

MouseMoveEvent::MouseMoveEvent(const glm::vec2& coordinate)
    : m_Coordinate(coordinate) {
}
glm::vec2 MouseMoveEvent::GetCoordinate() const {
	return m_Coordinate;
}

EventType MouseMoveEvent::GetStaticType() {
	return EventType::MouseMove;
}

#pragma endregion

#pragma region MOUSE_SCROLL_EVENT

MouseScrollEvent::MouseScrollEvent(bool scrollUp)
    : m_IsScrollUp(scrollUp) {
}

bool MouseScrollEvent::IsScrollUp() const {
	return m_IsScrollUp;
}
EventType MouseScrollEvent::GetEventType() const {
	return GetStaticType();
}

EventType MouseScrollEvent::GetStaticType() {
	return EventType::MouseMove;
}

#pragma endregion

#pragma region MOUSE_CLICK_EVENT
MouseClickEvent::MouseClickEvent(MouseClickEventType type)
    : m_Type(type) {
}

MouseClickEventType MouseClickEvent::GetMouseClickEventType() const {
	return m_Type;
}

EventType MouseClickEvent::GetEventType() const {
	return GetStaticType();
}

EventType MouseClickEvent::GetStaticType() {
	return EventType::MouseClick;
}

#pragma endregion

};
