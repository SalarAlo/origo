#pragma once

#include "origo/events/Event.h"
#include "origo/events/EventTypes.h"

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
	WindowResizeEvent(Vec2 size);
	static EventType GetStaticType();
	EventType GetEventType() const override;
	Vec2 GetSize() const;

private:
	Vec2 m_Size {};
};

}
