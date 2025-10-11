#pragma once

#include "origo/events/Event.h"
#include "origo/events/EventTypes.h"

#include "glm/glm.hpp"

namespace Origo {
class MouseEvent : public Event {
public:
	EventType GetEventType() const override;
	virtual MouseEventType GetMouseEventType() const = 0;
};

class MouseMoveEvent : public MouseEvent {
public:
	MouseMoveEvent(const glm::vec2& coordinate);
	MouseEventType GetMouseEventType() const override;
	glm::vec2 GetCoordinate() const;

private:
	glm::vec2 m_Coordinate;
};

class MouseScrollEvent : public MouseEvent {
public:
	MouseScrollEvent(bool scrolledUp);
	MouseEventType GetMouseEventType() const override;
	bool IsScrollUp() const;

private:
	bool m_IsScrollUp;
};

class MouseClickEvent : public MouseEvent {
public:
	MouseClickEvent(MouseClickEventType clickType);
	MouseEventType GetMouseEventType() const override;
	MouseClickEventType GetMouseClickEventType() const;

private:
	MouseClickEventType m_Type;
};
}
