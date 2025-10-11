#include "origo/events/WindowEvent.h"
#include "origo/events/EventTypes.h"

namespace Origo {

EventType WindowEvent::GetEventType() const {
	return EventType::Window;
}

#pragma region WINDOW_FOCUS_CHANGE

WindowFocusChangeEvent::WindowFocusChangeEvent(bool focusWon)
    : m_FocusWon(focusWon) {
}
WindowEventType WindowFocusChangeEvent::GetWindowEventType() const {
	return WindowEventType::WindowFocusChange;
}

bool WindowFocusChangeEvent::IsFocusWon() const {
	return m_FocusWon;
}

#pragma endregion

#pragma region WINDOW_RESIZE

WindowResizeEvent::WindowResizeEvent(glm::vec2 size)
    : m_Size(size) { };

WindowEventType WindowResizeEvent::GetWindowEventType() const {
	return WindowEventType::WindowResize;
}

glm::vec2 WindowResizeEvent::GetSize() const {
	return m_Size;
}
#pragma endregion

}
