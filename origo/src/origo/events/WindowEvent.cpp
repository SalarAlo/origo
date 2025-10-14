#include "origo/events/WindowEvent.h"
#include "origo/events/EventTypes.h"

namespace Origo {

#pragma region WINDOW_FOCUS_CHANGE

WindowFocusChangeEvent::WindowFocusChangeEvent(bool focusWon)
    : m_FocusWon(focusWon) {
}
EventType WindowFocusChangeEvent::GetEventType() const {
	return GetStaticType();
}

bool WindowFocusChangeEvent::IsFocusWon() const {
	return m_FocusWon;
}

EventType WindowFocusChangeEvent::GetStaticType() {
	return EventType::WindowFocusChange;
}

#pragma endregion

#pragma region WINDOW_RESIZE

WindowResizeEvent::WindowResizeEvent(glm::vec2 size)
    : m_Size(size) { };

EventType WindowResizeEvent::GetEventType() const {
	return GetStaticType();
}

glm::vec2 WindowResizeEvent::GetSize() const {
	return m_Size;
}

EventType WindowResizeEvent::GetStaticType() {
	return EventType::WindowResize;
}
#pragma endregion

}
