#pragma once

#include <filesystem>

#include "origo/assets/Metadata.h"

#include "origo/assets/importers/IAssetImporter.h"

#include "origo/utils/Singleton.h"

namespace Origo {

class AssetImportPipeline : public Singleton<AssetImportPipeline> {
public:
	void run_initial_import();

private:
	bool is_import_candidate(const std::filesystem::directory_entry& entry);
	IAssetImporter* resolve_importer(const std::filesystem::path& path);
	Scope<AssetMetadata> load_or_create_metadata(const std::filesystem::path& path, IAssetImporter* importer);
	bool is_import_necessary(const std::filesystem::path& path, const AssetMetadata& meta, IAssetImporter* importer);
	void import_asset(const std::filesystem::path& path, IAssetImporter* importer, AssetMetadata& meta);
	void load_cached_asset(const std::filesystem::path& importFile, AssetMetadata& meta, const IAssetImporter& importer);
	void register_authored_asset(const std::filesystem::path& path, const IAssetImporter& importer, AssetMetadata& meta);
};

}
