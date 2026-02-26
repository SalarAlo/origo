#pragma once

#include "origo/events/Event.h"
#include "origo/events/EventTypes.h"

namespace Origo {
class MouseMoveEvent : public Event {
public:
	MouseMoveEvent(const Vec2& coordinate);
	EventType get_event_type() const override;
	Vec2 get_coordinate() const;
	static EventType get_static_type();

private:
	Vec2 m_coordinate;
};

class MouseScrollEvent : public Event {
public:
	MouseScrollEvent(bool scrolledUp);
	EventType get_event_type() const override;
	bool is_scroll_up() const;
	static EventType get_static_type();

private:
	bool m_is_scroll_up;
};

class MouseClickEvent : public Event {
public:
	MouseClickEvent(MouseClickEventType clickType);
	EventType get_event_type() const override;
	MouseClickEventType get_mouse_click_event_type() const;
	static EventType get_static_type();

private:
	MouseClickEventType m_type;
};
}
