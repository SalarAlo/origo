#pragma once

#include "origo/assets/Metadata.h"
#include "origo/assets/importers/IAssetImporter.h"
#include <filesystem>

namespace Origo {

class AssetImportPipeline {
public:
	static void RunInitialImport();
	static bool HasRunInitialImport() { return s_InitialImportComplete; }

private:
	static bool IsImportCandidate(const std::filesystem::directory_entry& entry);
	static IAssetImporter* ResolveImporter(const std::filesystem::path& path);
	static Scope<AssetMetadata> LoadOrCreateMetadata(const std::filesystem::path& path, IAssetImporter* importer);
	static bool IsImportNecessary(const std::filesystem::path& path, const AssetMetadata& meta);
	static void ImportAsset(const std::filesystem::path& path, IAssetImporter* importer, AssetMetadata& meta, int& importCount);
	static void LoadCachedAsset(const std::filesystem::path& importFile, AssetMetadata& meta);

private:
	inline static bool s_InitialImportComplete;
};

}
