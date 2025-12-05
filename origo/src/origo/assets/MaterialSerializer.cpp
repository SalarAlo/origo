#include "origo/assets/MaterialSerializer.h"
#include "origo/assets/Material.h"
#include "origo/serialization/ISerializer.h"
#include <stdexcept>

// TODO: Implement Serialization

namespace Origo {

void MaterialSerializer::Serialize(const Asset* asset, ISerializer& backend) const {
	auto material {
		static_cast<const Material*>(asset)
	};

	backend.Write("albedo", material->GetAlbedo().GetId());
	backend.Write("shader", material->GetShader().GetId());
	ORG_INFO("Serializing an asset of type material");
}

Scope<Asset> MaterialSerializer::Deserialize(ISerializer& backend) const {
	throw std::logic_error("Not implemented");
}

}
