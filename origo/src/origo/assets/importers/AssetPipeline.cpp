#include <filesystem>

#include "origo/assets/importers/AssetPipeline.h"

#include "origo/assets/AssetDatabase.h"
#include "origo/assets/AssetFactory.h"
#include "origo/assets/AssetManager.h"

#include "origo/assets/importers/AssetImporterRegistry.h"
#include "origo/assets/importers/IAssetImporter.h"

#include "origo/assets/serialization/AssetSerializer.h"

#include "origo/core/Logger.h"

#include "origo/serialization/JsonSerializer.h"

namespace Origo {

void AssetImportPipeline::run_initial_import() {
	std::filesystem::path root_asset_dir { "./resources" };

	ORG_CORE_INFO("[PIPELINE] Starting initial import scan at {}", root_asset_dir.string());

	if (!std::filesystem::exists(root_asset_dir)) {
		ORG_CORE_WARN("[PIPELINE] No directory for resources found.");
		return;
	}

	for (auto& asset_file : std::filesystem::recursive_directory_iterator(root_asset_dir)) {

		ORG_CORE_TRACE("[SCAN] Found {}", asset_file.path().string());

		if (!is_import_candidate(asset_file)) {
			ORG_CORE_TRACE("[SCAN] Skipping (not import candidate)");
			continue;
		}

		const auto& asset_path = asset_file.path();

		IAssetImporter* asset_importer = resolve_importer(asset_path);

		if (!asset_importer) {
			ORG_CORE_WARN("[SCAN] No importer for {}", asset_path.string());
			continue;
		}

		ORG_CORE_INFO("[SCAN] Importer found for {}", asset_path.string());

		auto meta = load_or_create_metadata(asset_path, asset_importer);

		ORG_CORE_INFO("[META] UUID={} TYPE={} ORIGIN={}",
		    meta->ID->to_string(),
		    magic_enum::enum_name(meta->Type),
		    magic_enum::enum_name(meta->Origin));

		auto truth = asset_importer->get_truth_location();

		ORG_CORE_INFO("[PIPELINE] TruthLocation = {}",
		    truth == AssetTruthLocation::ImportPayload ? "ImportPayload" : "SourceFile");

		if (truth == AssetTruthLocation::ImportPayload) {
			if (is_import_necessary(asset_path, *meta, asset_importer)) {
				ORG_CORE_INFO("[PIPELINE] Import required for {}", asset_path.string());
				import_asset(asset_path, asset_importer, *meta);
			} else {
				ORG_CORE_INFO("[PIPELINE] Loading cached {}", asset_path.string());
				load_cached_asset(asset_path.string() + ".import", *meta, *asset_importer);
			}
		} else {
			ORG_CORE_INFO("[PIPELINE] Registering authored asset {}", asset_path.string());
			register_authored_asset(asset_path, *asset_importer, *meta);
		}
	}

	ORG_CORE_INFO("[PIPELINE] Resolving all assets");
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

bool AssetImportPipeline::is_import_necessary(const std::filesystem::path& path, const AssetMetadata& meta, IAssetImporter* importer) {
	if (importer->get_truth_location() == AssetTruthLocation::SourceFile)
		return false;

	std::filesystem::path import_file { path.string() + ".import" };

	if (!std::filesystem::exists(import_file))
		return true;

	if (meta.ImportedTimestamp < meta.SourceTimestamp)
		return true;

	return false;
}

void AssetImportPipeline::import_asset(const std::filesystem::path& path, IAssetImporter* importer, AssetMetadata& meta) {
	ORG_CORE_WARN("[IMPORT] Importing {}", path.string());

	Scope<Asset> asset = importer->import(path, meta);

	AssetManager::get_instance().register_asset(std::move(asset), meta.ID);
	meta.ImportedTimestamp = meta.SourceTimestamp;

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

Scope<AssetMetadata> AssetImportPipeline::load_or_create_metadata(const std::filesystem::path& sourcePath, IAssetImporter* importer) {
	const std::filesystem::path import_path { sourcePath.string() + ".import" };
	Scope<AssetMetadata> meta;

	ORG_CORE_INFO("[META] Checking {}", import_path.string());

	if (std::filesystem::exists(import_path)) {
		ORG_CORE_INFO("[META] Found existing import file");
		meta = make_scope<AssetMetadata>(AssetDatabase::get_instance().load_import_header(import_path));
	} else {
		ORG_CORE_TRACE("[META] Creating new metadata for {}", sourcePath.string());

		meta = make_scope<AssetMetadata>();
		meta->ID = UUID::generate();
		meta->Name = sourcePath.stem().string();
		meta->Type = importer->get_asset_type();
		meta->Origin = importer->get_origin();
		meta->SourcePath = sourcePath;

		if (std::filesystem::exists(sourcePath)) {
			meta->SourceTimestamp = std::filesystem::last_write_time(sourcePath);
			ORG_CORE_INFO("[META] SourceTimestamp set");
		}

		using sys = std::chrono::system_clock;
		meta->ImportedTimestamp = std::chrono::clock_cast<std::filesystem::file_time_type::clock>(
		    sys::from_time_t(0));

		ORG_CORE_INFO("[META] New UUID {}", meta->ID->to_string());
	}

	return meta;
}
void AssetImportPipeline::register_authored_asset(const std::filesystem::path& path, const IAssetImporter& importer, AssetMetadata& meta) {
	ORG_CORE_TRACE("[REGISTER] Authored asset {}", path.string());

	if (std::filesystem::exists(path)) {
		meta.SourceTimestamp = std::filesystem::last_write_time(path);
		ORG_CORE_INFO("[REGISTER] SourceTimestamp refreshed");
	}

	ORG_CORE_INFO("[REGISTER] Registering metadata UUID={}", meta.ID->to_string());
	AssetDatabase::get_instance().register_metadata(meta);

	std::filesystem::path import_path = path.string() + ".import";
	ORG_CORE_TRACE("[REGISTER] Attempting write {}", import_path.string());

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

	ORG_CORE_INFO("[REGISTER] Creating runtime asset");
	AssetFactory::get_instance().create_imported_asset(meta, std::move(asset));
}

}
