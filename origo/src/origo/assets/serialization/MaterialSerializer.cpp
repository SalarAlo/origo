#include "origo/assets/serialization/MaterialSerializer.h"
#include "origo/assets/AssetManager.h"
#include "origo/assets/Material.h"
#include "origo/serialization/ISerializer.h"

namespace Origo {

void MaterialSerializer::Serialize(const Asset* asset, ISerializer& backend) const {
	auto material {
		static_cast<const Material2D*>(asset)
	};

	auto& am { AssetManager::GetInstance() };

	if (auto albedo { material->GetAlbedo() }; albedo.has_value())
		if (auto albedoUUID { am.GetUUID(*albedo) }; albedoUUID.has_value())
			backend.Write("albedo", (*albedoUUID).ToString());

	if (auto shader { material->GetShader() }; shader.has_value())
		if (auto shaderUUID { am.GetUUID(*shader) }; shaderUUID.has_value())
			backend.Write("shader", (*shaderUUID).ToString());

	ORG_INFO("Serializing an asset of type material");
}

void MaterialSerializer::Deserialize(ISerializer& backend, Asset& asset) const {
	auto& material { static_cast<Material2D&>(asset) };

	if (std::string albedoUuidStr {}; backend.TryRead("albedo", albedoUuidStr)) {
		auto albedoUuid { UUID::FromString(albedoUuidStr) };
		material.SetAlbedoUUID(albedoUuid);
	}

	if (std::string shaderUuidStr {}; backend.TryRead("shader", shaderUuidStr)) {
		auto shaderUuid { UUID::FromString(shaderUuidStr) };
		material.SetShaderUUID(shaderUuid);
	}
}

}
