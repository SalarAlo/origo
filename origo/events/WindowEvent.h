#pragma once

#include "origo/events/Event.h"
#include "origo/events/EventTypes.h"

namespace Origo {

class WindowFocusChangeEvent : public Event {
public:
	WindowFocusChangeEvent(bool focusWon);
	EventType get_event_type() const override;
	bool is_focus_won() const;
	static EventType get_static_type();

private:
	bool m_focus_won;
};

class WindowResizeEvent : public Event {
public:
	WindowResizeEvent(Vec2 size);
	static EventType get_static_type();
	EventType get_event_type() const override;
	Vec2 get_size() const;

private:
	Vec2 m_size {};
};

}
