#include "origo/components/serialization/BoxColliderComponentSerializer.h"

#include "origo/components/BoxColliderComponent.h"

namespace Origo {

void BoxColliderComponentSerializer::serialize(Component* comp, ISerializer& s) const {
	auto* box = static_cast<BoxColliderComponent*>(comp);

	s.write("offset", box->Offset);
	s.write("size", box->Size);
}

void BoxColliderComponentSerializer::deserialize(Component* comp, ISerializer& s) {
	auto* box = static_cast<BoxColliderComponent*>(comp);

	s.try_read("offset", box->Offset);
	s.try_read("size", box->Size);
}

}
