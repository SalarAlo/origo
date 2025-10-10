#pragma once

#include "origo/events/Event.h"
#include "origo/events/EventType.h"

namespace Origo {
class MouseEvent : public Event {
public:
	EventType GetEventType() override;
	virtual MouseEventType GetMouseEventType() = 0;
};

class MouseMoveEvent : public MouseEvent {
public:
	MouseEventType GetMouseEventType() override {
		return MouseEventType::MouseMove;
	}
};

}
