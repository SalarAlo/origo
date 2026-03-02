#include "origo/assets/Material2DSource.h"

#include "magic_enum/magic_enum.hpp"

#include "origo/assets/AssetManager.h"
#include "origo/assets/DefaultAssetCache.h"

#include "origo/core/Logger.h"

#include "origo/serialization/JsonSerializer.h"

namespace Origo {
void Material2DSource::serialize(ISerializer& backend) const {
	backend.write("type", magic_enum::enum_name(get_source_type()));

	backend.begin_object("data");
	serialize_body(backend);
	backend.end_object();
}

MaterialData Material2DSourceID::get_material_data() const {
	auto shader_handle { AssetManager::get_instance().get_handle_by_uuid(m_shader) };
	auto albedo_handle { AssetManager::get_instance().get_handle_by_uuid(m_albedo) };
	if (!shader_handle) {
		shader_handle = DefaultAssetCache::get_instance().get_shader();
	}

	if (!albedo_handle) {
		albedo_handle = DefaultAssetCache::get_instance().get_texture();
	}

	return MaterialData { .AlbedoHandle = *albedo_handle, .ShaderHandle = *shader_handle };
}

MaterialData Material2DSourceFile::get_material_data() const {
	JsonSerializer backend { m_path };
	backend.load_file();

	std::string albedo_id_str;
	UUID albedo_id = UUID::bad();
	if (backend.try_read("albedo", albedo_id_str))
		albedo_id = UUID::from_string(albedo_id_str);
	else
		albedo_id = *AssetManager::get_instance().get_uuid(DefaultAssetCache::get_instance().get_texture());

	std::string shader_id_str;
	UUID shader_id = UUID::bad();
	if (backend.try_read("shader", shader_id_str))
		shader_id = UUID::from_string(shader_id_str);
	else
		shader_id = *AssetManager::get_instance().get_uuid(DefaultAssetCache::get_instance().get_shader());

	return Material2DSourceID { albedo_id, shader_id }.get_material_data();
}

Scope<Material2DSource> Material2DSource::deserialize(ISerializer& backend) {
	std::string type_str {};

	backend.try_read("type", type_str);
	auto type { magic_enum::enum_cast<MaterialSourceType>(type_str) };

	if (!type) {
		ORG_CORE_ERROR("Material Source was not properly serialized. Type can not be read!");
		return nullptr;
	}

	backend.begin_object("data");
	switch (*type) {
	case MaterialSourceType::File: {
		std::string path;
		backend.try_read("path", path);
		backend.end_object();
		return MakeScope<Material2DSourceFile>(path);
	}
	case MaterialSourceType::Raw:
		[[fallthrough]];
	case MaterialSourceType::ID:
		std::string albedo_id_str;
		std::string shader_id_str;
		backend.try_read("albedo", albedo_id_str);
		backend.try_read("shader", shader_id_str);
		backend.end_object();

		return MakeScope<Material2DSourceID>(
		    UUID::from_string(albedo_id_str),
		    UUID::from_string(shader_id_str));
	}

	backend.end_object();

	return nullptr;
}

}
