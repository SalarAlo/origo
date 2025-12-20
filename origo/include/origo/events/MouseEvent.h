#pragma once

#include "origo/events/Event.h"
#include "origo/events/EventTypes.h"

namespace Origo {
class MouseMoveEvent : public Event {
public:
	MouseMoveEvent(const glm::vec2& coordinate);
	EventType GetEventType() const override;
	glm::vec2 GetCoordinate() const;
	static EventType GetStaticType();

private:
	glm::vec2 m_Coordinate;
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
