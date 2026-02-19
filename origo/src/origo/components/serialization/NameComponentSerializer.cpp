#include "NameComponentSerializer.h"

#include "origo/components/Name.h"

namespace Origo {

void NameComponentSerializer::Serialize(Component* comp, ISerializer& s) const {
	auto name = static_cast<NameComponent*>(comp);
	s.Write("name", name->Name);
}

void NameComponentSerializer::Deserialize(Component* comp, ISerializer& s) {
	auto name = static_cast<NameComponent*>(comp);

	s.TryRead("name", name->Name);
}

}
