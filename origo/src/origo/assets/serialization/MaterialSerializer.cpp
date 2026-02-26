#include "origo/assets/serialization/MaterialSerializer.h"

#include "origo/assets/AssetManager.h"
#include "origo/assets/Material2D.h"

#include "origo/serialization/ISerializer.h"

namespace Origo {

void MaterialSerializer::serialize(const Asset* asset, ISerializer& backend) const {
	auto material {
		static_cast<const Material2D*>(asset)
	};

	auto& am { AssetManager::get_instance() };

	if (auto albedo { material->get_albedo() }; albedo.has_value())
		if (auto albedo_uuid { am.get_uuid(*albedo) }; albedo_uuid.has_value())
			backend.write("albedo", (*albedo_uuid).to_string());

	if (auto shader { material->get_shader() }; shader.has_value())
		if (auto shader_uuid { am.get_uuid(*shader) }; shader_uuid.has_value())
			backend.write("shader", (*shader_uuid).to_string());

	ORG_INFO("Serializing an asset of type material");
}

void MaterialSerializer::deserialize(ISerializer& backend, Asset& asset) const {
	auto& material { static_cast<Material2D&>(asset) };

	if (std::string albedo_uuid_str {}; backend.try_read("albedo", albedo_uuid_str)) {
		auto albedo_uuid { UUID::from_string(albedo_uuid_str) };
		material.set_albedo_uuid(albedo_uuid);
	}

	if (std::string shader_uuid_str {}; backend.try_read("shader", shader_uuid_str)) {
		auto shader_uuid { UUID::from_string(shader_uuid_str) };
		material.set_shader_uuid(shader_uuid);
	}
}

}
