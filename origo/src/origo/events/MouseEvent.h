#pragma once

#include "origo/events/Event.h"
#include "origo/events/EventTypes.h"

namespace Origo {
class MouseMoveEvent : public Event {
public:
	MouseMoveEvent(const Vec2& coordinate);
	EventType GetEventType() const override;
	Vec2 GetCoordinate() const;
	static EventType GetStaticType();

private:
	Vec2 m_Coordinate;
};

class MouseScrollEvent : public Event {
public:
	MouseScrollEvent(bool scrolledUp);
	EventType GetEventType() const override;
	bool IsScrollUp() const;
	static EventType GetStaticType();

private:
	bool m_IsScrollUp;
};

class MouseClickEvent : public Event {
public:
	MouseClickEvent(MouseClickEventType clickType);
	EventType GetEventType() const override;
	MouseClickEventType GetMouseClickEventType() const;
	static EventType GetStaticType();

private:
	MouseClickEventType m_Type;
};
}
