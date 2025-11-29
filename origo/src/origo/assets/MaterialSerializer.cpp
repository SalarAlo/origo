#include "origo/assets/MaterialSerializer.h"
#include "origo/assets/Material.h"
#include "origo/serialization/ISerializer.h"

// TODO: Implement Serialization

namespace Origo {
bool MaterialSerializer::ShouldSerialize(const Asset* asset) const {
	auto mat { dynamic_cast<const Material*>(asset) };
	return true;
}

void MaterialSerializer::Serialize(const Asset* asset, ISerializer& backend) const {
}

Asset* MaterialSerializer::Deserialize(ISerializer& backend) const {
	return nullptr;
}

REGISTER_SERIALIZER(Material)
}
