#include "origo/events/WindowEvent.h"

namespace Origo {

EventType WindowEvent::GetEventType() const {
	return EventType::Window;
}

}
