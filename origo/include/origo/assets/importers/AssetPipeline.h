#pragma once

#include "origo/assets/AssetMetadata.h"
#include "origo/assets/importers/IAssetImporter.h"

namespace Origo {

class AssetPipeline {
public:
	static void RunInitialImport();

private:
	static Scope<AssetMetadata> LoadOrCreateMetadata(const std::filesystem::path& path, IAssetImporter* importer);
	static bool CreateAssetFileIfMissing(const std::filesystem::path& path, IAssetImporter* importer, const AssetMetadata& metaData);
};

}
