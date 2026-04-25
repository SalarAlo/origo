#pragma once

#include "EventTypes.h"

namespace Origo {

class Event {
public:
	virtual EventType get_event_type() const = 0;
	void set_is_handled() { m_handled = true; }
	bool is_handled() const { return m_handled; }

private:
	bool m_handled {};
};

template <typename T>
concept EventTypeConcept = std::derived_from<T, Event>;

class EventDispatcher {
public:
	EventDispatcher(Event& event)
	    : m_event(event) { };

	template <EventTypeConcept TEvent>
	void dispatch(std::function<void(TEvent&)> fn) {
		if (TEvent::get_static_type() == m_event.get_event_type()) {
			fn(static_cast<TEvent&>(m_event));
		}
	}

private:
	Event& m_event;
};

using EventCallbackFn = std::function<void(Event&)>;

}
