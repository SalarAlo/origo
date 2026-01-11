#pragma once

#include "origo/assets/Metadata.h"
#include "origo/assets/importers/IAssetImporter.h"
#include <filesystem>

namespace Origo {

class AssetImportPipeline {
public:
	static void RunInitialImport();

private:
	static bool IsImportCandidate(const std::filesystem::directory_entry& entry);
	static IAssetImporter* ResolveImporter(const std::filesystem::path& path);
	static Scope<AssetMetadata> LoadOrCreateMetadata(const std::filesystem::path& path, IAssetImporter* importer);
	static bool IsImportNecessary(const std::filesystem::path& path, const AssetMetadata& meta);
	static void ImportAsset(const std::filesystem::path& path, IAssetImporter* importer, AssetMetadata& meta, int& importCount);
	static void LoadCachedAsset(const std::filesystem::path& importFile, AssetMetadata& meta);
};

}
