#pragma once

#include "EventTypes.h"
#include <concepts>
#include <functional>

namespace Origo {

class Event {
public:
	virtual EventType GetEventType() const = 0;

private:
};
template <typename T>
concept EventTypeConcept = std::derived_from<T, Event>;

class EventDispatcher {
public:
	EventDispatcher(Event& event)
	    : m_Event(event) { };

	template <EventTypeConcept TEvent>
	void Dispatch(std::function<void(TEvent&)> fn) {
		if (TEvent::GetStaticType() == m_Event.GetEventType()) {
			fn(static_cast<TEvent&>(m_Event));
		}
	}

private:
	Event& m_Event;
};

using EventCallbackFn = std::function<void(Event&)>;

}
