#pragma once

#include "origo/assets/Metadata.h"
#include "origo/assets/importers/IAssetImporter.h"
#include <filesystem>

namespace Origo {

class AssetImportPipeline {
public:
	static void run_initial_import();

private:
	static bool is_import_candidate(const std::filesystem::directory_entry& entry);
	static IAssetImporter* resolve_importer(const std::filesystem::path& path);
	static Scope<AssetMetadata> load_or_create_metadata(const std::filesystem::path& path, IAssetImporter* importer);
	static bool is_import_necessary(const std::filesystem::path& path, const AssetMetadata& meta);
	static void import_asset(const std::filesystem::path& path, IAssetImporter* importer, AssetMetadata& meta, int& importCount);
	static void load_cached_asset(const std::filesystem::path& importFile, AssetMetadata& meta);
};

}
