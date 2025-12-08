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
	std::string albedoUuidStr {};
	std::string shaderUuidStr {};

	backend.TryRead("albedo", albedoUuidStr);
	backend.TryRead("shader", shaderUuidStr);

	auto albedoUuid { UUID::FromString(albedoUuidStr) };
	auto shaderUuid { UUID::FromString(shaderUuidStr) };

	return MakeScope<Material>(albedoUuid, shaderUuid);
}

}
