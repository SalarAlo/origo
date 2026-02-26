#include <filesystem>

#include "origo/assets/AssetDatabase.h"

#include "magic_enum/magic_enum.hpp"

#include "origo/assets/AssetFactory.h"
#include "origo/assets/AssetManager.h"

#include "origo/assets/serialization/AssetSerializer.h"

#include "origo/core/Logger.h"

#include "origo/serialization/JsonSerializer.h"

namespace Origo {

void AssetDatabase::register_metadata(const AssetMetadata& meta) {
	m_s_metadata[*meta.ID] = meta;
}

void AssetDatabase::write_import_file(const UUID& id) {
	auto meta_it = m_s_metadata.find(id);
	if (meta_it == m_s_metadata.end()) {
		ORG_ERROR("AssetDatabase::writeImportFile: No metadata for UUID {}", id.to_string());
		return;
	}

	const AssetMetadata& meta = meta_it->second;

	auto& am = AssetManager::get_instance();
	OptionalAssetHandle handle = am.get_handle_by_uuid(id);

	if (!handle.has_value()) {
		ORG_WARN("Skipping save: asset {} not loaded", id.to_string());
		return;
	}

	Asset* asset = am.get(*handle);
	auto path = get_import_path(meta);
	JsonSerializer serializer { path.string() };

	serializer.begin_object("header");
	serializer.write("uuid", (*meta.ID).to_string());
	serializer.write("name", meta.Name);
	serializer.write("type", magic_enum::enum_name(meta.Type));
	serializer.write("origin", magic_enum::enum_name(meta.Origin));
	serializer.write("source_path", meta.SourcePath.string());
	serializer.write("source_time_stamp", std::to_string(meta.SourceTimestamp.time_since_epoch().count()));
	serializer.write("imported_time_stamp", std::to_string(meta.ImportedTimestamp.time_since_epoch().count()));
	serializer.end_object();

	serializer.begin_object("payload");
	auto asset_serializer = AssetSerializationSystem::get(meta.Type);
	asset_serializer->serialize(asset, serializer);
	serializer.end_object();

	serializer.save_to_file();
}

AssetMetadata AssetDatabase::load_import_header(const std::filesystem::path& path) {
	if (!std::filesystem::exists(path)) {
		ORG_ERROR("Import file '{}' does not exist", path.string());
		return {};
	}

	JsonSerializer backend { path.string() };
	backend.load_file();

	AssetMetadata meta;

	backend.begin_object("header");

	std::string uuid_str;
	backend.try_read("uuid", uuid_str);
	meta.ID = UUID::from_string(uuid_str);

	backend.try_read("name", meta.Name);

	std::string type_str;
	backend.try_read("type", type_str);
	if (auto t = magic_enum::enum_cast<AssetType>(type_str))
		meta.Type = *t;

	std::string origin_str;
	backend.try_read("origin", origin_str);
	if (auto o = magic_enum::enum_cast<AssetOrigin>(origin_str))
		meta.Origin = *o;

	std::string source;
	if (backend.try_read("source_path", source))
		meta.SourceTimestamp = std::filesystem::last_write_time(source);
	meta.SourcePath = source;

	std::string imported_stamp;
	backend.try_read("imported_time_stamp", imported_stamp);
	meta.ImportedTimestamp = std::filesystem::file_time_type(std::filesystem::file_time_type::duration(std::stoll(imported_stamp)));

	backend.begin_array("dependencies");
	std::string dep;
	while (backend.try_read_array_element(dep)) {
		meta.Dependencies.push_back(UUID::from_string(dep));
	}
	backend.end_array();

	backend.end_object();

	return meta;
}

Asset* AssetDatabase::load_asset(const UUID& id) {
	auto& am { AssetManager::get_instance() };

	auto meta_it = m_s_metadata.find(id);
	if (meta_it == m_s_metadata.end()) {
		ORG_ERROR("AssetDatabase::LoadAsset: No metadata for uuid {}", id.to_string());
		return nullptr;
	}

	const AssetMetadata& meta = meta_it->second;
	auto path = get_import_path(meta);

	JsonSerializer backend { path.string() };
	backend.load_file();

	backend.begin_object("header");
	backend.end_object();

	backend.begin_object("payload");

	auto serializer = AssetSerializationSystem::get(meta.Type);
	auto asset { AssetFactory::get_instance().allocate_hollow_asset(meta.Type) };
	serializer->deserialize(backend, *asset);

	backend.end_object();

	auto handle = am.register_asset(std::move(asset), meta.ID);

	return am.get(handle);
}

void AssetDatabase::save_all() {
	auto& am = AssetManager::get_instance();

	for (const auto& [uuid, handle] : am.get_uuid_map()) {
		if (am.is_valid(handle))
			write_import_file(uuid);
	}
}

std::filesystem::path AssetDatabase::get_import_path(const AssetMetadata& meta) {
	if (!meta.SourcePath.empty()) {
		return meta.SourcePath.string() + ".import";
	}

	return ROOT / "generated" / ((*meta.ID).to_string() + ".import");
}

const AssetMetadata& AssetDatabase::get_metadata(const UUID& id) { return m_s_metadata[id]; }

}
