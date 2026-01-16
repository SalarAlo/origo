#pragma once

#include "origo/components/Component.h"
#include "origo/serialization/ISerializer.h"

namespace Origo {

class IComponentSerializer {
public:
	virtual void Serialize(Component*, ISerializer&) const = 0;
	virtual void Deserialize(Component*, ISerializer&) = 0;
};

}
