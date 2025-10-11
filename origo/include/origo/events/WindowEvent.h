#pragma once

#include "origo/events/Event.h"
#include "origo/events/EventTypes.h"

#include "glm/glm.hpp"

namespace Origo {

class WindowEvent : public Event {
public:
	EventType GetEventType() const override;
	virtual WindowEventType GetWindowEventType() const = 0;
};

class WindowFocusChangeEvent : public WindowEvent {
public:
	WindowFocusChangeEvent(bool focusWon);
	WindowEventType GetWindowEventType() const override;
	bool IsFocusWon() const;

private:
	bool m_FocusWon;
};

class WindowResizeEvent : public WindowEvent {
public:
	WindowResizeEvent(glm::vec2 size);
	WindowEventType GetWindowEventType() const override;
	glm::vec2 GetSize() const;

private:
	glm::vec2 m_Size {};
};

}
