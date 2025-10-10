#pragma once

#include "Event.h"
#include "origo/events/EventTypes.h"

namespace Origo {

class WindowEvent : public Event {
public:
	EventType GetEventType() const;
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

}
