#include <chrono>
#include <filesystem>

#include "origo/assets/importers/AssetPipeline.h"

#include "origo/assets/AssetDatabase.h"
#include "origo/assets/AssetFactory.h"
#include "origo/assets/AssetManager.h"

#include "origo/assets/importers/AssetImporterRegistry.h"
#include "origo/assets/importers/IAssetImporter.h"

#include "origo/assets/serialization/AssetSerializer.h"

#include "origo/core/Logger.h"
#include "origo/core/PathContext.h"

#include "origo/serialization/JsonSerializer.h"

namespace Origo {

void AssetImportPipeline::run_initial_import() {
	using clock = std::chrono::steady_clock;
	const std::filesystem::path root_asset_dir = PathContextService::get_instance().project().resources_root();
	const auto started_at = clock::now();

	size_t scanned_files = 0;
	size_t imported_assets = 0;
	size_t cached_assets = 0;
	size_t authored_assets = 0;
	size_t skipped_assets = 0;

	ORG_CORE_INFO("[PIPELINE] Starting initial import scan at {}", root_asset_dir.string());

	if (!std::filesystem::exists(root_asset_dir)) {
		ORG_CORE_WARN("[PIPELINE] No directory for resources found.");
		return;
	}

	for (auto& asset_file : std::filesystem::recursive_directory_iterator(root_asset_dir)) {
		if (!is_import_candidate(asset_file)) {
			continue;
		}
		++scanned_files;

		const auto& asset_path = asset_file.path();
		const auto source_timestamp = asset_file.last_write_time();

		IAssetImporter* asset_importer = resolve_importer(asset_path);

		if (!asset_importer) {
			++skipped_assets;
			ORG_CORE_TRACE("[SCAN] No importer for {}", asset_path.string());
			continue;
		}

		auto load_result = load_or_create_metadata(asset_path, source_timestamp, asset_importer);
		auto& meta = load_result.Metadata;

		auto truth = asset_importer->get_truth_location();

		if (truth == AssetTruthLocation::ImportPayload) {
			if (is_import_necessary(load_result.ImportFileExists, *meta, asset_importer)) {
				++imported_assets;
				import_asset(asset_path, asset_importer, *meta);
			} else {
				++cached_assets;
				load_cached_asset(asset_path.string() + ".import", *meta, *asset_importer);
			}
		} else {
			++authored_assets;
			register_authored_asset(asset_path, *asset_importer, *meta);
		}
	}

	AssetManager::get_instance().resolve_all();

	const auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(clock::now() - started_at).count();
	ORG_CORE_INFO(
	    "[PIPELINE] Initial import complete in {} ms (scanned={}, imported={}, cached={}, authored={}, skipped={})",
	    elapsed_ms,
	    scanned_files,
	    imported_assets,
	    cached_assets,
	    authored_assets,
	    skipped_assets);
}

bool AssetImportPipeline::is_import_candidate(const std::filesystem::directory_entry& entry) {
	if (!entry.is_regular_file())
		return false;

	return entry.path().extension() != ".import";
}

IAssetImporter* AssetImportPipeline::resolve_importer(const std::filesystem::path& path) {
	return AssetImporterRegistry::get_instance().get_importer(path);
}

bool AssetImportPipeline::is_import_necessary(bool import_file_exists, const AssetMetadata& meta, IAssetImporter* importer) {
	if (importer->get_truth_location() == AssetTruthLocation::SourceFile)
		return false;

	if (!import_file_exists)
		return true;

	if (meta.ImportedTimestamp < meta.SourceTimestamp)
		return true;

	return false;
}

void AssetImportPipeline::import_asset(const std::filesystem::path& path, IAssetImporter* importer, AssetMetadata& meta) {
	ORG_CORE_WARN("[IMPORT] Importing {}", path.string());

	Scope<Asset> asset = importer->import(path, meta);
	meta.ImportedTimestamp = meta.SourceTimestamp;
	AssetFactory::get_instance().create_imported_asset(meta, std::move(asset));

	ORG_CORE_INFO("[IMPORT] Writing import file UUID={}", meta.ID->to_string());

	AssetDatabase::get_instance().register_metadata(meta);
	AssetDatabase::get_instance().write_import_file(*meta.ID);
}

void AssetImportPipeline::load_cached_asset(const std::filesystem::path& importFile, AssetMetadata& meta, const IAssetImporter& importer) {
	auto asset_serializer = AssetSerializationSystem::get(meta.Type);
	if (!asset_serializer) {
		ORG_ERROR("AssetImportPipeline::load_cached_asset: no serializer for asset type {}", magic_enum::enum_name(meta.Type));
		return;
	}

	JsonSerializer serializer { importFile.c_str() };
	serializer.load_file();

	auto asset = AssetFactory::get_instance().allocate_hollow_asset(meta.Type);

	ORG_CORE_TRACE("Beginning deserilaization for {} of type {} and path {}", meta.Name, magic_enum::enum_name(meta.Type), meta.SourcePath->c_str());

	if (importer.get_truth_location() == AssetTruthLocation::ImportPayload) {
		serializer.begin_object("payload");
		asset_serializer->deserialize(serializer, *asset.get());
		serializer.end_object();
	} else {
		JsonSerializer source { meta.SourcePath->string().c_str() };
		source.load_file();

		asset_serializer->deserialize(source, *asset.get());
	}

	AssetFactory::get_instance().create_imported_asset(meta, std::move(asset));
	AssetDatabase::get_instance().register_metadata(meta);
}

AssetImportPipeline::MetadataLoadResult AssetImportPipeline::load_or_create_metadata(
    const std::filesystem::path& sourcePath,
    std::filesystem::file_time_type source_timestamp,
    IAssetImporter* importer) {
	const std::filesystem::path import_path { sourcePath.string() + ".import" };
	MetadataLoadResult result;
	result.ImportFileExists = std::filesystem::exists(import_path);

	if (result.ImportFileExists) {
		result.Metadata = make_scope<AssetMetadata>(AssetDatabase::get_instance().load_import_header(import_path));
	} else {
		result.Metadata = make_scope<AssetMetadata>();
		result.Metadata->ID = UUID::generate();
		result.Metadata->Name = sourcePath.stem().string();
		result.Metadata->Type = importer->get_asset_type();
		result.Metadata->Origin = importer->get_origin();
		result.Metadata->SourcePath = sourcePath;

		using sys = std::chrono::system_clock;
		result.Metadata->ImportedTimestamp = std::chrono::clock_cast<std::filesystem::file_time_type::clock>(
		    sys::from_time_t(0));
	}

	result.Metadata->SourcePath = sourcePath;
	result.Metadata->SourceTimestamp = source_timestamp;
	return result;
}
void AssetImportPipeline::register_authored_asset(const std::filesystem::path& path, const IAssetImporter& importer, AssetMetadata& meta) {
	AssetDatabase::get_instance().register_metadata(meta);
	AssetDatabase::get_instance().write_import_file(*meta.ID);

	auto asset = AssetFactory::get_instance().allocate_hollow_asset(meta.Type);

	auto serializer = AssetSerializationSystem::get(meta.Type);
	if (!serializer) {
		ORG_ERROR("AssetImportPipeline::register_authored_asset: no serializer for asset type {}", magic_enum::enum_name(meta.Type));
		return;
	}

	JsonSerializer source { path.c_str() };
	source.load_file();

	serializer->deserialize(source, *asset.get());
	AssetFactory::get_instance().create_imported_asset(meta, std::move(asset));
}

}
