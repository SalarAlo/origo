#include <filesystem>

#include "origo/assets/AssetDatabase.h"

#include "magic_enum/magic_enum.hpp"

#include "origo/assets/AssetFactory.h"
#include "origo/assets/AssetManager.h"
#include "origo/assets/Metadata.h"

#include "origo/assets/importers/AssetImporterRegistry.h"
#include "origo/assets/importers/AssetTruthLocation.h"

#include "origo/assets/serialization/AssetSerializer.h"

#include "origo/core/PathContext.h"

#include "origo/core/Logger.h"

#include "origo/serialization/JsonSerializer.h"

namespace Origo {

void AssetDatabase::register_metadata(const AssetMetadata& meta) {
	m_metadata[*meta.ID] = meta;
}

void AssetDatabase::write_import_file(const UUID& id) {
	auto meta_it = m_metadata.find(id);
	if (meta_it == m_metadata.end()) {
		ORG_ERROR("AssetDatabase::writeImportFile: No metadata for UUID {}", id.to_string());
		return;
	}

	const AssetMetadata& meta = meta_it->second;

	auto path = get_import_path(meta);
	JsonSerializer serializer { path.string() };

	serializer.begin_object("header");
	serializer.write("uuid", (*meta.ID).to_string());
	serializer.write("name", meta.Name);
	serializer.write("type", magic_enum::enum_name(meta.Type));
	serializer.write("origin", magic_enum::enum_name(meta.Origin));
	if (meta.ParentID)
		serializer.write("parent_id", meta.ParentID->to_string());

	if (meta.SourcePath)
		serializer.write("source_path", PathContextService::get_instance().serialize_path(*meta.SourcePath).string());

	serializer.write("source_time_stamp",
	    std::to_string(meta.SourceTimestamp.time_since_epoch().count()));

	serializer.write("imported_time_stamp",
	    std::to_string(meta.ImportedTimestamp.time_since_epoch().count()));

	serializer.begin_array("dependencies");
	for (const UUID& dependency : meta.Dependencies)
		serializer.write(dependency.to_string());
	serializer.end_array();
	serializer.end_object();

	// Payload only necessary in .import file if imported file
	if (meta.Origin == AssetOrigin::Imported) {
		auto& am = AssetManager::get_instance();
		OptionalAssetHandle handle = am.get_handle_by_uuid(id);

		if (!handle.has_value()) {
			ORG_WARN("Skipping payload save: asset {} not loaded", id.to_string());
		} else {
			Asset* asset = am.get(*handle);
			auto asset_serializer = AssetSerializationSystem::get(meta.Type);
			if (!asset_serializer) {
				ORG_ERROR("Skipping payload save: no serializer for asset type {}", magic_enum::enum_name(meta.Type));
				serializer.save_to_file();
				return;
			}

			serializer.begin_object("payload");
			asset_serializer->serialize(asset, serializer);
			serializer.end_object();
		}
	}

	serializer.save_to_file();
}

AssetMetadata AssetDatabase::load_import_header(const std::filesystem::path& path) {
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

	std::string parent_id_str;
	if (backend.try_read("parent_id", parent_id_str) && !parent_id_str.empty())
		meta.ParentID = UUID::from_string(parent_id_str);

	std::string source;
	if (backend.try_read("source_path", source) && !source.empty()) {
		const auto resolved_source = PathContextService::get_instance().resolve_serialized_path(source);
		meta.SourcePath = resolved_source;
	}

	std::string source_stamp;
	if (backend.try_read("source_time_stamp", source_stamp) && !source_stamp.empty())
		meta.SourceTimestamp = std::filesystem::file_time_type(std::filesystem::file_time_type::duration(std::stoll(source_stamp)));

	std::string imported_stamp;
	backend.try_read("imported_time_stamp", imported_stamp);
	meta.ImportedTimestamp = std::filesystem::file_time_type(std::filesystem::file_time_type::duration(std::stoll(imported_stamp)));

	backend.begin_array("dependencies");
	std::string dep;
	while (backend.try_read_array_object(dep))
		meta.Dependencies.push_back(UUID::from_string(dep));
	backend.end_array();

	backend.end_object();

	return meta;
}

Asset* AssetDatabase::load_asset(const UUID& id) {
	auto& am { AssetManager::get_instance() };

	auto meta_it = m_metadata.find(id);
	if (meta_it == m_metadata.end()) {
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
	if (!serializer) {
		ORG_ERROR("AssetDatabase::load_asset: no serializer for asset type {}", magic_enum::enum_name(meta.Type));
		return nullptr;
	}
	auto asset { AssetFactory::get_instance().allocate_hollow_asset(meta.Type) };
	serializer->deserialize(backend, *asset);

	backend.end_object();

	auto handle = am.register_asset(std::move(asset), meta.ID, meta.SourcePath);

	return am.get(handle);
}

void AssetDatabase::save_assets() {
	auto& am = AssetManager::get_instance();

	for (const auto& [uuid, handle] : am.get_uuid_map()) {
		const auto& md = m_metadata[uuid];

		if (!md.SourcePath)
			continue;

		if (md.Origin == AssetOrigin::Synthetic)
			continue;

		Asset* asset = am.get(handle);

		auto* importer = AssetImporterRegistry::get_instance().get_importer(*md.SourcePath);

		if (!importer)
			continue;

		auto truth = importer->get_truth_location();

			if (truth == AssetTruthLocation::SourceFile) {
				JsonSerializer serializer { *md.SourcePath };
				auto* asset_serializer = AssetSerializationSystem::get(md.Type);
				if (!asset_serializer) {
					ORG_ERROR("AssetDatabase::save_assets: no serializer for asset type {}", magic_enum::enum_name(md.Type));
					continue;
				}

				asset_serializer->serialize(asset, serializer);

			serializer.save_to_file();
		} else if (truth == AssetTruthLocation::ImportPayload) {
			write_import_file(uuid);
		}
	}
}

std::filesystem::path AssetDatabase::get_import_path(const AssetMetadata& meta) {
	if (meta.SourcePath) {
		return meta.SourcePath->string() + ".import";
	}

	return PathContextService::get_instance().project().generated_assets_root() / "generated" / ((*meta.ID).to_string() + ".import");
}

const AssetMetadata& AssetDatabase::get_metadata(const UUID& id) { return m_metadata[id]; }

}
