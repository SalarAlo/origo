#pragma once

#include "EventType.h"

namespace Origo {

class Event {
public:
	virtual EventType GetType() = 0;

private:
};

}
