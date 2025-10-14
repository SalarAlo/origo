#pragma once

#include "origo/events/Event.h"
#include "origo/events/EventTypes.h"

#include "glm/glm.hpp"

namespace Origo {

class WindowFocusChangeEvent : public Event {
public:
	WindowFocusChangeEvent(bool focusWon);
	EventType GetEventType() const override;
	bool IsFocusWon() const;
	static EventType GetStaticType();

private:
	bool m_FocusWon;
};

class WindowResizeEvent : public Event {
public:
	WindowResizeEvent(glm::vec2 size);
	static EventType GetStaticType();
	EventType GetEventType() const override;
	glm::vec2 GetSize() const;

private:
	glm::vec2 m_Size {};
};

}
