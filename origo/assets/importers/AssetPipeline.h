#pragma once

#include <chrono>
#include <filesystem>

#include "origo/assets/Metadata.h"

#include "origo/assets/importers/IAssetImporter.h"

#include "origo/utils/Singleton.h"

namespace Origo {

class AssetImportPipeline : public Singleton<AssetImportPipeline> {
public:
	void run_initial_import();

private:
	struct MetadataLoadResult {
		Scope<AssetMetadata> Metadata;
		bool ImportFileExists { false };
	};

	bool is_import_candidate(const std::filesystem::directory_entry& entry);
	IAssetImporter* resolve_importer(const std::filesystem::path& path);
	MetadataLoadResult load_or_create_metadata(
	    const std::filesystem::path& path,
	    std::filesystem::file_time_type source_timestamp,
	    IAssetImporter* importer);
	bool is_import_necessary(bool import_file_exists, const AssetMetadata& meta, IAssetImporter* importer);
	void import_asset(const std::filesystem::path& path, IAssetImporter* importer, AssetMetadata& meta);
	void load_cached_asset(const std::filesystem::path& importFile, AssetMetadata& meta, const IAssetImporter& importer);
	void register_authored_asset(const std::filesystem::path& path, const IAssetImporter& importer, AssetMetadata& meta);
};

}
