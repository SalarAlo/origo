#include "origo/assets/MaterialSerializer.h"
#include "origo/assets/Material.h"
#include "origo/serialization/ISerializer.h"

// TODO: Implement Serialization

namespace Origo {

void MaterialSerializer::Serialize(const Asset* asset, ISerializer& backend) const {
	auto material {
		static_cast<const Material*>(asset)
	};

	backend.Write("albedo", material->GetAlbedo().GetId());
	backend.Write("shader", material->GetShader().GetId());
}

Asset* MaterialSerializer::Deserialize(ISerializer& backend) const {
	return nullptr;
}

REGISTER_SERIALIZER(Material)
}
