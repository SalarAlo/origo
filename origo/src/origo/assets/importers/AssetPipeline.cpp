#include <filesystem>

#include "origo/assets/importers/AssetPipeline.h"

#include "origo/assets/AssetDatabase.h"
#include "origo/assets/AssetFactory.h"
#include "origo/assets/AssetManager.h"

#include "origo/assets/importers/AssetImporterRegistry.h"

#include "origo/assets/serialization/AssetSerializer.h"

#include "origo/core/Logger.h"

#include "origo/serialization/JsonSerializer.h"

namespace Origo {

void AssetImportPipeline::run_initial_import() {
	std::filesystem::path root_asset_dir { "./resources" };

	if (!std::filesystem::exists(root_asset_dir)) {
		ORG_CORE_WARN("No directory for resources found.");
		return;
	}

	int import_count {};

	for (auto& asset_file : std::filesystem::recursive_directory_iterator(root_asset_dir)) {
		if (!is_import_candidate(asset_file))
			continue;

		const auto& asset_path = asset_file.path();

		IAssetImporter* asset_importer = resolve_importer(asset_path);
		if (!asset_importer)
			continue;

		auto meta = load_or_create_metadata(asset_path, asset_importer);

		if (is_import_necessary(asset_path, *meta)) {
			import_asset(asset_path, asset_importer, *meta, import_count);
		} else {
			load_cached_asset(asset_path.string() + ".import", *meta);
		}
	}

	ORG_CORE_TRACE("Initial import complete. {} assets imported.", import_count);

	AssetManager::get_instance().resolve_all();
}

bool AssetImportPipeline::is_import_candidate(const std::filesystem::directory_entry& entry) {
	if (!entry.is_regular_file())
		return false;

	return entry.path().extension() != ".import";
}

IAssetImporter* AssetImportPipeline::resolve_importer(const std::filesystem::path& path) {
	return AssetImporterRegistry::get_instance().get_importer(path);
}

bool AssetImportPipeline::is_import_necessary(const std::filesystem::path& path, const AssetMetadata& meta) {
	std::filesystem::path import_file { path.string() + ".import" };

	if (!std::filesystem::exists(import_file))
		return true;

	if (meta.ImportedTimestamp < meta.SourceTimestamp)
		return true;

	return false;
}

void AssetImportPipeline::import_asset(const std::filesystem::path& path, IAssetImporter* importer, AssetMetadata& meta, int& importCount) {
	Scope<Asset> asset = importer->import(path, meta);

	AssetManager::get_instance().register_asset(std::move(asset), meta.ID);
	meta.ImportedTimestamp = meta.SourceTimestamp;

	AssetDatabase::get_instance().register_metadata(meta);
	AssetDatabase::get_instance().write_import_file(*meta.ID);

	importCount++;
}

void AssetImportPipeline::load_cached_asset(const std::filesystem::path& importFile, AssetMetadata& meta) {
	auto asset_serializer = AssetSerializationSystem::get(meta.Type);

	JsonSerializer serializer { importFile.c_str() };
	serializer.load_file();

	auto asset = AssetFactory::get_instance().allocate_hollow_asset(meta.Type);

	ORG_CORE_TRACE("Beginning deserilaization for {} of type {} and path {}", meta.Name, magic_enum::enum_name(meta.Type), meta.SourcePath.c_str());

	serializer.begin_object("payload");
	asset_serializer->deserialize(serializer, *asset.get());
	serializer.end_object();

	AssetFactory::get_instance().create_imported_asset(meta, std::move(asset));
	AssetDatabase::get_instance().register_metadata(meta);
}

Scope<AssetMetadata> AssetImportPipeline::load_or_create_metadata(const std::filesystem::path& sourcePath, IAssetImporter* importer) {
	const std::filesystem::path import_path { sourcePath.string() + ".import" };
	Scope<AssetMetadata> meta;

	if (std::filesystem::exists(import_path)) {
		meta = MakeScope<AssetMetadata>(AssetDatabase::get_instance().load_import_header(import_path));
	} else {
		meta = MakeScope<AssetMetadata>();
		meta->ID = UUID::generate();
		meta->Name = sourcePath.stem().string();
		meta->Type = importer->get_asset_type();
		meta->Origin = AssetOrigin::Imported;
		meta->SourcePath = sourcePath;

		if (std::filesystem::exists(sourcePath))
			meta->SourceTimestamp = std::filesystem::last_write_time(sourcePath);

		using sys = std::chrono::system_clock;
		meta->ImportedTimestamp = std::chrono::clock_cast<std::filesystem::file_time_type::clock>(
		    sys::from_time_t(0));
	}

	return meta;
}

}
