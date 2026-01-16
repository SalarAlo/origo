#include "NameComponentSerializer.h"
#include "origo/components/Name.h"

namespace Origo {

void NameComponentSerializer::Serialize(Component* comp, ISerializer& s) const {
	auto name = static_cast<NameComponent*>(comp);
	s.Write("name", name->GetName());
}

void NameComponentSerializer::Deserialize(Component* comp, ISerializer& s) {
	auto name = static_cast<NameComponent*>(comp);

	std::string value = name->GetName();
	s.TryRead("name", value);
	name->SetName(value);
}

}
