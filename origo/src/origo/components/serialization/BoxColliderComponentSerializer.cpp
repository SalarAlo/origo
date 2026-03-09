#include "origo/components/serialization/BoxColliderComponentSerializer.h"

#include "origo/components/BoxColliderComponent.h"

namespace Origo {

void BoxColliderComponentSerializer::serialize(Component* comp, ISerializer& s) const {
	// intentionally left empty
}

void BoxColliderComponentSerializer::deserialize(Component* comp, ISerializer& s) {
	// intentionally left empty
}

}
