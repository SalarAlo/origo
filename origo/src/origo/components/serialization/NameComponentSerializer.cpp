#include "NameComponentSerializer.h"

#include "origo/components/Name.h"

namespace Origo {

void NameComponentSerializer::serialize(Component* comp, ISerializer& s) const {
	auto name = static_cast<NameComponent*>(comp);
	s.write("name", name->Name);
}

void NameComponentSerializer::deserialize(Component* comp, ISerializer& s) {
	auto name = static_cast<NameComponent*>(comp);

	s.try_read("name", name->Name);
}

}
