#include "origo/assets/MaterialSerializer.h"
#include "origo/assets/AssetManagerFast.h"
#include "origo/assets/Material.h"
#include "origo/serialization/ISerializer.h"

namespace Origo {

void MaterialSerializer::Serialize(const Asset* asset, ISerializer& backend) const {
	auto material {
		static_cast<const Material2D*>(asset)
	};

	auto& am { AssetManagerFast::GetInstance() };
	backend.Write("albedo", am.GetUUID(material->GetAlbedo()).ToString());
	backend.Write("shader", am.GetUUID(material->GetShader()).ToString());

	ORG_INFO("Serializing an asset of type material");
}

void MaterialSerializer::Deserialize(ISerializer& backend, Asset& asset) const {
	auto& material { static_cast<Material2D&>(asset) };

	std::string albedoUuidStr {};
	std::string shaderUuidStr {};

	backend.TryRead("albedo", albedoUuidStr);
	backend.TryRead("shader", shaderUuidStr);

	auto albedoUuid { UUID::FromString(albedoUuidStr) };
	auto shaderUuid { UUID::FromString(shaderUuidStr) };
}

}
