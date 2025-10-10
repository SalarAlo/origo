#pragma once

#include "EventTypes.h"
#include <functional>

namespace Origo {

class Event {
public:
	virtual EventType GetEventType() = 0;

private:
};

using EventCallbackFn = std::function<void(const Event&)>;

}
