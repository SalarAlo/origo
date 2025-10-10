#pragma once

#include "origo/events/Event.h"
#include "origo/events/EventTypes.h"

namespace Origo {
class MouseEvent : public Event {
public:
	EventType GetEventType() override;
	virtual MouseEventType GetMouseEventType() const = 0;
};

class MouseMoveEvent : public MouseEvent {
public:
	MouseMoveEvent(int x, int y);
	MouseEventType GetMouseEventType() const override;
	int GetX() const;
	int GetY() const;

private:
	int m_X {};
	int m_Y {};
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
