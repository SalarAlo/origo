#pragma once

#include "origo/components/Component.h"
#include "origo/serialization/ISerializer.h"

namespace Origo {

class IComponentSerializer {
public:
	virtual void serialize(Component*, ISerializer&) const = 0;
	virtual void deserialize(Component*, ISerializer&) = 0;
};

}
