#include "origo/assets/MaterialSerializer.h"
#include "origo/assets/Material.h"
#include "origo/serialization/ISerializer.h"

// TODO: Implement Serialization

namespace Origo {

void MaterialSerializer::Serialize(const Asset* asset, ISerializer& backend) const {
	auto material {
		static_cast<const Material*>(asset)
	};

	backend.Write("albedo", material->GetAlbedo().ToString());
	backend.Write("shader", material->GetShader().ToString());
	ORG_INFO("Serializing an asset of type material");
}

Scope<Asset> MaterialSerializer::Deserialize(ISerializer& backend) const {
	std::string albedoUuid {};
	std::string shaderUuid {};

	backend.TryRead("albedo", albedoUuid);
	backend.Write("shader", shaderUuid);
}

}
